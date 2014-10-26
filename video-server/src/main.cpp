/*
 * main.cpp
 *
 *  Created on: 29. 6. 2014
 *      Author: dron
 */

#include "Application.h"
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[]) {
	try {
		Application::Setup setup = Application::Setup::load("VideoServer.ini");

		Application app(setup);

		std::cout << "Play the stream using url: " << app.getUrl() << std::endl;

		app.loop();
	} catch (std::exception& e) {
		std::cerr << "Program ended with exception: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "Program exited normally." << std::endl;
	return EXIT_SUCCESS;
}

