/*
 * Conversion.cpp
 *
 *  Created on: 11. 10. 2014
 *      Author: dron
 */

#include "Conversion.h"

AVPixelFormat convert(const _PvImageFormat& src) {
	switch (src.format) {
		case ePvFmtMono8: return AV_PIX_FMT_GRAY8;
		case ePvFmtMono16: return AV_PIX_FMT_GRAY16LE;
		case ePvFmtBayer8: {
			if (src.bayerPattern == ePvBayerBGGR) {
				return AV_PIX_FMT_BAYER_BGGR8;
			}
			if (src.bayerPattern == ePvBayerGBRG) {
				return AV_PIX_FMT_BAYER_GBRG8;
			}
			if (src.bayerPattern == ePvBayerGRBG) {
				return AV_PIX_FMT_BAYER_GRBG8;
			}
			if (src.bayerPattern == ePvBayerRGGB) {
				return AV_PIX_FMT_BAYER_RGGB8;
			}
			break;
		}
		case ePvFmtBayer16: {
			if (src.bayerPattern == ePvBayerBGGR) {
				return AV_PIX_FMT_BAYER_BGGR16LE;
			}
			if (src.bayerPattern == ePvBayerGBRG) {
				return AV_PIX_FMT_BAYER_GBRG16LE;
			}
			if (src.bayerPattern == ePvBayerGRBG) {
				return AV_PIX_FMT_BAYER_GRBG16LE;
			}
			if (src.bayerPattern == ePvBayerRGGB) {
				return AV_PIX_FMT_BAYER_RGGB16LE;
			}
			break;
		}
		case ePvFmtRgb24: return AV_PIX_FMT_RGB24;
		case ePvFmtRgb48: return AV_PIX_FMT_RGB48LE;
		case ePvFmtYuv411: return AV_PIX_FMT_YUV411P;
		case ePvFmtYuv422: return AV_PIX_FMT_YUV422P;
		case ePvFmtYuv444: return AV_PIX_FMT_YUV444P;
		case ePvFmtBgr24: return AV_PIX_FMT_BGR24;
		case ePvFmtRgba32: return AV_PIX_FMT_RGBA;
		case ePvFmtBgra32: return AV_PIX_FMT_BGRA;
		case ePvFmtMono12Packed: return AV_PIX_FMT_NONE;
		case ePvFmtBayer12Packed: return AV_PIX_FMT_NONE;
		default: return AV_PIX_FMT_NONE;
	}

	return AV_PIX_FMT_NONE;
}

AVPixelFormat convert(const _PvImageFormat_String& src) {
	_PvImageFormat _src(__ePvFmt_force_32, __ePvBayer_force_32);

	if (src.format == "Mono8") {
		_src.format = ePvFmtMono8;
	}
	if (src.format == "Mono16") {
		_src.format = ePvFmtMono16;
	}
	if (src.format == "Bayer8") {
		_src.format = ePvFmtBayer8;
	}
	if (src.format == "Bayer16") {
		_src.format = ePvFmtBayer16;
	}
	if (src.format == "Rgb24") {
		_src.format = ePvFmtRgb24;
	}
	if (src.format == "Rgb48") {
		_src.format = ePvFmtRgb48;
	}
	if (src.format == "Yuv411") {
		_src.format = ePvFmtYuv411;
	}
	if (src.format == "Yuv422") {
		_src.format = ePvFmtYuv422;
	}
	if (src.format == "Yuv444") {
		_src.format = ePvFmtYuv444;
	}
	if (src.format == "Bgr24") {
		_src.format = ePvFmtBgr24;
	}
	if (src.format == "Rgba32") {
		_src.format = ePvFmtRgba32;
	}
	if (src.format == "Bgra32") {
		_src.format = ePvFmtBgra32;
	}

	if (src.bayerPattern == "BGGR") {
		_src.bayerPattern = ePvBayerBGGR;
	}
	if (src.bayerPattern == "GBRG") {
		_src.bayerPattern = ePvBayerGBRG;
	}
	if (src.bayerPattern == "GRBG") {
		_src.bayerPattern = ePvBayerGRBG;
	}
	if (src.bayerPattern == "RGGB") {
		_src.bayerPattern = ePvBayerRGGB;
	}

	return convert(_src);
}
