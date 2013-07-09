
class Config
{
	public:
		static bool WriteConfig(std::string &ErrMsg);
		static bool WriteConfig(const std::string FileName, std::string &ErrMsg);

		static bool ReadConfig(std::string &ErrMsg);
		static bool ReadConfig(const std::string FileName, std::string &ErrMsg);

		static std::string GetDefaultConfigFile();

};


