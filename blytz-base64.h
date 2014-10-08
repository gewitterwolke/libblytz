#define B64_MAX_LINE_LEN 64

//unsigned int b64_get_encoded_len(unsigned int len, bool use_newlines);
unsigned int b64_get_decoded_len(const char* b64input, bool use_newlines);

char *b64_encode(const char *str);
char *b64_encode(const char *str, unsigned int len);
char *b64_encode_nnl(const char *str, unsigned int len);
char *b64_encode(const char *str, unsigned int len, bool use_newlines);
char *b64_encode(const char *str, unsigned int len,
		bool use_newlines, bool trailing_newline);

char *b64_decode_nnl(const char *str, unsigned int *len);
char *b64_decode(const char *str);
char *b64_decode(const char *str, unsigned int *len);
char *b64_decode(const char *str, unsigned int *len, bool trailing_newline);

extern char ERR[];
