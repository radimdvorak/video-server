
#ifndef H264LiveServerMediaSession_H_
#define H264LiveServerMediaSession_H_

#include "liveMedia.hh"
#include "OnDemandServerMediaSubsession.hh"
#include "LiveSourceWithX264.h"

class H264LiveServerMediaSession: public OnDemandServerMediaSubsession {
	public:
		struct Setup {
			UsageEnvironment& env;
			bool reuseFirstSource;
			std::string cameraSource;

			cv::Size outSize;

			Setup(
				UsageEnvironment& env, bool reuseFirstSource, const std::string& cameraSource,
				const cv::Size& outSize
			);
		};

		static H264LiveServerMediaSession* createNew(const Setup& setup);
		void checkForAuxSDPLine1();
		void afterPlayingDummy1();

		void abort();

	protected:
		H264LiveServerMediaSession(const Setup& setup);
		virtual ~H264LiveServerMediaSession(void);
		void setDoneFlag() {
			fDoneFlag = ~0;
		}
	protected:
		virtual char const* getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource);
		virtual FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);
		virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);
	private:
		Setup setup;

		char* fAuxSDPLine;
		char fDoneFlag;
		RTPSink* fDummySink;

		LiveSourceWithx264* source;
};

#endif // H264LiveServerMediaSession_H_
