
#include "host-stats.h"

Gui::Gui()
{

}

Gui::~Gui()
{

}

void Gui::ShowVersion(WINDOW *parent)
{
	int pWidth, pHeight;
	getmaxyx(m_screen, pHeight, pWidth);
	bool Loop = true;
	int nLines = 3;
	int nCols = 20;
	int LocX = (pWidth / 2) - (nCols / 2);
	int LocY = (pHeight / 2) - (nLines / 2);
	WINDOW *window = newwin(nLines, nCols, LocY, LocX);
	int maxy, maxx;
	getmaxyx(m_screen, maxy, maxx);

	while(Loop)
	{
		werase(window);

		box(window, 0, 0);

		mvwprintw(window, 0, 1, "Version");
		mvwprintw(window, 1, 1, "%s", Version::GetVersion().c_str());

		wrefresh(window);

		if (WaitInput())
		{
			int input = getch();

			switch(input)
			{
				case ERR:
					break;
				default:
					Loop = false;
					break;
			}
		}
	}
	delwin(window);
}

void Gui::ShowMainHelp(WINDOW *parent)
{
	int pWidth, pHeight;
	getmaxyx(m_screen, pHeight, pWidth);
	bool Loop = true;
	int nLines = 20;
	int nCols = 50;
	int LocX = (pWidth / 2) - (nCols / 2);
	int LocY = (pHeight / 2) - (nLines / 2);
	WINDOW *window = newwin(nLines, nCols, LocY, LocX);
	int maxy, maxx;
	getmaxyx(m_screen, maxy, maxx);

	while(Loop)
	{
		werase(window);

		box(window, 0, 0);

		mvwprintw(window, 0, 1, "Help");

		int cury = 2;

		mvwprintw(window, cury++, 1, " ?/h - Show Help");
		mvwprintw(window, cury++, 1, "   a - Show About Information");
		mvwprintw(window, cury++, 1, "   c - Clear All Stats");
		mvwprintw(window, cury++, 1, "   d - Clear stats for selected host");
		mvwprintw(window, cury++, 1, "   r - Reset Stats for selected host");
		mvwprintw(window, cury++, 1, "   v - Show Version Information");
		mvwprintw(window, cury++, 1, "   q - Quit");

		wrefresh(window);

		if (WaitInput())
		{
			int input = getch();

			switch(input)
			{
				case ERR:
					break;
				default:
					Loop = false;
					break;
			}
		}
	}
	delwin(window);
}

void Gui::MainLoop(PCap &Capture, Hosts &HostInfo)
{
	bool Running = true;
	int SelectedHost = 0;
	m_screen = initscr();
	
	cbreak();
	noecho();

	keypad(stdscr, TRUE);

	while(Capture.IsRunning() && Running == true)
	{
		int maxy, maxx;
		int rxcol = 20;
		int txcol = 50;
		getmaxyx(m_screen, maxy, maxx);

		werase(m_screen);
		box(m_screen, 0, 0);

		int nHosts = HostInfo.GetCount();
		mvwprintw(m_screen, 0, 1, "Hosts (%d/%d)", SelectedHost + 1, nHosts);
		mvwprintw(m_screen, 0, rxcol, "RX Packets  RX Bytes");
		mvwprintw(m_screen, 0, txcol, "TX Packets  TX Bytes");

		struct HostStats HostData;

		int StartIdx = 0;

		if (SelectedHost + 3 > maxy)
			StartIdx = SelectedHost + 3 - maxy;

		if (StartIdx < 0)
			StartIdx = 0;

		for(int i =0;i<maxy - 2;i++)
		{
			if (i + StartIdx >= nHosts)
				break;

			if (HostInfo.GetStats(i + StartIdx, &HostData) == false)
				break;

			if (i + StartIdx == SelectedHost)
				wattron(m_screen, A_STANDOUT);

			mvwprintw(m_screen, i + 1, 1, "%s", HostData.Addr->c_str());
			mvwprintw(m_screen, i + 1, rxcol, "%.10s", PacketsToHuman(HostData.DestPackets).c_str());
			mvwprintw(m_screen, i + 1, rxcol + 12, "%.15s", BytesToHuman(HostData.DestBytes).c_str());

			mvwprintw(m_screen, i + 1, txcol, "%.10s", PacketsToHuman(HostData.SourcePackets).c_str());
			mvwprintw(m_screen, i + 1, txcol + 12, "%.15s", BytesToHuman(HostData.SourceBytes).c_str());

			if (i + StartIdx == SelectedHost)
				wattroff(m_screen, A_STANDOUT);
		}

		uint64_t TotalPackets = 0;
		uint64_t TotalBytes = 0;
		float TotalPacketsPerSecond = 0.0f;
		float TotalBytesPerSecond = 0.0f;
		HostInfo.GetTotals(&TotalPackets, &TotalBytes);
		HostInfo.GetTotalSpeed(&TotalPacketsPerSecond, &TotalBytesPerSecond);

		mvwprintw(m_screen, maxy - 1, 1, "Total Packets: %s Bytes: %s  Packets Per Second: %.2f Bytes Per Second: %.2f",
				PacketsToHuman(TotalPackets).c_str(), BytesToHuman(TotalBytes).c_str(),
				TotalPacketsPerSecond, TotalBytesPerSecond);
		wrefresh(m_screen);

		if (WaitInput())
		{
			int input = getch();

			switch(input)
			{
				case ERR:
					break;
				case '?':
				case 'h':
				case 'H':
					ShowMainHelp(m_screen);
					break;
				//r - reset stats for host
				//d - delete selected host
				//s - Change host sorting type (Host / RX / TX / Total)
				case 'a': /* About */
				case 'A':
					throw;
					break;
				case 'c': /* Clear Stats */
				case 'C':
					SelectedHost = 0;
					HostInfo.Clear();
					box(m_screen, 0, 0);
					mvwprintw(m_screen, maxy - 1, 2, "Reset Stats");
					wrefresh(m_screen);
					Delay();
					break;
				case 'd': /* Delete Selected Host */
				case 'D':
					if (nHosts == 0) //Edge Case For empty list
						break;
					if (HostInfo.GetStats(SelectedHost, &HostData) == false)
						break;

					HostInfo.ClearHost(*HostData.Addr);
					nHosts = HostInfo.GetCount();
					if (SelectedHost >= nHosts - 1)
						SelectedHost = nHosts - 1;
					break;

				case 'r': /* Reset Selected Host */
				case 'R':
					if (nHosts == 0) //Edge Case For empty list
						break;
					if (HostInfo.GetStats(SelectedHost, &HostData) == false)
						break;
					HostInfo.ResetHost(*HostData.Addr);
					break;

				case 'v': /* Version */
				case 'V':
					ShowVersion(m_screen);
					break;

				case 'q': /* Quit */
				case 'Q':
					Running = false;
					break;

				case KEY_DOWN: /* Down */
					if (SelectedHost < nHosts - 1)
						SelectedHost++;
					break;
				case KEY_UP: /* Up */
					if (SelectedHost > 0)
						SelectedHost--;
					break;
				case KEY_NPAGE: /* Page Down */
					SelectedHost += 15;
					if (SelectedHost > nHosts - 1)
						SelectedHost = nHosts - 1;
					break;
				case KEY_PPAGE: /* Page Up */
					SelectedHost -= 15;
					if (SelectedHost < 0)
						SelectedHost = 0;
					break;
				case KEY_HOME: /* Home / Jump to Start */
					SelectedHost = 0;
					break;
				case KEY_END: /* End / Jump To End */
					SelectedHost = nHosts - 1;
					break;
				default:
					box(m_screen, 0, 0);
					mvwprintw(m_screen, maxy - 1, 2, "Unknown Command '%c'", input);
					wrefresh(m_screen);
					Delay();
					break;
			}
		}
	}

	endwin();
	delscreen(NULL);
}

bool Gui::WaitInput()
{
	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = 100 * 1000;	//Default 0.1 seconds

	return WaitInput(&tv);
}

bool Gui::WaitInput(struct timeval *tv)
{
	fd_set fds;

	FD_ZERO(&fds);
	FD_SET(fileno(stdin), &fds);

retry:
	int ret = select(fileno(stdin) + 1, &fds, NULL, NULL, tv);
	if (ret < 0)
	{
		int err = errno;
		switch(errno)
		{
			case EINTR:
				goto retry;
				break;
			default:
				throw(Exception(err));
				break;
		}
	}

	//Timeout
	if (ret == 0)
		return false;

	//Data?
	if (FD_ISSET(fileno(stdin), &fds))
		return true;

	return false;
}

void Gui::Delay()
{
	struct timespec ts;
	ts.tv_sec = 1;
	ts.tv_nsec = 0;	//Default 1 seconds
	Delay(&ts);
}

void Gui::Delay(struct timespec *ts)
{
	struct timespec rem;
restart:
	int ret = nanosleep(ts, &rem);
	if (ret < 0)
	{
		int err = errno;
		switch(err)
		{
			case EINTR:
				ts->tv_sec = rem.tv_sec;
				ts->tv_nsec = rem.tv_nsec;
				goto restart;
				break;
			default:
				throw(Exception(err));
				break;
		}
	}
}

std::string Gui::PacketsToHuman(uint64_t value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}

std::string Gui::BytesToHuman(uint64_t value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}
