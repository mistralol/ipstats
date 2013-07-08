

#include "host-stats.h"


double TimeUtil::TvDiffMs(const struct timeval &tv1, const struct timeval &tv2)
{
	//Convert to MS
	double t1 = (tv1.tv_sec * 1000) + (tv1.tv_usec / 1000);
	double t2 = (tv2.tv_sec * 1000) + (tv2.tv_usec / 1000);
	double ms = t1 - t2;

	if (ms < 0)
		abort();

	return ms;
}

