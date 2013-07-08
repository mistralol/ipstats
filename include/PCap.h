


class PCap
{
	public:
		PCap(std::string interface);
		~PCap();

		void HandlerAdd(IPCapHandler *);
		void HandlerRemove(IPCapHandler *);

		void Start();
		void Stop();
		bool IsRunning();

	private:
		static void Run(PCap *self);

	private:
		pcap_t *m_pcap;
		boost::thread *m_thread;
		boost::mutex m_mutex;

		std::string m_ifname;
		std::list<IPCapHandler *> m_handlers;
		volatile bool m_running;
};




