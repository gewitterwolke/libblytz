#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>

#include <memory.h>

#include <qrencode.h>
#include <png.h>

#include "blytz-qr.h"
#include "blytz-api.h"

// this file contains a lot of code from qrenc.c in libqrencode licensed under
// the GPL -->
// TODO: include GPL 

namespace blytz {

	void qr_margin(unsigned int realwidth, int use_ansi, std::stringstream &buf) {
		unsigned int x, y;

		for (y = 0; y < margin/2; y++) {
			buf << white;

			for (x = 0; x < realwidth; x++)  {
				buf << full_white;
			}

			buf << ( reset);
			buf << ("\n");
		}
	}

	std::string qr_to_str(QRcode *qrcode, int use_ansi) {
		unsigned int x, y;
		unsigned int realwidth;

		if (use_ansi){
			;
		} else {
			//	memset(white, 0, strlen(white));
			//	memset(reset, 0, strlen(reset));
			//	memset(full_white, 0, strlen(full_white));
			//	memset(upper_white, 0, strlen(upper_white));
			//	memset(lower_white, 0, strlen(lower_white));
		}

		realwidth = (qrcode->width + margin * 2);

		std::stringstream buf;

		// top margin
		qr_margin(realwidth, use_ansi, buf);

		// data
		for(y = 0; y < (unsigned int)qrcode->width; y += 2) {

			unsigned char *row1, *row2;
			row1 = qrcode->data + y * qrcode->width;
			row2 = row1 + qrcode->width;

			buf << (white);

			for (x = 0; x < margin; x++) {
				buf << (full_white);
			}

			for (x = 0; x < (unsigned int) qrcode->width; x++) {
				if (row1[x] & 1) {
					if (y < (unsigned int)qrcode->width - 1 && row2[x] & 1) {
						buf << (" ");
					} else {
						buf << (lower_white);
					}
				} else {
					if (y < (unsigned int) qrcode->width - 1 && row2[x] & 1) {
						buf << (upper_white);
					} else {
						buf << (full_white);
					}
				}
			}

			for (x = 0; x < margin; x++) {
				buf << (full_white);
			}

			buf << (reset);
			buf << std::endl;
		}

		// bottom margin
		qr_margin(realwidth, use_ansi, buf);

		// print
		std::string str = buf.str();
		return str;	
	}

	// provide a C compatible interface, hence const char instead of string
	const char *get_qrcode_ascii(const char *str) {

		char *str2 = (char *)malloc(strlen(str) + MAX_ENC_PWD_LEN);
		strcpy(str2, str);
		if (has_encryption_pwd()) {
			str2 = strcat(str2, "|");
			str2 = strcat(str2, get_encryption_pwd());
		}

		QRecLevel level = QR_ECLEVEL_M;
		QRcode *qr = QRcode_encodeString8bit(str2, 0, level);

		std::string qr_str = qr_to_str(qr, false);				

		// user has to delete this
		char *qr_ch = (char *) malloc(qr_str.size());
		memcpy(qr_ch, qr_str.c_str(), qr_str.size());

		return qr_ch;
	}

	std::vector<unsigned char> get_qrcode_png(const char *str) {

		FILE *f = fopen("/tmp/debugapi.txt", "a");
		fprintf( f, "BLYTZ-API - creating QR code from string %s\n", str);

		char *str2 = (char *)malloc(strlen(str) + MAX_ENC_PWD_LEN);
		strcpy(str2, str);
		if (has_encryption_pwd()) {
			str2 = strcat(str2, "|");
			str2 = strcat(str2, get_encryption_pwd());
		}

		QRecLevel level = QR_ECLEVEL_M;
		QRcode *qr = QRcode_encodeString8bit(str2, 0, level);

		std::vector<unsigned char> buf = writePNG(qr);

		FILE *fqr = fopen("/tmp/qrtmp.png", "w");
		for (unsigned int i = 0; i < buf.size(); i++) {
			fputc( buf[i], fqr);
		}
		fclose(fqr);

		fprintf( f, "BLYTZ-API - QR code created\n");
		fclose(f);
		return buf;
	}

	static std::vector<unsigned char> png_buf;

	void png_write_data(png_structp png_ptr, png_bytep data, 
			png_size_t length) {

		for (unsigned int i = 0; i < length; i++) {
			png_buf.push_back(data[i]);
		}
	}

	void png_flush(png_structp png_ptr) {
	}
	
	std::vector<unsigned char> writePNG(const QRcode *qrcode) {

		png_structp png_ptr;
		png_infop info_ptr;
		png_colorp palette = NULL;
		png_byte alpha_values[2];
		unsigned char *row, *p, *q;
		unsigned int x, y, xx, yy;
		int bit;
		unsigned int realwidth;

		realwidth = (qrcode->width + margin * 2) * size;

		unsigned int rowlen = (realwidth + 7) / 8;
		row = (unsigned char *)malloc(rowlen);

		if(row == NULL) {
			fprintf(stderr, "Failed to allocate memory.\n");
			exit(EXIT_FAILURE);
		}

		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if(png_ptr == NULL) {
			fprintf(stderr, "Failed to initialize PNG writer.\n");
			exit(EXIT_FAILURE);
		}

		info_ptr = png_create_info_struct(png_ptr);
		if(info_ptr == NULL) {
			fprintf(stderr, "Failed to initialize PNG write.\n");
			exit(EXIT_FAILURE);
		}

		if(setjmp(png_jmpbuf(png_ptr))) {
			png_destroy_write_struct(&png_ptr, &info_ptr);
			fprintf(stderr, "Failed to write PNG image.\n");
			exit(EXIT_FAILURE);
		}

		palette = (png_colorp) malloc(sizeof(png_color) * 2);
		if(palette == NULL) {
			fprintf(stderr, "Failed to allocate memory.\n");
			exit(EXIT_FAILURE);
		}

		palette[0].red = png_white[0];
		palette[0].green = png_white[1];
		palette[0].blue = png_white[2];

		palette[1].red = png_black[0];
		palette[1].green = png_black[1];
		palette[1].blue = png_black[2];

		alpha_values[0] = png_white[3];
		alpha_values[1] = png_black[3];

		png_set_PLTE(png_ptr, info_ptr, palette, 2);
		png_set_tRNS(png_ptr, info_ptr, alpha_values, 2, NULL);

		png_buf.clear();

		png_set_write_fn(png_ptr, NULL, png_write_data, png_flush);
		
		png_set_IHDR(png_ptr, info_ptr,
				realwidth, realwidth,
				1,
				PNG_COLOR_TYPE_PALETTE,
				PNG_INTERLACE_NONE,
				PNG_COMPRESSION_TYPE_DEFAULT,
				PNG_FILTER_TYPE_DEFAULT);

		png_set_pHYs(png_ptr, info_ptr, dpi * INCHES_PER_METER,
				dpi * INCHES_PER_METER, PNG_RESOLUTION_METER);
		png_write_info(png_ptr, info_ptr);

		// top margin
		memset(row, 0xff, rowlen);
		for( y = 0; y < margin * size; y++) {
			png_write_row(png_ptr, row);
		}

		// data
		p = qrcode->data;
		for (y = 0; y < (unsigned int) qrcode->width; y++) {
			memset(row, 0xff, rowlen);
			q = row;
			q += margin * size / 8;
			bit = 7 - (margin * size % 8);

			for (x = 0; x < (unsigned int) qrcode->width; x++) {
				for (xx = 0; xx < size; xx++) {
					*q ^= (*p & 1) << bit;
					bit--;

					if (bit < 0) {
						q++;
						bit = 7;
					}
				}
				p++;
			}
			for (yy = 0; yy < size; yy++) {
				png_write_row(png_ptr, row);
			}
		}

		// bottom margin
		memset(row, 0xff, rowlen);
		for (y = 0; y < margin * size; y++) {
			png_write_row(png_ptr, row);
		}

		png_write_end(png_ptr, info_ptr);
		png_destroy_write_struct(&png_ptr, &info_ptr);

		free(row);
		free(palette);
		return png_buf;
	}

} // namespace blytz
