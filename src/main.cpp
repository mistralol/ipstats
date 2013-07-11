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

#include <stdlib.h>

#include "host-stats.h"


int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cout << "Usage: " << argv[0] << " <interface>" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::string ifname(argv[1]);

	try
	{
		PCap Capture(ifname);
		Hosts HostStats;

		Capture.HandlerAdd((IPCapHandler *) &HostStats);
		Capture.Start();

		Gui G;
		G.MainLoop(Capture, HostStats);

		Capture.Stop();
		Capture.HandlerRemove((IPCapHandler *) &HostStats);
	}
	catch(std::exception ex)
	{
		std::cout << "Exception: " << ex.what() << std::endl;
	}
	
	return 0;
}

