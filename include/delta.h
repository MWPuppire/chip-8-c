#pragma once

#include <time.h>

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L \
	&& !defined(DELTA_TIME_LOW_RES)

inline static double deltatime(double *lasttime) {
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	double time = (double) ts.tv_sec + (double) ts.tv_nsec / 1e9;
	double diff = time - (lasttime == 0 ? 0.0 : *lasttime);
	lasttime != 0 && (*lasttime = time);
	return diff;
}

#else
#	ifndef DELTA_TIME_LOW_RES
#		define DELTA_TIME_LOW_RES
#	endif

inline static double deltatime(double *lasttime) {
	clock_t val = clock();
	double time = (double) val / CLOCKS_PER_SEC;
	double diff = time - (lasttime == 0 ? 0.0 : *lasttime);
	lasttime != 0 && (*lasttime = time);
	return diff;
}

#endif
