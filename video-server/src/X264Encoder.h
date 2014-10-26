
#ifndef X264Encoder_H_
#define X264Encoder_H_

#ifdef __cplusplus
#define __STDINT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#include <iostream>
#include <queue>
#include "opencv2/opencv.hpp"
#include <queue>
#include <stdint.h>

extern "C" {
	#include "x264.h"
	#include "ffmpeg/libswscale/swscale.h"
}

class X264Encoder {
	public:
		struct Setup {
			AVPixelFormat inColorSpace; ///< color space type of input image
			cv::Size inSize;
			std::size_t inFPS;

			cv::Size outSize;

			Setup(
				AVPixelFormat inColorSpace, const cv::Size& inSize, std::size_t inFPS,
				const cv::Size& outSize
			);
		};

		X264Encoder(const Setup& setup);
		~X264Encoder(void);

	public:
		void encodeFrame(cv::Mat& image);
		bool isNalsAvailableInOutputQueue();
		x264_nal_t getNalUnit();

	private:
		void initilize();
		void unInitilize();

	private:
		Setup setup;

		// Use this context to convert your BGR Image to YUV image since x264 do not support RGB input
		SwsContext* convertContext;
		std::queue<x264_nal_t> outputQueue;
		x264_param_t parameters;
		x264_picture_t picture_in, picture_out;
		x264_t* encoder;
};

#endif // X264Encoder_H_
