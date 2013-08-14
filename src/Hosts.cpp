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

bool HostsSortByRxPackets(struct HostStats *a, struct HostStats *b)
{
	uint64_t RxPackets1 = a->Dest->GetTotalPackets();
	uint64_t RxPackets2 = b->Dest->GetTotalPackets();

	if (RxPackets1 == RxPackets2)
		return HostsSortByAddress(a, b);

	return RxPackets1 > RxPackets2;
}

bool HostsSortByRxBytes(struct HostStats *a, struct HostStats *b)
{
	uint64_t RxBytes1 = a->Dest->GetTotalBytes();
	uint64_t RxBytes2 = b->Dest->GetTotalBytes();

	if (RxBytes1 == RxBytes2)
		return HostsSortByAddress(a, b);

	return RxBytes1 > RxBytes2;
}

bool HostsSortByTxPackets(struct HostStats *a, struct HostStats *b)
{
	uint64_t RxPackets1 = a->Source->GetTotalPackets();
	uint64_t RxPackets2 = b->Source->GetTotalPackets();

	if (RxPackets1 == RxPackets2)
		return HostsSortByAddress(a, b);

	return RxPackets1 > RxPackets2;
}

bool HostsSortByTxBytes(struct HostStats *a, struct HostStats *b)
{
	uint64_t RxBytes1 = a->Source->GetTotalBytes();
	uint64_t RxBytes2 = b->Source->GetTotalBytes();

	if (RxBytes1 == RxBytes2)
		return HostsSortByAddress(a, b);

	return RxBytes1 > RxBytes2;
}

bool HostsSortByTotalPackets(struct HostStats *a, struct HostStats *b)
{
	uint64_t Packets1 = a->Source->GetTotalPackets() + a->Dest->GetTotalBytes();
	uint64_t Packets2 = b->Source->GetTotalPackets() + a->Dest->GetTotalBytes();

	if (Packets1 == Packets2)
		return HostsSortByAddress(a, b);

	return Packets1 > Packets2;
}

bool HostsSortByTotalBytes(struct HostStats *a, struct HostStats *b)
{
	uint64_t Bytes1 = a->Source->GetTotalBytes() + a->Dest->GetTotalBytes();
	uint64_t Bytes2 = b->Source->GetTotalBytes() + a->Dest->GetTotalBytes();

	if (Bytes1 == Bytes2)
		return HostsSortByAddress(a, b);

	return Bytes1 > Bytes2;
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

int Hosts::GetAddressIdx(const std::string &Addr)
{
	return GetAddressIdx(Addr, 0);
}

int Hosts::GetAddressIdx(const std::string &Addr, int StartIdx)
{
	int idx = StartIdx;
	boost::recursive_mutex::scoped_lock lock(m_mutex);
	for(size_t i = StartIdx; i < m_vhosts.size(); i++, idx++)
	{
		struct HostStats *p = m_vhosts[i];
		if (*(p->Addr) == Addr)
			return idx;
	}

	return 0; /* Not Found */
}

void Hosts::GetTotals(uint64_t *TotalPackets, uint64_t *TotalBytes)
{
	Totals.GetTotals(TotalPackets, TotalBytes);
}

void Hosts::GetTotalSpeed(double *PacketsPerSecond, double *BytesPerSecond)
{
	Totals.GetSpeeds(PacketsPerSecond, BytesPerSecond);
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
	Stats->Source->Add(1, Bytes);
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
	Stats->Dest->Add(1, Bytes);
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
		Stats->Source = new Counter();
		Stats->Dest = new Counter();
		ZeroHost(Stats);
		m_hosts[Address] = Stats;
		m_vhosts.push_back(Stats);
		Sort();
	}
}

void Hosts::Sort()
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);
	std::sort(m_vhosts.begin(), m_vhosts.end(), HostsSortByAddress);
}

void Hosts::Clear()
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);
	for(std::map<std::string, struct HostStats *>::iterator it = m_hosts.begin(); it != m_hosts.end(); it++)
	{
		struct HostStats *Stats = it->second;
		delete Stats->Addr;
		delete Stats->Source;
		delete Stats->Dest;
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
	Stats->Dest->Reset();
	Stats->Source->Reset();
}


