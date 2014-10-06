namespace blytz {
#define SALTSTR_LEN 8
#define SALT_LEN 8
	unsigned char *get_dat(const char *str, unsigned int len);
	unsigned char *get_salt(const char *str, unsigned int len);

	unsigned char *get_keystr(const unsigned char *key, unsigned int len, 
			const unsigned char *salt);
	char *read_keystr_b64(const char *keystr, const char *pwd);

	const char INVALID[] =  "---";
	const char *encrypt(const char *str, const char *pwd);
	const char *encrypt(const char *str, const char *pwd, bool replace_newlines);
	const char *decrypt(const char *str, const char *pwd);
	const char *decrypt(const char *str, const char *pwd, bool replace_newlines);
}
