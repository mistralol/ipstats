

class Gui
{
	public:
		Gui();
		~Gui();

		void ShowVersion(WINDOW *parent);
		void ShowMainHelp(WINDOW *parent);

		void MainLoop(PCap &Pcap, Hosts &Hosts);

		bool WaitInput();
		bool WaitInput(struct timeval *tv);
		void Delay();
		void Delay(struct timespec *ts);

	private:
		std::string PacketsToHuman(uint64_t value);
		std::string BytesToHuman(uint64_t value);

	private:
		WINDOW *m_screen;

};

