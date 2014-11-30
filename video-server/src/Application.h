/*
 * Application.h
 *
 *  Created on: 27. 7. 2014
 *      Author: dron
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <memory>
#include <string>

#include "BasicUsageEnvironment.hh"
#include "RTSPServer.hh"
#include "ServerMediaSession.hh"
#include "H264LiveServerMediaSession.h"

class Application {
		static volatile char terminate_;
		static void term();

	public:
		struct CameraSource {
			std::string source;
			std::size_t index; ///< index of camera, e.g. for CvCapture

			CameraSource(const std::string& source, std::size_t index)
				: source(source), index(index) {}
		};

		struct Setup {
				int port;
				std::string streamName;
				CameraSource cameraSource;

				cv::Size outSize;

				static Setup load(const std::string& iniFile);

			private:
				Setup();
		};


		Application(const Setup& setup);
		virtual ~Application();

		std::string getUrl() const;

		void loop();

	private:
		static void save(const std::string& iniFile, const Setup& setup);
	private:
		Setup setup;

		std::unique_ptr<TaskScheduler> taskScheduler;
		BasicUsageEnvironment* usageEnvironment;
		RTSPServer* rtspServer;
		ServerMediaSession* sms;
		H264LiveServerMediaSession *liveSubSession;
};

#endif /* APPLICATION_H_ */
