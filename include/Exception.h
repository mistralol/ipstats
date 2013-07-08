

class Exception :
	public std::exception
{
	public:
		Exception(const std::string str);
		Exception(const char *str);
		Exception(char *str);
		Exception(int err);
		virtual ~Exception() throw();

		virtual const char *what() const throw();

	private:
		std::string m_message;
};

