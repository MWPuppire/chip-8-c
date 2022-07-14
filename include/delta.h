#pragma once

#include <time.h>

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L \
	&& !defined(DELTA_TIME_LOW_RES)

inline static long double deltatime(long double *lasttime) {
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	long double time = (long double) ts.tv_sec
		+ (long double) ts.tv_nsec / 1e9;
	long double diff = time - *lasttime;
	*lasttime = time;
	return diff;
}

#else
#ifndef DELTA_TIME_LOW_RES
#define DELTA_TIME_LOW_RES
#endif

inline static long double deltatime(long double *lasttime) {
	clock_t val = clock();
	long double time = (long double) val / CLOCKS_PER_SEC;
	long double diff = time - *lasttime;
	*lasttime = time;
	return diff;
}

#endif
