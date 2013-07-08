

class IPCapHandler
{
	public:
		virtual ~IPCapHandler() { };

		virtual void PCapPacket(const struct pcap_pkthdr *hdr, const u_char *buf) = 0;
};


