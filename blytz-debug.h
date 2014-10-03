#include <cstdio>
#include <cstdarg>

namespace blytz {
#define BLYTZ_DEBUG 1

// 1 = stdout, 2 = /tmp/blytz-debug.log, 3 = custom (not implemented yet)
#define DEBUG_TARGET 2

#ifdef BLYTZ_DEBUG
#if DEBUG_TARGET == 2
#define printfd( FMT, ...) {\
	time_t t = time(0); \
	tm* lt = localtime(&t); \
	\
	FILE *df; \
	df = fopen("/tmp/blytz-debug.log", "a"); \
	fflush(df);\
	\
	fprintf( df, "%02d-%02d %02d:%02d - %s:%d | " FMT, \
			lt->tm_mon + 1, lt->tm_mday, \
			lt->tm_hour, lt->tm_min, \
			__FILE__, __LINE__, \
			##__VA_ARGS__);\
	fflush(df);\
	fclose(df);\
}
#else
#define printfd( FMT, ...) {\
	time_t t = time(0); \
	tm* lt = localtime(&t); \
	\
	fprintf( stdout, "%02d-%02d %02d:%02d - %s:%d | " FMT, \
			lt->tm_mon + 1, lt->tm_mday, \
			lt->tm_hour, lt->tm_min, \
			__FILE__, __LINE__, \
			##__VA_ARGS__);\
}
#endif
#else
#define printfd( FMT, ...)
#endif

//	int printfd(const char *fmt, ...);

}
