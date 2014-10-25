/*
 * CvVideoCapture.h
 *
 *  Created on: 27. 7. 2014
 *      Author: dron
 */

#ifndef CVVIDEOCAPTURE_H_
#define CVVIDEOCAPTURE_H_

#include <memory>
#include <opencv2/opencv.hpp>
#include "libavutil/pixfmt.h"

class CvVideoCapture {
	public:
		CvVideoCapture(int device);
		virtual ~CvVideoCapture();

		cv::VideoCapture& videoCapture();

		cv::Size getInputSize() const;
		std::size_t getInputFPS() const;
		AVPixelFormat getAVPixelFormat() const;

	private:
		std::unique_ptr<cv::VideoCapture> vc;

		cv::Size inSize;
};

#endif /* CVVIDEOCAPTURE_H_ */
