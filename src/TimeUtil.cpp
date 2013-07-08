

#include "host-stats.h"


float TimeUtil::TvDiffMs(const struct timeval &tv1, const struct timeval &tv2)
{
	time_t nsec = tv1.tv_sec - tv2.tv_sec;
	suseconds_t nmicro = tv1.tv_usec - tv2.tv_usec;
	float total = (nsec * 1000) + (nmicro / 1000);
	return total;
}

