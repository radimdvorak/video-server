/*
 * AVPixFormatHelper.cpp
 *
 *  Created on: 12. 10. 2014
 *      Author: dron
 */

#include "AVPixFormatHelper.h"
#include <map>

std::string AVPixFormatHelper::pixFormatName(AVPixelFormat avPixelFormat) {
	static std::map<AVPixelFormat, std::string> pixFormatNameMap;
	if (pixFormatNameMap.empty()) {
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_YUV420P, "YUV420P"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_YUYV422, "YUYV422"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_RGB24, "RGB24"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_BGR24, "BGR24"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_YUV422P, "YUV422P"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_YUV444P, "YUV444P"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_YUV410P, "YUV410P"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_YUV411P, "YUV411P"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_GRAY8, "GRAY8"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_MONOWHITE, "MONOWHITE"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_MONOBLACK, "MONOBLACK"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_PAL8, "PAL8"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_YUVJ420P, "YUVJ420p"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_YUVJ422P, "YUVJ422p"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_YUVJ444P, "YUVJ444p"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_XVMC_MPEG2_MC, "XVMC_MPEG2_MC"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_XVMC_MPEG2_IDCT, "XVMC_MPEG2_IDCT"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_UYVY422, "UYVY422"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_UYYVYY411, "UYYVYY411"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_BGR8, "BGR8"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_BGR4, "BGR4"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_BGR4_BYTE, "BGR4_BYTE"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_RGB8, "RGB8"));

		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_RGB4, "RGB4"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_RGB4_BYTE, "RGB4_BYTE"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_NV12, "NV12"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_NV21, "NV21"));

		// TODO

		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_BAYER_BGGR8, "BAYER_BGGR8"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_BAYER_RGGB8, "BAYER_RGGB8"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_BAYER_GBRG8, "BAYER_GBRG8"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_BAYER_GRBG8, "BAYER_GRBG8"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_BAYER_BGGR16LE, "BAYER_BGGR16LE"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_BAYER_BGGR16BE, "BAYER_BGGR16BE"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_BAYER_RGGB16LE, "BAYER_RGGB16LE"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_BAYER_RGGB16BE, "BAYER_RGGB16BE"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_BAYER_GBRG16LE, "BAYER_GBRG16LE"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_BAYER_GBRG16BE, "BAYER_GBRG16BE"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_BAYER_GRBG16LE, "BAYER_GRBG16LE"));
		pixFormatNameMap.insert(std::make_pair(AV_PIX_FMT_BAYER_GRBG16BE, "BAYER_GRBG16BE"));
	}

	std::map<AVPixelFormat, std::string>::iterator found = pixFormatNameMap.find(avPixelFormat);
	if (found != pixFormatNameMap.end()) {
		return found->second;
	}

	return "Unknown";
}

