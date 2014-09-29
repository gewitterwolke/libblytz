// adapted from
// https://raw.githubusercontent.com/exabytes18/OpenSSL-Base64/master/base64.c

#include <openssl/bio.h>
#include <openssl/evp.h>

#include <cstring>
#include <math.h>

#include <string>

#include "blytz-base64.h"

char *b64_encode(const char *str) {
	 BIO *bio, *b64;

	 unsigned int len = strlen(str);

	 // size of base64 buffer
	 int enclen = 4 * ceil((double) len / 3);
	 char *buffer = (char *)malloc(enclen + 1);
	 // file stream to buffer
	 FILE *streambuf = fmemopen(buffer, enclen + 1, "w");

	 b64 = BIO_new(BIO_f_base64());
	 bio = BIO_new_fp(streambuf, BIO_NOCLOSE);
	 bio = BIO_push(b64, bio);

	 //Ignore newlines - write everything in one line
	 BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); 

	 BIO_write(bio, str, len);
	 BIO_flush(bio);
	 BIO_free_all(bio);

	 fclose(streambuf);

	 return buffer;
}

// calculates the length of a decoded base64 string
int get_decoded_len(const char* b64input) { 
	unsigned int len = strlen(b64input);
	int padding = 0;
	 
	// last two chars are =
	if (b64input[len-1] == '=' && b64input[len-2] == '=') 
		padding = 2;

	// last char is =
	else if (b64input[len-1] == '=') 
		padding = 1;
	 
	return (int)len * 0.75 - padding;
}

#define B64_MAX_LINE_LEN 63

char *b64_decode_wo_newlines(const char *str) {

	unsigned int strl = strlen(str);
	unsigned int strl_n = strl / B64_MAX_LINE_LEN;

	char *nstr = (char *) malloc( strl + strl_n);

	if (strl > B64_MAX_LINE_LEN && str[B64_MAX_LINE_LEN] != '\n') {

		for (unsigned int i = 0, j = 0; i < strl; i += B64_MAX_LINE_LEN) {

			unsigned int r = strl - i;

			if (r > B64_MAX_LINE_LEN) {
				memcpy( nstr + j, str + i, B64_MAX_LINE_LEN);
				j += B64_MAX_LINE_LEN;
				nstr[++j] = '\n';
			} else {
				memcpy( nstr + j, str + i, r);
			}
		}
		return b64_decode(nstr);
	}

	return b64_decode(str);
}

char *b64_decode(const char *str) {

	 BIO *bio, *b64;
	 unsigned int declen = get_decoded_len(str);
	 unsigned int readlen = 0;
	 unsigned int len = strlen(str);

	 char *buffer = (char*)malloc(declen + 1);

	 FILE *stream = fmemopen((void *)str, len, "r");
	  
	 b64 = BIO_new(BIO_f_base64());
	 bio = BIO_new_fp(stream, BIO_NOCLOSE);
	 bio = BIO_push(b64, bio);

	 // do not use newlines to flush buffer
	 BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); 
	 readlen = BIO_read(bio, buffer, len);

	 // can test here if len == declen - if not, then return an error
	 buffer[readlen] = '\0';
	  
	 BIO_free_all(bio);
	 fclose(stream);
	   
	 return buffer;
}
