/*

  This file is part of ipstats, a network monitoring utility
  The original author can be contacted at james@stev.org or http://www.stev.org/
  The source code can be found at https://github.com/mistralol/ipstats

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/


#include <host-stats.h>


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

