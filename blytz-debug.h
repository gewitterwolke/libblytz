#include <cstdio>
#include <cstdarg>

// TODO: code reuse in macros

namespace blytz {

// 2 = /tmp/blytz-debug.log, 3 = custom (not implemented yet), 1/else = stdout
#define DEBUG_TARGET 2

// 0 = nothing, 1 = error, 2 = warn, 3 = debug
#define DEBUG_LEVEL 3

#if DEBUG_TARGET == 2 // file
#if DEBUG_LEVEL == 3
#define printfd( FMT, ...) {\
	time_t t = time(0); \
	tm* lt = localtime(&t); \
	\
	FILE *df; \
	df = fopen("/tmp/blytz-debug.log", "a"); \
	fflush(df);\
	\
	fprintf( df, "[DEBUG] %02d-%02d %02d:%02d - %s:%d | " FMT, \
			lt->tm_mon + 1, lt->tm_mday, \
			lt->tm_hour, lt->tm_min, \
			__FILE__, __LINE__, \
			##__VA_ARGS__);\
	fflush(df);\
	fclose(df);\
}
#else
#define printfd( FMT, ...)
#endif
#if DEBUG_LEVEL > 1
#define printfw( FMT, ...) {\
	time_t t = time(0); \
	tm* lt = localtime(&t); \
	\
	FILE *df; \
	df = fopen("/tmp/blytz-debug.log", "a"); \
	fflush(df);\
	\
	fprintf( df, "[WARN] %02d-%02d %02d:%02d - %s:%d | " FMT, \
			lt->tm_mon + 1, lt->tm_mday, \
			lt->tm_hour, lt->tm_min, \
			__FILE__, __LINE__, \
			##__VA_ARGS__);\
	fflush(df);\
	fclose(df);\
}
#else
#define printfw( FMT, ...)
#endif
#if DEBUG_LEVEL > 0
#define printfe( FMT, ...) {\
	time_t t = time(0); \
	tm* lt = localtime(&t); \
	\
	FILE *df; \
	df = fopen("/tmp/blytz-debug.log", "a"); \
	fflush(df);\
	\
	fprintf( df, "[ERROR] %02d-%02d %02d:%02d - %s:%d | " FMT, \
			lt->tm_mon + 1, lt->tm_mday, \
			lt->tm_hour, lt->tm_min, \
			__FILE__, __LINE__, \
			##__VA_ARGS__);\
	fflush(df);\
	fclose(df);\
}
#else
#define printfe( FMT, ...)
#endif
#else // stdout
#if DEBUG_LEVEL == 3
#define printfd( FMT, ...) {\
	time_t t = time(0); \
	tm* lt = localtime(&t); \
	\
	fprintf( stdout, "[DEBUG] %02d-%02d %02d:%02d - %s:%d | " FMT, \
			lt->tm_mon + 1, lt->tm_mday, \
			lt->tm_hour, lt->tm_min, \
			__FILE__, __LINE__, \
			##__VA_ARGS__);\
}
#else
#define printfd( FMT, ...)
#endif
#if DEBUG_LEVEL > 1
#define printfw( FMT, ...) {\
	time_t t = time(0); \
	tm* lt = localtime(&t); \
	\
	fprintf( stdout, "[WARN] %02d-%02d %02d:%02d - %s:%d | " FMT, \
			lt->tm_mon + 1, lt->tm_mday, \
			lt->tm_hour, lt->tm_min, \
			__FILE__, __LINE__, \
			##__VA_ARGS__);\
}
#else
#define printfw( FMT, ...)
#endif
#if DEBUG_LEVEL > 0
#define printfe( FMT, ...) {\
	time_t t = time(0); \
	tm* lt = localtime(&t); \
	\
	fprintf( stdout, "[ERROR] %02d-%02d %02d:%02d - %s:%d | " FMT, \
			lt->tm_mon + 1, lt->tm_mday, \
			lt->tm_hour, lt->tm_min, \
			__FILE__, __LINE__, \
			##__VA_ARGS__);\
}
#else
#define printfe( FMT, ...)
#endif
#endif

//	int printfd(const char *fmt, ...);

}
