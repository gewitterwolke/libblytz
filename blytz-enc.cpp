#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

#include <string.h>

#include "blytz-enc.h"
#include "blytz-base64.h"
#include "blytz-debug.h"

namespace blytz {

	//
	// Create an 256 bit key and IV using the supplied key_data. salt can be 
	// added for taste.
	// Fills in the encryption and decryption ctx objects and returns 0 on success
	//
	int aes_init(unsigned char *key_data, int key_data_len, unsigned char *salt, 
			EVP_CIPHER_CTX *e_ctx, EVP_CIPHER_CTX *d_ctx) {

		// nrounds must be 0 or 1 to maintain compatibilty with openssl (command-line
		// tool)
		int i, nrounds = 0;
		unsigned char key[32], iv[32];

		///
		// Gen key & IV for AES 256 CBC mode. A SHA1 digest is used to hash the 
		// supplied key material.
		// nrounds is the number of times the we hash the material. 
		// More rounds are more secure but slower.
		///

		//i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), salt, 
		//		key_data, key_data_len, nrounds, key, iv);
		i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_md5(), salt, 
				key_data, key_data_len, nrounds, key, iv);

		if (i != 32) {
			printfd("Key size is %d bits - should be 256 bits\n", i);
			return -1;
		}

		EVP_CIPHER_CTX_init(e_ctx);
		EVP_EncryptInit_ex(e_ctx, EVP_aes_256_cbc(), NULL, key, iv);
		EVP_CIPHER_CTX_init(d_ctx);
		EVP_DecryptInit_ex(d_ctx, EVP_aes_256_cbc(), NULL, key, iv);

		return 0;
	}

	///
	// Encrypt *len bytes of data
	// All data going in & out is considered binary (unsigned char[])
	///
	unsigned char *aes_encrypt(EVP_CIPHER_CTX *e, unsigned char *plaintext, 
			int *len) {

		// max ciphertext len for a n bytes of plaintext is n + AES_BLOCK_SIZE -1 bytes
		int c_len = *len + AES_BLOCK_SIZE, f_len = 0;
		unsigned char *ciphertext = (unsigned char *)malloc(c_len);

		// allows reusing of 'e' for multiple encryption cycles
		EVP_EncryptInit_ex(e, NULL, NULL, NULL, NULL);

		// update ciphertext, c_len is filled with the length of ciphertext generated,
		// len is the size of plaintext in bytes
		EVP_EncryptUpdate(e, ciphertext, &c_len, plaintext, *len);

		// update ciphertext with the final remaining bytes
		EVP_EncryptFinal_ex(e, ciphertext+c_len, &f_len);

		*len = c_len + f_len;
		return ciphertext;
	}

	///
	// Decrypt *len bytes of ciphertext
	///
	unsigned char *aes_decrypt(EVP_CIPHER_CTX *e, unsigned char *ciphertext, 
			int *len) {

		// because we have padding ON, we must allocate an extra cipher block size 
		// of memory
		int p_len = *len, f_len = 0;
		unsigned char *plaintext = (unsigned char *)malloc(p_len + AES_BLOCK_SIZE);

		EVP_DecryptInit_ex(e, NULL, NULL, NULL, NULL);
		EVP_DecryptUpdate(e, plaintext, &p_len, ciphertext, *len);
		EVP_DecryptFinal_ex(e, plaintext+p_len, &f_len);

		*len = p_len + f_len;
		memset(plaintext + p_len + f_len, 0, AES_BLOCK_SIZE - f_len);
		return plaintext;
	}

	// encrypt message str using password pwd using AES and Base64 and password
	// pwd
	// (replaces newlines with '!' after Base64 encryption
	const char *encrypt(const char *str, const char *pwd) {

		unsigned char *salt = (unsigned char *)malloc(SALT_LEN);
		RAND_bytes(salt, SALT_LEN);

		unsigned int pwdlen = strlen(pwd);

		EVP_CIPHER_CTX en, de;

		if (aes_init((unsigned char *)pwd, pwdlen, salt, &en, &de)) {
			printfd("Couldn't initialize AES cipher\n");
			return INVALID_PASSWORD;
		}

		int len = strlen(str);
		unsigned char *dat = aes_encrypt(&en, (unsigned char *)str, &len);

		char *keystr = get_keystr((const char *)dat, (const char *)salt);
		char *enc = b64_encode(keystr, false);

		/*
		// replace newlines
		for (unsigned int i = 0; i < strlen(enc); i++) {
			if (enc[i] == '\n') {
				enc[i] = '!';
			}
		}
		*/

		free(keystr);
		free(salt);
		free(dat);

		return enc;
	}

	// decrypt a AES and Base64 encoded message str using password pwd
	// (replaces '!' with '\n' before decryption assuming they have been added by
	// encrypt() or BLYTZ App before)
	// 
	const char *decrypt(const char *str, const char *pwd) {

		char *str2 = (char *) calloc(1, strlen(str));
		//strcpy(str2, str);

		// strip leading and trailing quotation marks
		/*
		unsigned int j = 0;
		for (unsigned int i = 0; i < strlen(str); i++) {

			char c = str[i];

			if (c == '!') {
				str2[j++] = '\n';
			} else if (c == '\"') {
				
			} else {
				str2[j++] = c;
			}
		}
		*/

		bool has_newlines = false;
		if (strstr(str, "\n")) {
			has_newlines = true;
		}

		strncpy(str2, str + 1, strlen(str) - 2);

		// debug output
		//FILE *f = fopen("/tmp/debugenc.txt", "a");
		printfd( "Incoming string for decryption (after replacing '!')\n");
		printfd( "%s\n", str2);

		char *dec = b64_decode(str2, has_newlines);
		char *salt = get_salt(dec);

		printfd( "base64 decoded: %s\n", dec);

		unsigned int pwdlen = strlen(pwd);

		EVP_CIPHER_CTX en, de;

		// initialize AES using salt from incoming string
		if (aes_init((unsigned char *)pwd, pwdlen, (unsigned char *)salt, 
					&en, &de)) {
			printfd("Couldn't initialize AES cipher\n");
			return INVALID_PASSWORD;
		}

		char *dat = get_dat(dec);
		int len = strlen(dat) - strlen(salt);
		len = strlen(dat);

		// actual AES decryption
		unsigned char *plain = aes_decrypt(&de, (unsigned char *)dat, &len);

		free(salt);
		free(dec);
		free(dat);
		free(str2);

		return (char *)plain;
	}

	/*
		 void enctest() {
		 FILE *f = fopen("/tmp/enctest.txt", "w");
		 EVP_CIPHER_CTX en, de;
//int salt[] = {1111, 1111};
unsigned int salt[] = {0x01010101, 0x01010101};
unsigned char *key_data;
int key_data_len, i;
key_data = (unsigned char *)"test123";
key_data_len = strlen((const char*)key_data);

if (aes_init(key_data, key_data_len, (unsigned char *)&salt, &en, &de)) {
printf("Couldn't initialize AES cipher\n");
return;
}

unsigned char *ciphertext;
const char *dat = "testhehe2";
int len = strlen(dat);
ciphertext = aes_encrypt(&en, (unsigned char *)dat, &len);
//printf((const char*)ciphertext);

char osslstr[1024];
strcpy( osslstr, "Salted__");
int salted_len = strlen( "Salted__");
osslstr[salted_len ] = 1;
osslstr[salted_len + 1] = 1;
osslstr[salted_len + 2] = 1;
osslstr[salted_len + 3] = 1;
osslstr[salted_len + 4] = 1;
osslstr[salted_len + 5] = 1;
osslstr[salted_len + 6] = 1;
osslstr[salted_len + 7] = 1;
strcpy( osslstr + salted_len + 8, (const char *)ciphertext);

char *b64 = b64_encode((const char *)osslstr);
//printf("%s\n", b64);

//printf( read_keystr_b64("U2FsdGVkX18BAQEBAQEBARzEvge7iJbvaKv0u8NCcAM="));

//decrypt("U2FsdGVkX18BAQEBAQEBARzEvge7iJbvaKv0u8NCcAM=", "test123");
encrypt("testhehe2", "test123");
decrypt("U2FsdGVkX18BAQEBAQEBAZBgm8H8YjYnliVa0uc/7A8=", "test123");
return;
*/

/*
	 char *keystr = read_keystr_b64("U2FsdGVkX18BAQEBAQEBARzEvge7iJbvaKv0u8NCcAM=");
	 printf("%s\n", keystr);

	 char *key = get_key(keystr);
	 char *sl = get_salt(keystr);

	 char *keystr2 = get_keystr(key, sl);
	 char *enc2 = b64_encode(keystr2);
	 printf(enc2);
	 */

/*
	 char *bla = b64_decode(b64);

	 char *key = get_key(bla);
	 char *sl = get_salt(bla);
	 char *keystr = get_keystr(key, sl);
	 printf("%s\n", keystr);

	 b64 = b64_encode((const char *)keystr);
	 printf(b64);
	 */

/*
	 return;
	 }
	 */

	char *get_salt(const char *str) {

		char *salt = (char *)malloc(SALT_LEN);
		memcpy( salt, str + SALTSTR_LEN, SALT_LEN);
		return salt;
	}

	char *get_dat(const char *str) {

		unsigned int dat_len = strlen(str) - SALT_LEN;

		char *dat = (char *)malloc(dat_len);
		memcpy( dat, str + SALTSTR_LEN + SALT_LEN, dat_len);
		return dat;
	}

	// create an openssl-compatible key
	char *get_keystr(const char *dat, const char *salt) {

		char *keystr = (char *)malloc(32);
		strcpy( keystr, "Salted__");
		memcpy( keystr + SALTSTR_LEN, salt, SALT_LEN);
		memcpy( keystr + SALTSTR_LEN + SALT_LEN, dat, strlen(dat));
		return keystr;
	}

	// read a base64-encoded key string (in openssl-compatible format)
	char *read_keystr_b64(const char *keystr) {

		const char *dec = b64_decode(keystr);
		char *dat = get_dat(dec);
		char *salt = get_salt(dec);
		char *keystr_dec = get_keystr(dat, salt);
		free(dat);
		free(salt);
		return keystr_dec;
	}

}

