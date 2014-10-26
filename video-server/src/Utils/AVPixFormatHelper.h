/*
 * AVPixFormatHelper.h
 *
 *  Created on: 12. 10. 2014
 *      Author: dron
 */

#ifndef AVPIXFORMATHELPER_H_
#define AVPIXFORMATHELPER_H_

#include <string>
#include "ffmpeg/libavutil/pixfmt.h"

class AVPixFormatHelper {
	public:

		static std::string pixFormatName(AVPixelFormat avPixelFormat);

};

#endif /* AVPIXFORMATHELPER_H_ */
