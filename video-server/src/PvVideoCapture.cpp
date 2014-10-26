/*
 * PvVideoCapture.cpp
 *
 *  Created on: 26. 7. 2014
 *      Author: dron
 */

#include "PvVideoCapture.h"
#include <Poco/Timestamp.h>
#include <Poco/Util/IniFileConfiguration.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "Utils/Conversion.h"

PvVideoCapture* PvVideoCapture::instance = NULL;
bool PvVideoCapture::_abort = false;
bool PvVideoCapture::pvInitialized = false;
Poco::Mutex PvVideoCapture::mutex;

void check(tPvErr err, const std::string& errPrefix) {
	if (err != ePvErrSuccess) {
		std::stringstream sstr;
		sstr << errPrefix << err;
		throw std::runtime_error(sstr.str());
	}
}

PvVideoCapture::Setup::Setup(std::size_t framesCount)
	: framesCount(framesCount) {
}

void PvVideoCapture::pvInit() {
	if (pvInitialized) {
		return;
	}

	tPvErr errCode;
	if ((errCode = PvInitialize()) != ePvErrSuccess) {
		std::stringstream sstr;
		sstr << "PvInitialize err: " << errCode;
		throw std::runtime_error(sstr.str());
	}
	pvInitialized = true;
}

PvVideoCapture::PvVideoCapture(const Setup& setup) :
		setup(setup), uid(0), handle(NULL),
		currFrameDelivered(false), stopped(true), stopping(false) {

	if (instance == NULL) {
		instance = this;
	} else {
		throw std::runtime_error("Only one instance of PvVideoCapture is possible");
	}

	tPvErr errCode;
	unsigned long frameSize = 0;

	pvInit();

	//open camera
	getCamera(uid);
	if ((errCode = PvCameraOpen(uid, ePvAccessMaster, &(handle))) != ePvErrSuccess) {
		std::stringstream sstr;
		if (errCode == ePvErrAccessDenied) {
			sstr << "Camera already open as Master, or camera wasn't properly closed and still waiting to HeartbeatTimeout.";
		} else {
			sstr << "PvCameraOpen err: " << errCode;
		}
		throw std::runtime_error(sstr.str());
	}

	// Initialise frame buffers
	check(PvAttrUint32Get(handle, "TotalBytesPerFrame", &frameSize), "CameraSetup: Get TotalBytesPerFrame err: ");
	frames.resize(setup.framesCount);
	for (std::size_t i = 0; i < frames.size(); ++i) {
		frames[i].ImageBuffer = new char[frameSize];
		frames[i].ImageBufferSize = frameSize;
	}
	currFrame = frames.end();

	tPvUint32 tmp = 0;
	check(PvAttrUint32Get(handle, "Width", &tmp), "CameraSetup: Get Width err: ");
	inSize.width = tmp;
	check(PvAttrUint32Get(handle, "Height", &tmp), "CameraSetup: Get Height err: ");
	inSize.height = tmp;
	check(PvAttrFloat32Get(handle, "FrameRate", &inputFPS), "CameraSetup: Get FrameRate err: ");
}

PvVideoCapture::~PvVideoCapture() {
	if (!stopped) {
		stop();
	}

	tPvErr errCode;

	if ((errCode = PvCameraClose(handle)) != ePvErrSuccess) {
		std::cout << "CameraUnSetup: PvCameraClose err: " << errCode << std::endl;
	} else {
		std::cout << "Camera closed" << std::endl;
	}

	// delete image buffers
	for (std::size_t i = 0; i < frames.size(); ++i) {
		delete[] (char*) frames[i].ImageBuffer;
	}

	handle = NULL;
	instance = NULL;

	PvUnInitialize();
}

cv::Size PvVideoCapture::getInputSize() const {
	return inSize;
}

std::size_t PvVideoCapture::getInputFPS() const {
	return static_cast<std::size_t>(std::round(inputFPS));
}

AVPixelFormat PvVideoCapture::getAVPixelFormat() const {
	Poco::Mutex::ScopedLock lock(PvVideoCapture::mutex);
	if (currFrame != frames.end()) {
		return convert(_PvImageFormat(currFrame->Format, currFrame->BayerPattern));
	}

	return AV_PIX_FMT_NONE;
}

// Frame completed callback executes on seperate driver thread.
// One callback thread per camera. If a frame callback function has not
// completed, and the next frame returns, the next frame's callback function is queued.
// This situation is best avoided (camera running faster than host can process frames).
// Spend as little time in this thread as possible and offload processing
// to other threads or save processing until later.
//
// Note: If a camera is unplugged, this callback will not get called until PvCaptureQueueClear.
// i.e. callback with pFrame->Status = ePvErrUnplugged doesn't happen -- so don't rely
// on this as a test for a missing camera.
void _STDCALL frameDoneCB(tPvFrame* frame) {
	//Do something with the frame.
	//E.g. display to screen, shift pFrame->ImageBuffer location for later usage , etc
	//Here we display FrameCount and Status
	if (frame->Status == ePvErrDataMissing) {
		//Possible improper network card settings. See GigE Installation Guide.
		std::cout << "Frame: " << frame->FrameCount << " dropped packets" << std::endl;
	} else if (frame->Status == ePvErrCancelled) {
		std::cout << "Frame cancelled " << frame->FrameCount << std::endl;
	} else if (frame->Status != ePvErrSuccess) {
		std::cout << "Frame: " << frame->FrameCount << " error: " << frame->Status << std::endl;
	}

	// if frame hasn't been cancelled, requeue frame
	if (frame->Status != ePvErrCancelled) {
		if (PvVideoCapture::instance) {
			if (PvVideoCapture::instance->stopping) {
				std::cout << "PvVideoCapture is stopping, frame ignored." << std::endl;
				return;
			}
			PvCaptureQueueFrame(PvVideoCapture::instance->handle, frame, frameDoneCB);
			Poco::Mutex::ScopedLock lock(PvVideoCapture::mutex);
			std::size_t idx = (frame->FrameCount-1) % PvVideoCapture::instance->frames.size();
			//std::cout << "Frame idx: " << idx << std::endl;
			PvVideoCapture::instance->currFrame = PvVideoCapture::instance->frames.begin() + idx;
			PvVideoCapture::instance->currFrameDelivered = false;

			static int frameCnt = 0;
			static Poco::Timestamp ts;
			std::cout << "FPS: " << ++frameCnt / (ts.elapsed() / 1000000.0) << std::endl;
			if (frameCnt > 100) {
				frameCnt = 0;
				ts.update();
			}
		} else {
			std::cerr << "frameDoneCB: Unexpected NULL PvVideoCapture instance." << std::endl;
		}
	}
}

void PvVideoCapture::waitForCamera() {
	pvInit();
	std::cout << "Waiting for a camera" << std::endl;
	Poco::Timestamp now;
	while ((PvCameraCount() == 0) && !_abort/* && now.elapsed() < 10*1000*1000*/) {
		Poco::Thread::sleep(250);
	}
	if (PvCameraCount() == 0) {
		std::cout << "No camera connected." << std::endl;
	}
}

void PvVideoCapture::waitForEver() {
	tPvUint32 exposureVal;

	//Lazy/poor way to check for unplugged camera. See StreamPnp for
	//registering a ePvLinkRemove callback.
	while ((PvAttrUint32Get(handle, "ExposureValue", &exposureVal) != ePvErrUnplugged) && !_abort) {
		Poco::Thread::sleep(500);
	}
}

void PvVideoCapture::abort() {
	_abort = true;
}

bool PvVideoCapture::aborted() {
	return _abort;
}

void PvVideoCapture::start() {
	tPvErr errCode = ePvErrSuccess;
	bool failed = false;

	// NOTE: This call sets camera PacketSize to largest sized test packet, up to 8228, that doesn't fail
	// on network card. Some MS VISTA network card drivers become unresponsive if test packet fails.
	// Use PvUint32Set(handle, "PacketSize", MaxAllowablePacketSize) instead. See network card properties
	// for max allowable PacketSize/MTU/JumboFrameSize.
	if ((errCode = PvCaptureAdjustPacketSize(handle, 8228)) != ePvErrSuccess) {
		std::stringstream sstr;
		sstr << "start: PvCaptureAdjustPacketSize err: " << errCode;
		throw std::runtime_error(sstr.str());
	}

	// start driver capture stream
	if ((errCode = PvCaptureStart(handle)) != ePvErrSuccess) {
		std::stringstream sstr;
		sstr << "start: PvCaptureStart err: " << errCode;
		throw std::runtime_error(sstr.str());
	}

	// queue frames with frameDoneCB callback function. Each frame can use a unique callback function
	// or, as in this case, the same callback function.
	for (std::size_t i = 0; i < frames.size() && !failed; i++) {
		tPvErr _errCode = ePvErrSuccess;
		if ((_errCode = PvCaptureQueueFrame(handle, &(frames[i]), frameDoneCB)) != ePvErrSuccess) {
			errCode = _errCode;
			// stop driver capture stream
			PvCaptureEnd(handle);
			failed = true;
		}
	}

	if (failed) {
		std::stringstream sstr;
		sstr << "start: PvCaptureQueueFrame err: " << errCode;
		throw std::runtime_error(sstr.str());
	}

	// set the camera in freerun trigger, continuous mode, and start camera receiving triggers
	if ((PvAttrEnumSet(handle, "FrameStartTriggerMode", "Freerun") != ePvErrSuccess)
			|| (PvAttrEnumSet(handle, "AcquisitionMode", "Continuous") != ePvErrSuccess) || (PvCommandRun(handle, "AcquisitionStart") != ePvErrSuccess)) {
		std::stringstream sstr;
		sstr << "start: failed to set camera attributes";
		// clear queued frame
		PvCaptureQueueClear(handle);
		// stop driver capture stream
		PvCaptureEnd(handle);
		throw std::runtime_error(sstr.str());
	}

	stopped = false;
}

void PvVideoCapture::stop() {
	stopping = true;
	tPvErr errCode;

	//stop camera receiving triggers
	if ((errCode = PvCommandRun(handle, "AcquisitionStop")) != ePvErrSuccess) {
		std::cout << "AcquisitionStop command err: " << errCode << std::endl;
	} else {
		std::cout << "AcquisitionStop success" << std::endl;
	}

	//PvCaptureQueueClear aborts any actively written frame with Frame.Status = ePvErrDataMissing
	//Further queued frames returned with Frame.Status = ePvErrCancelled

	//Add delay between AcquisitionStop and PvCaptureQueueClear
	//to give actively written frame time to complete
	Sleep(200);

	if ((errCode = PvCaptureQueueClear(handle)) != ePvErrSuccess) {
		std::cout << "PvCaptureQueueClear err: " << errCode << std::endl;
	} else {
		std::cout << "...Queue cleared" << std::endl;
	}

	//stop driver stream
	if ((errCode = PvCaptureEnd(handle)) != ePvErrSuccess) {
		std::cout << "PvCaptureEnd err: " << errCode << std::endl;
	} else {
		std::cout << "Driver stream stopped" << std::endl;
	}

	stopped = true;
	stopping = false;
}

void PvVideoCapture::getCamera(std::size_t& uid) {
	tPvUint32 connected = 0;
	tPvCameraInfoEx list;

	//regardless if connected > 1, we only set UID of first camera in list
	tPvUint32 count = PvCameraListEx(&list, 1, &connected, sizeof(tPvCameraInfoEx));
	if (count == 1) {
		uid = list.UniqueId;
		std::cout << "Got camera " << list.SerialNumber << std::endl;
	} else {
		throw std::runtime_error("Failed to find a camera");
	}
}

PvVideoCapture& PvVideoCapture::operator >> (cv::Mat& image) {
	Poco::Mutex::ScopedLock lock(mutex);
	if (currFrame != frames.end() && not currFrameDelivered) {
		//std::cout << "PvVideoCapture, retrieve frame idx: " << currFrame - frames.begin() << " with size: " << currFrame->ImageBufferSize << std::endl;
		uint8_t* buffer = static_cast<uint8_t*>(currFrame->ImageBuffer);
		std::vector<uint8_t> vec;
		vec.assign(buffer, buffer + currFrame->ImageBufferSize);
		image = cv::Mat(vec, true);
		currFrameDelivered = true;
	}

	return *this;
}

