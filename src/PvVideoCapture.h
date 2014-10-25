/*
 * PvVideoCapture.h
 *
 *  Created on: 26. 7. 2014
 *      Author: dron
 */

#ifndef PVVIDEOCAPTURE_H_
#define PVVIDEOCAPTURE_H_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <string>
#include <vector>
#include <Poco/Mutex.h>
#include <opencv2/opencv.hpp>
#include "pvapi/PvApi.h"
#include "libavutil/pixfmt.h"

#ifdef _WIN32
#define _STDCALL __stdcall
#else
#define _STDCALL
#define TRUE     0
#endif

class PvVideoCapture {
		typedef std::vector<tPvFrame> FrameList;

	public:
		struct Setup {
			std::size_t framesCount;

			Setup(std::size_t framesCount);
		};

		PvVideoCapture(const Setup& setup);
		virtual ~PvVideoCapture();

		// wait for a camera to be plugged in
		static void waitForCamera();
		// wait until ctrl+C abort, or cam unplugged
		void waitForEver();
		// ends wait functions
		static void abort();
		static bool aborted();

		cv::Size getInputSize() const;
		std::size_t getInputFPS() const;
		AVPixelFormat getAVPixelFormat() const;

		// setup and start streaming
		// throw runtime_error on failure
		void start();

		// stop streaming
		void stop();

		PvVideoCapture& operator >> (cv::Mat& image);

		friend void _STDCALL frameDoneCB(tPvFrame* frame);

	private:
		static void pvInit();
		void getCamera(std::size_t& uid);

	private:
		Setup setup;

		static PvVideoCapture* instance;

		std::size_t uid;
		tPvHandle handle;

		cv::Size inSize;
		float inputFPS;

		FrameList frames;
		FrameList::iterator currFrame;
		bool currFrameDelivered;
		static Poco::Mutex mutex;

		bool stopped;
		bool stopping;
		static bool _abort;
		static bool pvInitialized;
};

#endif /* PVVIDEOCAPTURE_H_ */
