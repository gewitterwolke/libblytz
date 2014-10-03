// adapted from
// https://raw.githubusercontent.com/exabytes18/OpenSSL-Base64/master/base64.c

#include <openssl/bio.h>
#include <openssl/evp.h>

#include <cstring>
#include <math.h>

#include <string>

#include "blytz-base64.h"
#include "blytz-debug.h"

unsigned int b64_get_encoded_len(const char *in, bool use_newlines) {

	int len = strlen(in);

	printfd("length of input string: %d\n", len);

	// number of newlines in output
	unsigned int nnls = len / ( B64_MAX_LINE_LEN * 0.75);

	if (use_newlines) {
		printfd("newlines in encoded string: %d\n", nnls);
	}

	// size of base64 buffer (including newlines every 64 chars)
	unsigned int enclen = 4 * ceil( len / 3.0);

	printfd("length of plain encoded string: %d\n", enclen);

	if (use_newlines) {
		// +1 since openssl always adds a newline at the end
		enclen += nnls + 1;
	}

	return enclen;
}

unsigned int b64_get_encoded_len(const char *in) {
	return b64_get_encoded_len(in, true);
}

char *b64_encode(const char *str) {
	return b64_encode(str, true);
}

char *b64_encode(const char *str, bool use_newlines) {
	 BIO *bio, *b64;

	 unsigned int enclen = b64_get_encoded_len(str, use_newlines);

	 // +1 for zero-terminator
	 char *buffer = (char *) calloc(0, enclen + 5);

	 // file stream to buffer
	 FILE *streambuf = fmemopen(buffer, enclen, "w");

	 b64 = BIO_new(BIO_f_base64());
	 bio = BIO_new_fp(streambuf, BIO_NOCLOSE);
	 bio = BIO_push(b64, bio);

	 if (!use_newlines) {
	   BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); 
	 }
	 
	 unsigned int len = strlen(str);

	 BIO_write(bio, str, len);
	 BIO_flush(bio);
	 BIO_free_all(bio);

	 printfd( "%s\n", buffer);

	 fclose(streambuf);

	 /*
	 if (buffer[enclen] == '\n') {
		 buffer[enclen] = '\0';
	 }
	 */

	 return buffer;
}

// calculates the length of a decoded base64 string
unsigned int get_decoded_len(const char* b64input) { 
	unsigned int len = strlen(b64input);
	int padding = 0;
	 
	// last two chars are ==
	if (b64input[len-1] == '=' && b64input[len-2] == '=') 
		padding = 2;

	// last char is =
	else if (b64input[len-1] == '=') 
		padding = 1;
	 
	return len * 0.75 - padding;
}

char *b64_decode(const char *str, bool use_newlines) {

	 BIO *bio, *b64;
	 unsigned int declen = get_decoded_len(str);
	 unsigned int readlen = 0;
	 unsigned int len = strlen(str);

	 char *buffer = (char*)calloc(0, declen + 1);

	 FILE *stream = fmemopen((void *)str, len, "r");
	  
	 b64 = BIO_new(BIO_f_base64());
	 bio = BIO_new_fp(stream, BIO_NOCLOSE);
	 bio = BIO_push(b64, bio);

	 if (!use_newlines) {
		 // don't expect newlines every 64 bytes
	   BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); 
	 }
	 readlen = BIO_read(bio, buffer, len);

	 // can test here if len == declen - if not, then return an error
	 buffer[readlen] = '\0';
	  
	 BIO_free_all(bio);
	 fclose(stream);
	   
	 return buffer;
}

char *b64_decode(const char *str) {
	return b64_decode(str, true);
}
