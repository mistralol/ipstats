
struct HostStats
{
	std::string *Addr;
	Counter *Source;
	Counter *Dest;
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
		int GetAddressIdx(const std::string &Addr);
		int GetAddressIdx(const std::string &Addr, int StartIdx);
		void GetTotals(uint64_t *TotalPackets, uint64_t *TotalBytes);
		void GetTotalSpeed(double *PacketsPerSecond, double *BytesPerSecond);

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
		Counter Totals;
};




