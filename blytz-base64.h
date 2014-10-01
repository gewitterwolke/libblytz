#define B64_MAX_LINE_LEN 64

char *b64_encode(const char *str);
char *b64_encode(const char *str, bool use_newlines);

char *b64_decode(const char *str);
char *b64_decode(const char *str, bool use_newlines);
//char *b64_decode_wo_newlines(const char *str);
