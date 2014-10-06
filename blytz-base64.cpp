// adapted from
// https://raw.githubusercontent.com/exabytes18/OpenSSL-Base64/master/base64.c

// Encodes and decodes base64 strings
//
// We use the openssl library to perform the coding. When encoding, it adds
// a newline at the end of the encoded string, for decoding it expects this
// newline at the end or the decoding won't work.

#include <openssl/bio.h>
#include <openssl/evp.h>

#include <cstring>
#include <math.h>

#include <string>

#include "blytz-base64.h"
#include "blytz-debug.h"

char ERR[] = "ERR";

unsigned int b64_get_encoded_len(unsigned int len, bool use_newlines) {

	// number of newlines in output
	unsigned int nnls = len / ( (B64_MAX_LINE_LEN) * 0.75 + 1);

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

unsigned int b64_get_encoded_len(unsigned int len) {
	return b64_get_encoded_len(len, true);
}

char *b64_encode(const char *str, unsigned int len) {
	return b64_encode(str, len, true, true);
}

char *b64_encode_wo_trailing_nl(const char *str, unsigned int len) {
	return b64_encode(str, len, true, false);
}

char *b64_encode(const char *str, unsigned int len,
		bool use_newlines, bool trailing_newline) {

	 BIO *bio, *b64;

	 if (trailing_newline)
		 printfd("Using trailing newline\n");

	 //unsigned long len = strlen(str);
	 if (strlen(str) > len) {
		 printfe("Provided string is longer than given length (%lu > %du)\n",
				 strlen(str), len);
		 return ERR;
	 }

	 printfd( "Encoding \"%s\" (%d chars), using newlines: %s\n", str, 
			 len, use_newlines ? "true" : "false");

	 // add a newline at the end (to be compatible with openssl)
	 if (trailing_newline)
		 len++;
	 char *strnl = (char *) calloc(1, len + 1);
	 strcpy(strnl, str);
	 if (trailing_newline)
		 strcat(strnl, "\n");

	 unsigned int enclen = b64_get_encoded_len(len, use_newlines);

	 // +1 for zero-terminator and +1 for newline at the end 
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

	int nnls = len / (B64_MAX_LINE_LEN + 1);
	printfd( "newlines in decoded str: %d\n", nnls);
	 
	if (b64input[len - 1] == '=')
		padding = 1;

	if (b64input[len - 2] == '=')
		padding++;
	 
	int declen = floor((len - padding) * 0.75);

	// the newlines in the base64-encoded string are mandatory
	// and unrelated to the newlines in the decoded string
	// so don't count them.
	// -1 because we assume the encoded string was obtained by adding
	// a newline at the end of the original string for openssl compatibility
	return declen - nnls - 1;
}

char *b64_decode_nnl(const char *str, unsigned int *len, bool use_newlines) {
	return b64_decode(str, len, use_newlines, true);
}

// decodes a base64 encoded string
char *b64_decode(const char *str, unsigned int *declen, 
		bool use_newlines, bool no_trailing_nl) {

	 BIO *bio, *b64;

	 printfd( "decoding \"%s\" (%lu chars), using newlines: %s\n", str, 
			 strlen(str), use_newlines ? "true" : "false");

	 unsigned int ldeclen = get_decoded_len(str);
	 if (no_trailing_nl)
		 ldeclen++;
	 printfd( "calc. length of decoded string: %d\n", ldeclen);

	 unsigned int len = strlen(str);

	 // add a newline at the end (openssl compatibility)
	 len++;
	 char *strnl = (char *) calloc(1, len);
	 strcpy(strnl, str);
	 strcat(strnl, "\n");

	 // +1 for zero-terminator and +1 for newline (added by openssl)
	 char *buffer = (char*) calloc(1, ldeclen + 2);

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
	 buffer[ldeclen] = '\0';

	 // force zero termination 
	 buffer[ldeclen + 1] = '\0';
	  
	 BIO_free_all(bio);
	 fclose(stream);

	 printfd("decoded string: %s (length: %d)\n", buffer, ldeclen);

	 *declen = ldeclen;
	   
	 return buffer;
}

char *b64_decode(const char *str, unsigned int *len) {
	return b64_decode(str, len, true, false);
}
