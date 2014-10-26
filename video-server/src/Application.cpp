/*
 * Application.cpp
 *
 *  Created on: 27. 7. 2014
 *      Author: dron
 */

#include "Application.h"
#include "TerminateHandler.h"
#include <Poco/Util/IniFileConfiguration.h>
#include <Poco/File.h>
#include <fstream>

volatile char Application::terminate_ = 0;

void Application::term() {
	terminate_ = 1;
}

Application::Setup::Setup() :
	port(8554), streamName("cam"), cameraSource(LiveSourceWithx264::Source_USB),
	outSize(640, 480) {
}

bool getVal(Poco::AutoPtr<Poco::Util::IniFileConfiguration> conf, const std::string& key, int& val) {
	bool has = conf->has(key);
	if (has) {
		try {
			val = conf->getInt(key);
		} catch (...) {
			has = false;
		}
	}
	return has;
}

bool getVal(Poco::AutoPtr<Poco::Util::IniFileConfiguration> conf, const std::string& key, std::size_t& val) {
	int _val;
	if (not getVal(conf, key, _val)) {
		return false;
	}
	if (_val < 0) {
		return false;
	}

	val = _val;
	return true;
}

bool getVal(Poco::AutoPtr<Poco::Util::IniFileConfiguration> conf, const std::string& key, std::string& val) {
	bool has = conf->has(key);
	if (has) {
		try {
			val = conf->getString(key);
		} catch (...) {
			has = false;
		}
	}
	return has;
}

typedef std::vector<std::string> KeyList;

template<typename Val>
void getVal(
		Poco::AutoPtr<Poco::Util::IniFileConfiguration> conf, const std::string& key,
		Val& val, KeyList& missing)
{
	if (not getVal(conf, key, val)) {
		missing.push_back(key);
	}
}

Application::Setup Application::Setup::load(const std::string& iniFile) {
	if (not Poco::File(iniFile).exists()) {
		save(iniFile, Setup());
	}

	using Poco::AutoPtr;
	using Poco::Util::IniFileConfiguration;

	Setup setup;

	AutoPtr<IniFileConfiguration> conf(new IniFileConfiguration(iniFile));

	KeyList missingKeys;
	getVal(conf, "EndPoint.Port", setup.port, missingKeys);
	getVal(conf, "EndPoint.Suffix", setup.streamName, missingKeys);
	getVal(conf, "Camera.Source", setup.cameraSource, missingKeys);
	getVal(conf, "Output.FrameWidth", setup.outSize.width, missingKeys);
	getVal(conf, "Output.FrameHeight", setup.outSize.height, missingKeys);

	if (not missingKeys.empty()) {
		std::cout << "Configuration - missing keys (saving default values for them): " << std::endl;
		for (KeyList::iterator i = missingKeys.begin(); i != missingKeys.end(); ++i) {
			std::cout << "  " << *i << std::endl;
		}
		save(iniFile, setup);
	}

	return setup;
}

void Application::save(const std::string& iniFile, const Setup& setup) {
	std::ofstream ofs(iniFile);

	ofs << "[Endpoint]" << "\n";
	ofs << "Port = " << setup.port << "\n";
	ofs << "Suffix = " << setup.streamName << "\n";

	ofs << "[Camera]" << "\n";
	ofs << "; " << LiveSourceWithx264::Source_USB << ", " << LiveSourceWithx264::Source_PvAPI << "\n";
	ofs << "Source = " << setup.cameraSource << "\n";

	ofs << "[Output]" << "\n";
	ofs << "FrameWidth = " << setup.outSize.width << "\n";
	ofs << "FrameHeight = " << setup.outSize.height << "\n";

	ofs.flush();
	ofs.close();
}

Application::Application(const Setup& setup) :
		setup(setup), taskScheduler(BasicTaskScheduler::createNew()), usageEnvironment(0), rtspServer(0), sms(0), liveSubSession(0) {
	usageEnvironment = BasicUsageEnvironment::createNew(*taskScheduler);
	rtspServer = RTSPServer::createNew(*usageEnvironment, setup.port, NULL);

	if (rtspServer == NULL) {
		std::stringstream sstr;
		sstr << "Failed to create rtsp server: " << usageEnvironment->getResultMsg() << "\n";
		throw std::runtime_error(sstr.str());
	}

	sms = ServerMediaSession::createNew(*usageEnvironment, setup.streamName.c_str(), setup.streamName.c_str(), "Live H264 Stream");
	liveSubSession = H264LiveServerMediaSession::createNew(
			H264LiveServerMediaSession::Setup(
					*usageEnvironment,
					true,
					setup.cameraSource,
					setup.outSize
	));
	sms->addSubsession(liveSubSession);
	rtspServer->addServerMediaSession(sms);

	TerminateHandler::instance() += term;
}

Application::~Application() {
	rtspServer->closeAllClientSessionsForServerMediaSession(setup.streamName.c_str());
	rtspServer->deleteServerMediaSession(setup.streamName.c_str());
	RTSPServer::close(rtspServer);
	usageEnvironment->reclaim();
}

std::string Application::getUrl() const {
	char* url = 0;
	try {
		url = rtspServer->rtspURL(sms);
		std::string surl(url);
		delete[] url;
		return surl;
	} catch (...) {
		delete[] url;
		throw;
	}

	return std::string();
}

void Application::loop() {
	taskScheduler->doEventLoop((char*) &terminate_);

}



