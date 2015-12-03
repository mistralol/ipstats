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


#include <host-stats.h>

PCap::PCap(std::string interface)
{
	m_ifname = interface;
	m_running = false;
	m_thread = NULL;
	m_pcap = NULL;
}

PCap::~PCap()
{
	if (IsRunning())
		Stop();
}

void PCap::HandlerAdd(IPCapHandler *handler)
{
	boost::mutex::scoped_lock lock(m_mutex);
	m_handlers.push_back(handler);
}

void PCap::HandlerRemove(IPCapHandler *handler)
{
	boost::mutex::scoped_lock lock(m_mutex);
	std::list<IPCapHandler *>::iterator it = m_handlers.begin();

	while(it != m_handlers.end())
	{
		if (*it == handler)
			break;
		it++;
	}

	if (it != m_handlers.end())
		m_handlers.erase(it);
}

void PCap::Start()
{
	if (IsRunning() || m_thread != NULL)
		throw(Exception("Already Running"));

	char err[512];
	m_pcap = pcap_open_live(m_ifname.c_str(), sizeof(err), 0, 0, err);
	if (m_pcap == NULL)
	{
		throw(Exception(err));
	}

	m_running = true;
	m_thread = new boost::thread( boost::bind ( Run, this ) );

}

void PCap::Stop()
{
	m_running = false;
	if (m_thread != NULL)
	{
		m_thread->join();
		delete m_thread;
		m_thread = NULL;
	}

	if (m_pcap != NULL)
	{
		pcap_close(m_pcap);
		m_pcap = NULL;
	}
}

bool PCap::IsRunning()
{
	if (m_running)
		return true;
	return false;
}

void PCap::Run(PCap *self)
{
	while(self->IsRunning())
	{
		struct pcap_pkthdr hdr;
		const u_char *buf = pcap_next(self->m_pcap, &hdr);
		if (buf == NULL)
			break;

		boost::mutex::scoped_lock lock(self->m_mutex);
		std::list<IPCapHandler *>::iterator it;
		for(it = self->m_handlers.begin(); it != self->m_handlers.end(); it++)
		{
			IPCapHandler *p = *it;
			p->PCapPacket(&hdr, buf);
		}

	}
	self->m_running = false;
}


