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
		printfd("Newlines in encoded string: %d\n", nnls);
	}

	unsigned int enclen = 4 * ceil(len / 3.0);

	if (use_newlines) {
		enclen += nnls;
	}

	printfd("Length of encoded string %s: %d\n", 
			use_newlines ? "(incl. newlines)" : "", enclen);

	return enclen;
}

unsigned int b64_get_encoded_len(unsigned int len) {
	return b64_get_encoded_len(len, true);
}

char *b64_encode(const char *str, unsigned int len) {
	return b64_encode(str, len, true, true);
}

char *b64_encode_nnl(const char *str, unsigned int len) {
	return b64_encode(str, len, true, false);
}

char *b64_encode(const char *str, unsigned int len, bool use_newlines) {
	return b64_encode(str, len, use_newlines, true);
}

char *b64_encode(const char *str) {
	unsigned int len = strlen(str);
	return b64_encode(str, len, true, true);
}

char *b64_encode(const char *str, unsigned int len,
		bool use_newlines, bool trailing_newline) {

	 BIO *bio, *b64;

	 if (strlen(str) > len) {
		 printfw("String (\"%s\") is longer than given length (%lu > %d)\n", str,
				 strlen(str), len);
	 }

	 printfd( "Encoding \"%s\" (%d chars), newlines: %s, trailing newline: %s\n", 
			 str, len, use_newlines ? "true" : "false", 
			 trailing_newline ? "true" : "false");

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

	 printfd( "Encoded string: \"%s\" (%lu chars)\n", buffer, strlen(buffer));

	 return buffer;
}

// calculates the length of a decoded base64 string
unsigned int b64_get_decoded_len(const char* b64input, bool use_newlines) { 
	unsigned int len = strlen(b64input);
	int padding = 0;

	int nnls = 0;

	if (use_newlines) {
    nnls = len / (B64_MAX_LINE_LEN + 1);
		printfd( "newlines in encoded str: %d\n", nnls);
	}
	 
	if (b64input[len - 1] == '=')
		padding = 1;

	if (b64input[len - 2] == '=')
		padding++;
	 
	// the newlines in the base64-encoded string are mandatory
	// and unrelated to the newlines in the decoded string
	// so don't count them.
	int declen = floor((len - padding - nnls) * 0.75);

	printfd("declen: %d = (%d - %d - %d) * 0.75\n", declen, len, padding, nnls);

	// -1 because we assume the encoded string was obtained by adding
	// a newline at the end of the original string for openssl compatibility
	return declen - 1;
}

unsigned int b64_get_decoded_len(const char* b64input) { 
	return b64_get_decoded_len(b64input, true);
}

char *b64_decode_nnl(const char *str, unsigned int *len) {
	return b64_decode(str, len, false);
}

char *b64_decode(const char *str) {
	unsigned int unused;
	return b64_decode(str, &unused, true);
}

char *b64_decode(const char *str, unsigned int *len) {
	return b64_decode(str, len, true);
}

// decodes a base64 encoded string (str) returning the decoded length (len_out)
char *b64_decode(const char *str, unsigned int *len_out, bool trailing_newline) {

	 BIO *bio, *b64;

	 // check if the input contains newlines
	 // FIXME: define max length if string is not terminated?
	 unsigned int len_in = strlen(str);
	 bool use_newlines = false;
	 if (strstr(str, "\n") != NULL)
		 use_newlines = true;
	 
	 printfd( "Decoding \"%s\" (%lu chars), newlines: %s, trailing newline: %s\n", 
			 str, strlen(str), use_newlines ? "true" : "false", 
			 trailing_newline ? "true" : "false");

	 unsigned int len_dec = b64_get_decoded_len(str, use_newlines);
	 if (!trailing_newline)
		 len_dec++;
	 printfd( "Calculated length of decoded string: %d\n", len_dec);

	 // add a trailing newline (openssl compatibility)
	 len_in++;
	 char *strnl = (char *) calloc(1, len_in);
	 strcpy(strnl, str);
	 strcat(strnl, "\n");

	 // +1 for zero-terminator and +1 for newline (added by openssl)
	 len_dec += 1;
	 char *buffer = (char*) calloc(1, len_dec + 1);

	 unsigned int readlen = 0;

	 FILE *stream = fmemopen((void *)strnl, len_in, "r");
	 bio = BIO_new_fp(stream, BIO_NOCLOSE);
	  
	 b64 = BIO_new(BIO_f_base64());
	 if (!use_newlines) {
		 // don't expect newlines every 64 bytes
	   BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); 
	 }

	 bio = BIO_push(b64, bio);

	 readlen = BIO_read(bio, buffer, len_in);

	 if (readlen != len_dec) {
		 printfw("Read length (%d) does not match expected length (%d)\n", readlen, 
				 len_dec);
	 }

	 // remove trailing newline 
	 buffer[len_dec - 1] = '\0';
	 len_dec -= 1;

	 // force zero termination 
	 buffer[len_dec + 1] = '\0';
	  
	 BIO_free_all(bio);
	 fclose(stream);

	 printfd("decoded string: %s (length: %d)\n", buffer, len_dec);

	 *len_out = len_dec;
	   
	 return buffer;
}
