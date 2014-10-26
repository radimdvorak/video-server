/*
 * CvVideoCapture.cpp
 *
 *  Created on: 27. 7. 2014
 *      Author: dron
 */

#include "CvVideoCapture.h"

CvVideoCapture::CvVideoCapture(int device) {
	vc.reset(new cv::VideoCapture());
	vc->open(device);

	inSize.width = static_cast<std::size_t>(vc->get(CV_CAP_PROP_FRAME_WIDTH));
	inSize.height = static_cast<std::size_t>(vc->get(CV_CAP_PROP_FRAME_HEIGHT));
}

CvVideoCapture::~CvVideoCapture() {
	if (vc.get()) {
		vc->release();
	}
}

cv::VideoCapture& CvVideoCapture::videoCapture() {
	return *vc;
}

cv::Size CvVideoCapture::getInputSize() const {
	return inSize;
}

std::size_t CvVideoCapture::getInputFPS() const {
	return static_cast<std::size_t>(vc->get(CV_CAP_PROP_FPS));
}

AVPixelFormat CvVideoCapture::getAVPixelFormat() const {
	return AV_PIX_FMT_BGR24;
}
