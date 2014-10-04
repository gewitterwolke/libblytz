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

	// number of newlines in output
	unsigned int nnls = len / ( B64_MAX_LINE_LEN * 0.75);

	if (use_newlines) {
		printfd("newlines in encoded string: %d\n", nnls);
	}

	unsigned int enclen = 4 * ceil(len / 3.0);

	if (use_newlines) {
		enclen += nnls;
	}

	printfd("length of encoded string %s: %d\n", 
			use_newlines ? "(incl. newlines)" : "", enclen);

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

	 unsigned long len = strlen(str);

	 // add a newline at the end (to be compatible with openssl)
	 len++;
	 char *strnl = (char *) calloc(1, len);
	 strcpy(strnl, str);
	 strcat(strnl, "\n");

	 printfd( "encoding \"%s\" (%lu chars), using newlines: %s\n", strnl, 
			 len, use_newlines ? "true" : "false");

	 unsigned int enclen = b64_get_encoded_len(strnl, use_newlines);

	 // +1 for zero-terminator and +1 for newline at the end (added automatically
	 // by openssl)
	 char *buffer = (char *) calloc(1, enclen + 2);

	 // file stream to buffer
	 FILE *streambuf = fmemopen(buffer, enclen + 1, "w");
	 bio = BIO_new_fp(streambuf, BIO_NOCLOSE);

	 // enable base64 encoding
	 b64 = BIO_new(BIO_f_base64());
	 if (!use_newlines) {
	   BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); 
	 }
	 bio = BIO_push(b64, bio);
	 
	 BIO_write(bio, strnl, len);
	 BIO_flush(bio);
	 BIO_free_all(bio);

	 fclose(streambuf);

	 // remove trailing newline
	 buffer[enclen] = '\0';

	 // force zero termination
	 buffer[enclen + 1] = '\0';

	 printfd( "encoded string: \"%s\" (%lu chars)\n", buffer, strlen(buffer));

	 return buffer;
}

// calculates the length of a decoded base64 string
unsigned int get_decoded_len(const char* b64input) { 
	unsigned int len = strlen(b64input);
	int padding = 0;

	int nnls = len / B64_MAX_LINE_LEN;
	printfd( "number of newlines in decoded str: %d\n", nnls);
	 
	if (b64input[len - 1] == '=')
		padding = 1;

	if (b64input[len - 2] == '=')
		padding++;
	 
	return floor( (len - padding) * 0.75) - nnls;
}

char *b64_decode(const char *str, bool use_newlines) {

	 BIO *bio, *b64;

	 printfd( "decoding \"%s\", using newlines: %s\n", str, 
			 use_newlines ? "true" : "false");

	 unsigned int len = strlen(str);

	 // -1 as we ignore the newline at the end
	 unsigned int declen = get_decoded_len(str) - 1;
	 printfd( "calc. length of decoded string: %d\n", declen);

	 // add a newline at the end (to be compatible with openssl)
	 len++;
	 char *strnl = (char *) calloc(1, len);
	 strcpy(strnl, str);
	 strcat(strnl, "\n");

	 // +1 for zero-terminator and +1 for newline (added by openssl)
	 char *buffer = (char*) calloc(1, declen + 2);

	 int readlen = 0;

	 FILE *stream = fmemopen((void *)strnl, len, "r");
	 bio = BIO_new_fp(stream, BIO_NOCLOSE);
	  
	 b64 = BIO_new(BIO_f_base64());
	 if (!use_newlines) {
		 // don't expect newlines every 64 bytes
	   BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); 
	 }

	 bio = BIO_push(b64, bio);

	 readlen = BIO_read(bio, buffer, len);

	 // remove trailing newline 
	 buffer[declen] = '\0';

	 // force zero termination 
	 buffer[declen + 1] = '\0';
	  
	 BIO_free_all(bio);
	 fclose(stream);

	 printfd("decoded string: %s\n", buffer);
	   
	 return buffer;
}

char *b64_decode(const char *str) {
	return b64_decode(str, true);
}
