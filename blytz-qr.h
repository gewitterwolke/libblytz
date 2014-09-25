#ifndef _BLYTZ_QR_H_
#define _BLYTZ_QR_H_

//#include <string>
#include <qrencode.h>

namespace blytz {

	// ascii codes for qr code
	static const char white[] = "\033[40;37;1m";
	static const char reset[] = "\033[0m";
	static const char full_white[] = "\342\226\210";
	static const char upper_white[] = "\342\226\200";
	static const char lower_white[] = "\342\226\204";

	// png properties for png qr code
	static unsigned char png_white[4] = {0, 0, 0, 255};
	static unsigned char png_black[4] = {255, 255, 255, 255};
	static unsigned int dpi = 72;		
	static unsigned int size = 6;

#define INCHES_PER_METER (100.0/2.54)

	// global qr code margin size
	static const unsigned int margin = 4;

	const char *get_qrcode_ascii(const char *);

	struct mem_buffer {
		char *buffer;
		size_t size;
	};
	
	std::vector<unsigned char> get_qrcode_png(const char *);
	std::vector<unsigned char> writePNG(const QRcode *qrcode);

} // namespace blytz

#endif
