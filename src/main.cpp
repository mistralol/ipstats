
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

