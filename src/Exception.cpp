
#include "host-stats.h"

Exception::Exception(const std::string str)
{
	m_message = str;
}

Exception::Exception(const char *str)
{
	m_message = str;
}

Exception::Exception(char *str)
{
	m_message = str;
}

Exception::Exception(int err)
{
	m_message = strerror(err);
}

Exception::~Exception() throw()
{

}

const char *Exception::what() const throw()
{
	return m_message.c_str();
}

