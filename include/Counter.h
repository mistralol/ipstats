

class Counter
{
	public:
		Counter();
		~Counter();

		void Reset();
		void Add(const uint64_t Packets, uint64_t Bytes);
		void GetTotals(uint64_t *TotalPackets, uint64_t *TotalBytes);
		uint64_t GetTotalPackets();
		uint64_t GetTotalBytes();

		void GetSpeeds(double *PacketsPerSecond, double *BytesPerSecond);
		double GetPacketSpeed();
		double GetBytesSpeed();


	private:
		uint64_t m_TotalPackets;
		uint64_t m_TotalBytes;

		uint64_t m_OldTotalPackets;
		uint64_t m_OldTotalBytes;

		double m_PacketSpeed;
		double m_BytesSpeed;
		double m_OldPacketSpeed;
		double m_OldBytesSpeed;

		struct timeval m_LastSpeeds;
};

