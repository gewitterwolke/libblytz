#define B64_MAX_LINE_LEN 63

char *b64_encode(const char *str);

char *b64_decode(const char *str);
char *b64_decode_wo_newlines(const char *str);
