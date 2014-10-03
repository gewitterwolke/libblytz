#include <cstdarg>
#include <cstdio>

#include <time.h>

#include "blytz-debug.h"

namespace blytz {

int printfd1(const char *fmt, ...) {

#ifdef BLYTZ_DEBUG
		va_list vl;

		time_t t = time(0);
		tm* lt = localtime(&t);

		FILE *df;
#if DEBUG_TARGET == 2
		df = fopen("/tmp/blytz-debug.log", "a");
		fprintf(df, "---------------\n");
		fflush(df);
#else 
		df = stdout;
#endif

		va_start(vl, fmt);

		fprintf( df, "%02d-%02d %02d:%02d - ", lt->tm_mon, lt->tm_wday, 
				lt->tm_hour, lt->tm_min);

		int ret = vfprintf( df, fmt, vl);

		va_end(vl);

		fflush( df);

		return ret;
#else
		return 0;
#endif
	}

}
