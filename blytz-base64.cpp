// adapted from
// https://raw.githubusercontent.com/exabytes18/OpenSSL-Base64/master/base64.c

#include <openssl/bio.h>
#include <openssl/evp.h>

#include <cstring>
#include <math.h>

#include <string>

#include "blytz-base64.h"

unsigned int get_encoded_len(const char *in) {

	int len = strlen(in);

	printf("len: %d\n", len);

	// number of newlines in output
	unsigned int nnls = len / 48;

	printf("nnls: %d\n", nnls);

	// size of base64 buffer
	unsigned int enclen = 4 * ceil((double) len / 3) + nnls;

	return enclen;
}

char *b64_encode(const char *str) {
	 BIO *bio, *b64;

	 unsigned int enclen = get_encoded_len(str);
	 printf( "enclen: %d\n", enclen);

	 char *buffer = (char *)malloc(enclen + 1);
	 memset( buffer, 0, enclen + 1);

	 // file stream to buffer
	 FILE *streambuf = fmemopen(buffer, enclen + 1, "w");

	 b64 = BIO_new(BIO_f_base64());
	 bio = BIO_new_fp(streambuf, BIO_NOCLOSE);
	 bio = BIO_push(b64, bio);

	 // Ignore newlines - write everything in one line
	 //BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); 
	 
	 unsigned int len = strlen(str);
	 //printf("len: %d\n", len);

	 BIO_write(bio, str, len);
	 BIO_flush(bio);
	 BIO_free_all(bio);

	 fclose(streambuf);

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

char *b64_decode_helper(const char *str, bool use_newlines) {

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
	 if (!use_newlines) {
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
	return b64_decode_helper(str, true);
}

char *b64_decode_wo_newlines(const char *str) {
	return b64_decode_helper(str, false);
}
