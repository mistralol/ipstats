
struct HostStats
{
	std::string *Addr;
	uint64_t SourcePackets;
	uint64_t SourceBytes;
	uint64_t DestPackets;
	uint64_t DestBytes;

	uint64_t OldSourcePackets;
	uint64_t OldSourceBytes;
	uint64_t OldDestPackets;
	uint64_t OldDestBytes;

	struct timeval LastUpdate;
	struct timeval LastStatsUpdate;
};

class Hosts :
	public IPCapHandler
{
	public:
		Hosts();
		virtual ~Hosts();

		virtual void PCapPacket(const struct pcap_pkthdr *hdr, const u_char *buf);

		int GetCount();
		bool GetStats(const std::string Address, struct HostStats *);
		bool GetStats(unsigned int idx, struct HostStats *);
		void GetTotals(uint64_t *TotalPackets, uint64_t *TotalBytes);

		void UpdateSrc(const std::string Address, uint64_t Bytes);
		void UpdateDest(const std::string Address, uint64_t Bytes);
		void Sort();
		void Clear();
		void ClearHost(const std::string Address);
		void ResetHost(const std::string Address);


	private:
		void HostAdd(const std::string Address);
		void ZeroHost(struct HostStats *Stats);

	private:
		boost::recursive_mutex m_mutex;
		std::map<std::string, struct HostStats *> m_hosts;
		std::vector<struct HostStats *> m_vhosts;
		uint64_t m_TotalPackets;
		uint64_t m_TotalBytes;
};




