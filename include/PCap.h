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




