/*
 * LiveSourceWithX264.h
 *
 *  Created on: 29. 6. 2014
 *      Author: dron
 */

#ifndef LIVESOURCEWITHX264_H_
#define LIVESOURCEWITHX264_H_

#include <queue>
#include <memory>
#include <Poco/Timestamp.h>
#include "X264Encoder.h"
#include <opencv2/opencv.hpp>
#include "FramedSource.hh"

#include "CvVideoCapture.h"
#include "PvVideoCapture.h"

class LiveSourceWithx264: public FramedSource {
	public:
		static const std::string Source_USB;
		static const std::string Source_PvAPI;

		struct Setup {
			UsageEnvironment& env;
			std::string source;
			std::size_t deviceNum; ///< for USB cameras

			cv::Size outSize;

			Setup(
				UsageEnvironment& env, const std::string& source, std::size_t deviceNum,
				const cv::Size& outSize
			);
		};

		static LiveSourceWithx264* createNew(const Setup& setup);
		static EventTriggerId eventTriggerId;

		void abort();

	protected:
		LiveSourceWithx264(const Setup& setup);
		virtual ~LiveSourceWithx264(void);

	private:
		virtual void doGetNextFrame();
		static void deliverFrame0(void* clientData);
		void deliverFrame();
		void encodeNewFrame();
		X264Encoder* createEncoder(AVPixelFormat inputPixelFormat);
		static unsigned referenceCount;

	private:
		Setup setup;

		std::queue<x264_nal_t> nalQueue;
		timeval currentTime;
		Poco::Timestamp timestamp;
		int frameCount;
		AVPixelFormat* avPixelFormat;

		static std::unique_ptr<CvVideoCapture> cvVideoCapture;
		static std::unique_ptr<PvVideoCapture> pvVideoCapture;
		cv::Mat rawImage;
		static std::unique_ptr<X264Encoder> encoder;
};

#endif /* LIVESOURCEWITHX264_H_ */
