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

Counter::Counter()
{
	Reset();
}

Counter::~Counter()
{

}

void Counter::Reset()
{
	m_TotalPackets = 0;
	m_TotalBytes = 0;
	m_OldTotalPackets = 0;
	m_OldTotalBytes = 0;

	m_PacketSpeed = 0;
	m_BytesSpeed = 0;

	if (gettimeofday(&m_LastSpeeds, NULL) < 0)
		abort();
}

void Counter::Add(const uint64_t Packets, uint64_t Bytes)
{
	m_TotalPackets += Packets;
	m_TotalBytes += Bytes;
}

void Counter::GetTotals(uint64_t *TotalPackets, uint64_t *TotalBytes)
{
	*TotalPackets = m_TotalPackets;
	*TotalBytes = m_TotalBytes;
}

uint64_t Counter::GetTotalPackets()
{
	return m_TotalPackets;
}

uint64_t Counter::GetTotalBytes()
{
	return m_TotalBytes;
}

void Counter::GetSpeeds(double *PacketsPerSecond, double *BytesPerSecond)
{
	struct timeval tv;

	if (gettimeofday(&tv, NULL) < 0)
		abort();

	double ms = TimeUtil::TvDiffMs(tv, m_LastSpeeds);

	if (ms > 2000)
	{
		uint64_t PacketDiff = m_TotalPackets - m_OldTotalPackets;
		uint64_t BytesDiff = m_TotalBytes - m_OldTotalBytes;

		m_PacketSpeed = (PacketDiff / ms) * 1000;
		m_BytesSpeed = (BytesDiff / ms) * 1000;

		m_OldTotalPackets = m_TotalPackets;
		m_OldTotalBytes = m_TotalBytes;

		memcpy(&m_LastSpeeds, &tv, sizeof(tv));
	}

	*PacketsPerSecond = m_PacketSpeed;
	*BytesPerSecond = m_BytesSpeed;
}

double Counter::GetPacketSpeed()
{
	double PPS, BPS;
	GetSpeeds(&PPS, &BPS);
	return PPS;
}

double Counter::GetBytesSpeed()
{
	double PPS, BPS;
	GetSpeeds(&PPS, &BPS);
	return BPS;
}

