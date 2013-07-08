
#include "host-stats.h"

Hosts::Hosts()
{

}

Hosts::~Hosts()
{
	Clear();
}

static inline std::string ntohstr(u_int32_t addr)
{
	struct in_addr in;
	in.s_addr = addr;
	return inet_ntoa(in);
}

bool HostsSortByAddress(struct HostStats *a, struct HostStats *b)
{
	in_addr_t a1 = inet_addr(a->Addr->c_str());
	in_addr_t b1 = inet_addr(b->Addr->c_str());

	uint32_t a2 = ntohl(a1);
	uint32_t b2 = ntohl(b1);

	return a2 > b2;
}

void Hosts::PCapPacket(const struct pcap_pkthdr *hdr, const u_char *buf)
{
    /*  struct pcap_pkthdr {
        struct timeval ts;   time stamp 
        bpf_u_int32 caplen;  length of portion present 
        bpf_u_int32;         lebgth this packet (off wire) 
        }
     */
	const u_char *current = buf;
	unsigned int len = hdr->len;
	unsigned int caplen = hdr->caplen;

	if (caplen < sizeof(struct ether_header))
		return;

	struct ether_header *eth = (struct ether_header *) current;

	switch(ntohs(eth->ether_type))
	{
		case ETHERTYPE_IP:
			current += sizeof(*eth);
			caplen -= sizeof(*eth);

			if (caplen < sizeof(struct iphdr))
				return;

			do {
				struct iphdr *ip = (struct iphdr *) current;
				caplen -= sizeof(*ip);
				//unsigned int ip_len = ntohs(ip->tot_len);

				std::string IPSrc = ntohstr(ip->saddr);
				std::string IPDest = ntohstr(ip->daddr);

				Totals.Add(1, len);

				UpdateSrc(IPSrc, len);
				UpdateDest(IPDest, len);

				//std::cout << "IPSource: " << IPSrc << " IPDest " << IPDest << " Length: " << ip_len << std::endl;
			} while(0);

		break;
	default:
		//std::cout << "Unknown Packet Type: " << ntohs(eth->ether_type) << " Cap Length: " << caplen << " Length: " << len << std::endl;
		break;
	}
}

int Hosts::GetCount()
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);
	return m_hosts.size();
}

bool Hosts::GetStats(const std::string Address, struct HostStats *Stats)
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);
	std::map<std::string, struct HostStats *>::iterator it = m_hosts.find(Address);
	if (it == m_hosts.end())
		return false;
	memcpy(Stats, it->second, sizeof(*Stats));
	return true;
}

bool Hosts::GetStats(unsigned int idx, struct HostStats *Stats)
{
	struct HostStats *p = (struct HostStats *) m_vhosts[idx];
	memcpy(Stats, p, sizeof(*Stats));
	return true;
}

void Hosts::GetTotals(uint64_t *TotalPackets, uint64_t *TotalBytes)
{
	Totals.GetTotals(TotalPackets, TotalBytes);
}

void Hosts::GetTotalSpeed(float *PacketsPerSecond, float *BytesPerSecond)
{
	Totals.GetTotalSpeed(PacketsPerSecond, BytesPerSecond);
}

void Hosts::UpdateSrc(const std::string Address, uint64_t Bytes)
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);
	std::map<std::string, struct HostStats *>::iterator it = m_hosts.find(Address);
	if (it == m_hosts.end())
	{
		HostAdd(Address);
		UpdateSrc(Address, Bytes);
		return;
	}

	struct HostStats *Stats = it->second;
	Stats->SourcePackets++;
	Stats->SourceBytes += Bytes;
	if (gettimeofday(&Stats->LastUpdate, NULL) < 0)
		abort();

}

void Hosts::UpdateDest(const std::string Address, uint64_t Bytes)
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);
	std::map<std::string, struct HostStats *>::iterator it = m_hosts.find(Address);
	if (it == m_hosts.end())
	{
		HostAdd(Address);
		UpdateDest(Address, Bytes);
		return;
	}

	struct HostStats *Stats = it->second;
	Stats->DestPackets++;
	Stats->DestBytes += Bytes;
}

void Hosts::HostAdd(const std::string Address)
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);
	std::map<std::string, struct HostStats *>::iterator it = m_hosts.find(Address);
	if (it == m_hosts.end())
	{
		struct HostStats *Stats = (struct HostStats *) malloc(sizeof(*Stats));
		if (!Stats)
			throw(Exception("Malloc Failed"));

		Stats->Addr = new std::string(Address);
		ZeroHost(Stats);
		m_hosts[Address] = Stats;
		m_vhosts.push_back(Stats);
		Sort();
	}
}

void Hosts::Sort()
{
	std::sort(m_vhosts.begin(), m_vhosts.end(), HostsSortByAddress);
}

void Hosts::Clear()
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);
	for(std::map<std::string, struct HostStats *>::iterator it = m_hosts.begin(); it != m_hosts.end(); it++)
	{
		struct HostStats *Stats = it->second;
		delete Stats->Addr;
		free(Stats);
	}
	m_hosts.clear();
	m_vhosts.clear();
	Totals.Reset();
}

void Hosts::ClearHost(const std::string Address)
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);
	std::map<std::string, struct HostStats *>::iterator it = m_hosts.find(Address);
	if (it != m_hosts.end())
	{
		struct HostStats *Stats = it->second;

		for(std::vector<struct HostStats *>::iterator it2 = m_vhosts.begin(); it2 != m_vhosts.end(); it2++)
		{
			if (*it2 == Stats)
			{
				m_vhosts.erase(it2);
				break;
			}
		}
		m_hosts.erase(it);

		delete Stats->Addr;
		free(Stats);
	}
}

void Hosts::ResetHost(const std::string Address)
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);
	std::map<std::string, struct HostStats *>::iterator it = m_hosts.find(Address);
	if (it != m_hosts.end())
	{
		struct HostStats *Stats = it->second;
		ZeroHost(Stats);
	}
}

void Hosts::ZeroHost(struct HostStats *Stats)
{
	Stats->SourcePackets = 0;
	Stats->SourceBytes = 0;
	Stats->DestPackets = 0;
	Stats->DestBytes = 0;

	Stats->OldSourcePackets = 0;
	Stats->OldSourceBytes = 0;
	Stats->OldDestPackets = 0;
	Stats->OldDestBytes = 0;

	memset(&Stats->LastUpdate, 0, sizeof(Stats->LastUpdate));
	memset(&Stats->LastStatsUpdate, 0, sizeof(Stats->LastStatsUpdate));
}


