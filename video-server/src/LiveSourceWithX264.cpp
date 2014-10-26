/*
 * LiveSourceWithX264.cpp
 *
 *  Created on: 29. 6. 2014
 *      Author: dron
 */

#include "LiveSourceWithX264.h"
#include <GroupsockHelper.hh>
#include <stdexcept>
#include "libavutil/pixdesc.h"
#include "Utils/AVPixFormatHelper.h"

const std::string LiveSourceWithx264::Source_USB = "Source_USB";
const std::string LiveSourceWithx264::Source_PvAPI = "Source_PvAPI";
std::unique_ptr<CvVideoCapture> LiveSourceWithx264::cvVideoCapture;
std::unique_ptr<PvVideoCapture> LiveSourceWithx264::pvVideoCapture;
std::unique_ptr<X264Encoder> LiveSourceWithx264::encoder;

LiveSourceWithx264::Setup::Setup(
		UsageEnvironment& env, const std::string& source, std::size_t deviceNum,
		const cv::Size& outSize) :
	env(env), source(source), deviceNum(deviceNum),
	outSize(outSize) {
}

LiveSourceWithx264* LiveSourceWithx264::createNew(const Setup& setup) {
	return new LiveSourceWithx264(setup);
}

EventTriggerId LiveSourceWithx264::eventTriggerId = 0;

unsigned LiveSourceWithx264::referenceCount = 0;

LiveSourceWithx264::LiveSourceWithx264(const Setup& setup) :
		FramedSource(setup.env), setup(setup), frameCount(0), avPixelFormat(NULL) {

	++referenceCount;
	if (setup.source == Source_USB) {
		if (not cvVideoCapture.get()) {
			cvVideoCapture.reset(new CvVideoCapture(setup.deviceNum));
			cvVideoCapture->videoCapture().set(CV_CAP_PROP_FPS, 25);
		}
	}
	if (setup.source == Source_PvAPI) {
		if (not pvVideoCapture.get()) {
			//PvVideoCapture::waitForCamera();
			pvVideoCapture.reset(new PvVideoCapture(PvVideoCapture::Setup(4)));
			pvVideoCapture->start();
		}
	}

	if (eventTriggerId == 0) {
		eventTriggerId = envir().taskScheduler().createEventTrigger(deliverFrame0);
	}
}

LiveSourceWithx264::~LiveSourceWithx264(void) {
	--referenceCount;
	envir().taskScheduler().deleteEventTrigger(eventTriggerId);
	eventTriggerId = 0;
}

void LiveSourceWithx264::abort() {
	if (pvVideoCapture.get()) {
		pvVideoCapture->abort();
	}
}

X264Encoder* LiveSourceWithx264::createEncoder(AVPixelFormat avPixelFormat) {
	X264Encoder* encoder = NULL;

	X264Encoder::Setup encoderSetup(AV_PIX_FMT_NONE, cv::Size(0, 0), 0, setup.outSize);
	if (cvVideoCapture.get()) {
		encoderSetup.inSize = cvVideoCapture->getInputSize();
		encoderSetup.inFPS = cvVideoCapture->getInputFPS();
	}
	if (pvVideoCapture.get()) {
		encoderSetup.inSize = pvVideoCapture->getInputSize();
		encoderSetup.inFPS = pvVideoCapture->getInputFPS();
	}

	if (avPixelFormat != AV_PIX_FMT_NONE) {
		encoderSetup.inColorSpace = avPixelFormat;
		encoder = new X264Encoder(encoderSetup);
		std::cout << "Input  camera size: " << encoderSetup.inSize.width << "x" << encoderSetup.inSize.height << std::endl;
		std::cout << "Input FPS: " << encoderSetup.inFPS << std::endl;
		std::cout << "Output camera size: " << encoderSetup.outSize.width << "x" << encoderSetup.outSize.height << std::endl;
		std::cout << "AV Pixel format: " << AVPixFormatHelper::pixFormatName(avPixelFormat) << std::endl;
	}

	return encoder;
}

void LiveSourceWithx264::encodeNewFrame() {
	AVPixelFormat avPixelFormat = AV_PIX_FMT_NONE;

	rawImage = cv::Mat();
	while (rawImage.data == NULL) {
		if (cvVideoCapture.get()) {
			cvVideoCapture->videoCapture() >> rawImage;
			avPixelFormat = cvVideoCapture->getAVPixelFormat();
		} else if (pvVideoCapture.get()) {
			(*pvVideoCapture) >> rawImage;
			avPixelFormat = pvVideoCapture->getAVPixelFormat();
		}
	}

	// Got new image to stream
	assert(rawImage.data != NULL);

	if (not encoder.get()) {
		encoder.reset(createEncoder(avPixelFormat));
	}

	if (encoder.get()) {
		encoder->encodeFrame(rawImage);

		// Take all nals from encoder output queue to our input queue
		while (encoder->isNalsAvailableInOutputQueue()) {
			x264_nal_t nal = encoder->getNalUnit();
			nalQueue.push(nal);
		}
	}
}

void LiveSourceWithx264::deliverFrame0(void* clientData) {
	((LiveSourceWithx264*) clientData)->deliverFrame();
}

void LiveSourceWithx264::doGetNextFrame() {
	if (nalQueue.empty()) {
		encodeNewFrame();
		gettimeofday(&currentTime, NULL);
		deliverFrame();
//		std::cout << "FPS: " << ++frameCount / (timestamp.elapsed() / 1000000.0) << std::endl;
		if (frameCount > 100) {
			frameCount = 0;
			timestamp.update();
		}
	} else {
		deliverFrame();
	}
}

void LiveSourceWithx264::deliverFrame() {
	if (!isCurrentlyAwaitingData()) {
		return;
	}
	if (nalQueue.empty()) {
		return;
	}
	x264_nal_t nal = nalQueue.front();
	nalQueue.pop();
	assert(nal.p_payload != NULL);
	// You need to remove the start code which is there in front of every nal unit.
	// the start code might be 0x00000001 or 0x000001. so detect it and remove it. pass remaining data to live555
	int trancate = 0;
	if (nal.i_payload >= 4 && nal.p_payload[0] == 0 && nal.p_payload[1] == 0 && nal.p_payload[2] == 0 && nal.p_payload[3] == 1) {
		trancate = 4;
	} else {
		if (nal.i_payload >= 3 && nal.p_payload[0] == 0 && nal.p_payload[1] == 0 && nal.p_payload[2] == 1) {
			trancate = 3;
		}
	}

	if (nal.i_payload - trancate > static_cast<int>(fMaxSize)) {
		fFrameSize = fMaxSize;
		fNumTruncatedBytes = nal.i_payload - trancate - fMaxSize;
	} else {
		fFrameSize = nal.i_payload - trancate;
	}
	fPresentationTime = currentTime;
	memmove(fTo, nal.p_payload + trancate, fFrameSize);
	FramedSource::afterGetting(this);
}

