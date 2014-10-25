#include "X264Encoder.h"
#include "libavcodec/avcodec.h"
#include "Utils/AVPixFormatHelper.h"

X264Encoder::Setup::Setup(
		AVPixelFormat inColorSpace, const cv::Size& inSize, std::size_t inFPS,
		const cv::Size& outSize) :
	inColorSpace(inColorSpace), inSize(inSize), inFPS(inFPS),
	outSize(outSize) {
}

X264Encoder::X264Encoder(const Setup& setup)
	: setup(setup), convertContext(NULL), encoder(NULL) {

	initilize();
}

X264Encoder::~X264Encoder(void) {
	unInitilize();
}

void X264Encoder::initilize() {
	// configure scaling for X264 first
	convertContext = sws_getContext(
			setup.inSize.width, setup.inSize.height, setup.inColorSpace, // input
			setup.outSize.width, setup.outSize.height, AV_PIX_FMT_YUV420P, // output
			SWS_FAST_BILINEAR, NULL, NULL, NULL);

	x264_param_default_preset(&parameters, "veryfast", "zerolatency");
	//parameters.i_log_level = X264_LOG_INFO;
	parameters.i_threads = 2;
	parameters.i_width = setup.outSize.width;
	parameters.i_height = setup.outSize.height;
	parameters.i_fps_num = setup.inFPS > 0 ? setup.inFPS : 25;
	parameters.i_fps_den = 1;
	parameters.i_keyint_max = parameters.i_fps_num;
	parameters.b_intra_refresh = 1;
	parameters.rc.i_rc_method = X264_RC_CRF;
	parameters.rc.i_vbv_buffer_size = 300000;
	parameters.rc.i_vbv_max_bitrate = 90000;
	parameters.rc.f_rf_constant = 25;
	parameters.rc.f_rf_constant_max = 35;
	parameters.i_sps_id = 7;
	// the following two value you should keep 1
	parameters.b_repeat_headers = 1;    // to get header before every I-Frame
	parameters.b_annexb = 1; // put start code in front of nal. we will remove start code later

	x264_param_apply_profile(&parameters, "high10");

	encoder = x264_encoder_open(&parameters);
	x264_picture_alloc(&picture_in, X264_CSP_I420, parameters.i_width, parameters.i_height);
	picture_in.i_type = X264_TYPE_AUTO;
	picture_in.img.i_csp = X264_CSP_I420;

}

void X264Encoder::unInitilize() {
	x264_picture_clean(&picture_in);
	x264_encoder_close(encoder);
	sws_freeContext(convertContext);
}

void X264Encoder::encodeFrame(cv::Mat& image) {
	int srcStride[] = {setup.inSize.width * 3, 0, 0, 0};
	if (setup.inColorSpace == AV_PIX_FMT_YUV422P) {
		srcStride[0] = setup.inSize.width;
		srcStride[1] = setup.inSize.width / 2;
		srcStride[2] = 0;
		srcStride[3] = setup.inSize.width / 2;
	}
	if (setup.inColorSpace == AV_PIX_FMT_BAYER_RGGB8) {
		srcStride[0] = setup.inSize.width;
		srcStride[1] = 0;
		srcStride[2] = 0;
		srcStride[3] = 0;
	}
	sws_scale(convertContext, &(image.data), srcStride, 0, setup.inSize.height, picture_in.img.plane, picture_in.img.i_stride);

	x264_nal_t* nals;
	int i_nals = 0;
	int frameSize = -1;
	frameSize = x264_encoder_encode(encoder, &nals, &i_nals, &picture_in, &picture_out);
	if (frameSize > 0) {
		for (int i = 0; i < i_nals; i++) {
			outputQueue.push(nals[i]);
		}
	}
}

bool X264Encoder::isNalsAvailableInOutputQueue() {
	if (outputQueue.empty() == true) {
		return false;
	} else {
		return true;
	}
}

x264_nal_t X264Encoder::getNalUnit() {
	x264_nal_t nal;
	nal = outputQueue.front();
	outputQueue.pop();
	return nal;
}
