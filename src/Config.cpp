
#include "host-stats.h"


bool Config::WriteConfig(std::string &ErrMsg)
{
	return WriteConfig(Config::GetDefaultConfigFile(), ErrMsg);
}

bool Config::WriteConfig(const std::string FileName, std::string &ErrMsg)
{
	throw;
}

bool Config::ReadConfig(std::string &ErrMsg)
{
	return ReadConfig(GetDefaultConfigFile(), ErrMsg);
}

bool Config::ReadConfig(const std::string FileName, std::string &ErrMsg)
{
	throw;
}

std::string Config::GetDefaultConfigFile()
{
	char *tmp = getenv("HOME");
	if (tmp == NULL)
		abort();

	return std::string(tmp) + "/.ipstatsrc";
}

