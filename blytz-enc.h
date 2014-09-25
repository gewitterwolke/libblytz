namespace blytz {
#define SALTSTR_LEN 8
#define SALT_LEN 8
	char *get_dat(const char *str);
	char *get_salt(const char *str);
	char *get_keystr(const char *key, const char *salt);
	char *read_keystr_b64(const char *keystr, const char *pwd);

	const char INVALID_PASSWORD[] =  "---";
	const char * encrypt(const char *str, const char *pwd);
	const char * decrypt(const char *str, const char *pwd);
}
