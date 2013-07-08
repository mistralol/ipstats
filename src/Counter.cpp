

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

void Counter::GetTotalSpeed(float *PacketsPerSecond, float *BytesPerSecond)
{
	struct timeval tv;
	*PacketsPerSecond = 0.0f;
	*BytesPerSecond = 0.0f;

	if (gettimeofday(&tv, NULL) < 0)
		abort();

	float ms = TimeUtil::TvDiffMs(tv, m_LastSpeeds);
	
	if (ms > 5000)
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

