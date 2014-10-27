/*
 * Conversion.h
 *
 *  Created on: 11. 10. 2014
 *      Author: dron
 */

#ifndef CONVERSION_H_
#define CONVERSION_H_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "PvApi.h"
#include "libavutil/pixfmt.h"
#include <string>

struct _PvImageFormat {
	tPvImageFormat format;
	tPvBayerPattern bayerPattern;

	_PvImageFormat(tPvImageFormat format, tPvBayerPattern bayerPattern) :
		format(format), bayerPattern(bayerPattern) {}
};

AVPixelFormat convert(const _PvImageFormat& src);

struct _PvImageFormat_String {
	std::string format;
	std::string bayerPattern;

	_PvImageFormat_String(const std::string& format, const std::string& bayerPattern) :
		format(format), bayerPattern(bayerPattern) {}
};

AVPixelFormat convert(const _PvImageFormat_String& src);

#endif /* CONVERSION_H_ */
