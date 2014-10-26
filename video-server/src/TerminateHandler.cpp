/*
 * TerminateHandler.cpp
 *
 *  Created on: 29. 6. 2014
 *      Author: dron
 */

#include "TerminateHandler.h"
#include <iostream>

#ifdef _WIN32
	BOOL TerminateHandler::ctrlHandler(DWORD fdwCtrlType) {
		BOOL ret = false;
		switch (fdwCtrlType) {
			case CTRL_C_EVENT:
				ret = true;
				break;
			case CTRL_CLOSE_EVENT:
				ret = true;
				break;

			default:
				ret = false;
		}

		if (ret) {
			std::cout << "Terminate event." << std::endl;
			instance().event();
		}
		return ret;
	}
#else
	#include <signal.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <unistd.h>

	void TerminateHandler::sigHandler(int) {
		instance().event();
	}
#endif


void TerminateHandler::operator+=(Functor t) {
	event += t;
}

TerminateHandler& TerminateHandler::instance() {
	static TerminateHandler instance_;

#ifdef WIN32
	SetConsoleCtrlHandler((PHANDLER_ROUTINE) ctrlHandler, TRUE);
#else
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = TerminateHandler::sigHandler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);

	struct sigaction sigTermHandler;
	sigTermHandler.sa_handler = TerminateHandler::sigHandler;
	sigemptyset(&sigTermHandler.sa_mask);
	sigTermHandler.sa_flags = 0;
	sigaction(SIGTERM, &sigTermHandler, NULL);
#endif

	return instance_;
}

TerminateHandler::TerminateHandler() {
}
