// copied and adapted from
// https://svn.cs.clemson.edu/repos/JSORBER_CPSC3600-S14/security/openssl_aes.c

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

#include <string.h>

#include "blytz-enc.h"
#include "blytz-base64.h"
#include "blytz-debug.h"

// Caution: logging in here is a security risk

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

		// max ciphertext len for n bytes of plaintext is n + AES_BLOCK_SIZE -1 bytes
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

	const char *encrypt(const char *str, const char *pwd) {
		return encrypt(str, pwd, true);
	}

	// encrypt message str using password pwd using AES and Base64 and password
	// pwd
	// (replaces newlines with '!' after Base64 encryption
	const char *encrypt(const char *str, const char *pwd, bool replace_newlines) {


		unsigned char *salt = (unsigned char *) malloc(SALT_LEN);
		//RAND_bytes(salt, SALT_LEN);

		salt[0] = 1;
		salt[1] = 1;
		salt[2] = 1;
		salt[3] = 1;
		salt[4] = 1;
		salt[5] = 1;
		salt[6] = 1;
		salt[7] = 1;
		/*
		*/

		/*
		int test[1000];
		for (int i = 0; i < SALT_LEN; i++) {
			test[i] = salt[i];
			printf("%02x ", test[i]);
		}

		printf("\n");
		*/

		unsigned int pwdlen = strlen(pwd);

		EVP_CIPHER_CTX en, de;

		if (aes_init((unsigned char *)pwd, pwdlen, salt, &en, &de)) {
			printfe("Couldn't initialize AES cipher\n");
			return INVALID;
		}

		int len = strlen(str);
		char *strnl = (char *) calloc(1, len + 1);
		strcpy(strnl, str);
		strcat(strnl, "\n");
		len++;
		printfd("Length to encrypt: %d\n", len);

		printfd("Encrypting %s with salt %s and pwd %s\n", strnl, salt, pwd);

		unsigned char *dat = aes_encrypt(&en, (unsigned char *)strnl, &len);

		printf("Enc dat:\n");
		for (int i = 0; i < len; i++) {
			printf("%02x ", dat[i]);
		}
		printf("\n");
		unsigned char *keystr = get_keystr((const unsigned char*)dat, 
				(unsigned int)len, salt);

		printf("Keystr: %s\n", keystr);

		//char *enc = b64_encode_wo_trailing_nl((char *)keystr, 16 + len);
		char *enc = b64_encode((char *)keystr, 16 + len);

		// replace newlines
		if (replace_newlines) {
			for (unsigned int i = 0; i < strlen(enc); i++) {
				if (enc[i] == '\n') {
					enc[i] = '!';
				}
			}
		}

		free(keystr);
		free(salt);
		free(dat);

		return enc;
	}

	const char *decrypt(const char *str, const char *pwd) {
		return decrypt(str, pwd, true);
	}

	// decrypt a AES and Base64 encoded message str using password pwd
	// (replaces '!' with '\n' before decryption assuming they have been added by
	// encrypt() or BLYTZ App before)
	// 
	const char *decrypt(const char *str, const char *pwd, bool replace_newlines) {

		char *str2 = (char *) calloc(1, strlen(str));
		//strcpy(str2, str);

		// strip leading and trailing quotation marks and replace newlines
		for (unsigned int i = 0, j = 0; i < strlen(str); i++) {

			char c = str[i];

			if (c == '!' && replace_newlines) {
				str2[j++] = '\n';
			} else if (c == '\"') {
				
			} else {
				str2[j++] = c;
			}
		}

		bool has_newlines = false;
		if (strstr(str, "\n")) {
			has_newlines = true;
		}

		// debug output
		printfd("Incoming string for decryption (after replacing '!'): %s\n", str2);

		unsigned int len; // = get_decoded_len(str2);
		//printfd("Estimated length of decoded: %d\n", len);

		char *dec = b64_decode_nnl(str2, &len, has_newlines);
		unsigned char *salt = get_salt(dec, len);

		printfd("Salt: %s\n", salt);

		printfd("Base64 decoded: %s\n", dec);

		unsigned int pwdlen = strlen(pwd);

		EVP_CIPHER_CTX en, de;

		// initialize AES using salt from incoming string
		if (aes_init((unsigned char *)pwd, pwdlen, (unsigned char *)salt, 
					&en, &de)) {
			printfe("Couldn't initialize AES cipher\n");
			return INVALID;
		}

		unsigned char *dat = get_dat(dec, len);

		printf("dat to decrypt:\n");
		for (int i = 0; i < len; i++) {
			printf("%02x ", dat[i]);
		}
		printf("\n");
		//int len = declen - strlen(salt);
		//len = strlen(dat);
		
		len -= (SALTSTR_LEN + SALT_LEN);

		// actual AES decryption
		unsigned char *plain = aes_decrypt(&de, (unsigned char *)dat, (int *)&len);

		printfd("Decrypted String: %s (length: %d)\n", plain, len);

		free(salt);
		free(dec);
		free(dat);
		free(str2);

		return (char *)plain;
	}

	unsigned char *get_salt(const char *str, unsigned int len) {

		if (len < SALTSTR_LEN + SALT_LEN) {
			printfe("Cannot get salt, data too short\n");
			return (unsigned char *)ERR;
		}

		unsigned char *salt = (unsigned char *) calloc(1, SALT_LEN);
		memcpy( salt, str + SALTSTR_LEN, SALT_LEN);
		return salt;
	}

	unsigned char *get_dat(const char *str, unsigned int len) {

		if (len < SALT_LEN + SALTSTR_LEN) {
			printfe("Cannot get payload data, data too short\n");
			return (unsigned char *)ERR;
		}

		unsigned int dat_len = len - SALT_LEN - SALTSTR_LEN;

		unsigned char *dat = (unsigned char *)malloc(dat_len);
		memcpy( dat, str + SALTSTR_LEN + SALT_LEN, dat_len);
		return dat;
	}

	// create an openssl-compatible key
	unsigned char *get_keystr(const unsigned char *dat, unsigned int len, 
			const unsigned char *salt) {

		unsigned int totlen = len + SALTSTR_LEN + SALT_LEN;
		printfd("Length of enc data: %d\n", len);

		unsigned char *keystr = (unsigned char *)calloc(1, totlen);

		char saltstr[] = "Salted__";
		memcpy( keystr, saltstr, SALTSTR_LEN);
		memcpy( keystr + SALTSTR_LEN, salt, SALT_LEN);
		memcpy( keystr + SALTSTR_LEN + SALT_LEN, dat, len);

		for (int i = 0; i < totlen; i++) {
			printf("%02x ", keystr[i]);
		}
		printf("\n");
		return keystr;
	}

	// read a base64-encoded key string (in openssl-compatible format)
	unsigned char *read_keystr_b64(const char *keystr, unsigned int len) {

		unsigned int declen;
		const char *dec = b64_decode(keystr, &declen);

		unsigned char *salt = (unsigned char *)get_salt(dec, declen);
		unsigned char *dat = (unsigned char *)get_dat(dec, len);
		unsigned char *keystr_dec = get_keystr(dat, len, salt);
		free(dat);
		free(salt);
		return keystr_dec;
	}

}
