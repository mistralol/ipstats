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




