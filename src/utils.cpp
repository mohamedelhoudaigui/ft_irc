#include "../headers/utils.hpp"

static bool is_all_numeric(const std::string& str)
{
    if (str.empty())
		return (false);

    for (size_t i = 0 ; i < str.size(); ++i)
	{
        if (!isdigit(str[i]))
            return (false);
    }

    return (true);
}

int	parse_port(std::string port)
{
	if (!port.empty() && is_all_numeric(port))
	{
		std::stringstream s(port);
		int result;
		s >> result;
		if (result <= MAX_PORT && result > 0)
			return (result);
	}

	std::cerr << "invalid port value" << std::endl;
	exit(1);
}

std::string	convert_to_lower(std::string s)
{
	for (size_t i = 0; i <s.size(); ++i)
	{
		s[i] = tolower(s[i]);
	}
	return (s);
}

bool	valid_nick_name(std::string s)
{
	if (s.size() < 1 || s.size() > 20 ||
		s == "*" || s[0] == ':' || s[0] == '#' ||
		s.find(' ') != std::string::npos)
	{
		return (false);
	}
	return (true);
}

void    valid_password(std::string password)
{
    if (password.size() < 4 || password.size() > 20)
    {
        std::cerr << "password must be between 4 and 20 charcters, only alphanumeric" << std::endl;
        exit(1);
    }

    for (size_t i = 0; i < password.size(); ++i)
    {
        if (!isalnum(password[i]))
        {
            std::cerr << "password must be between 4 and 20 charcters, only alphanumeric" << std::endl;
            exit(1);
        }
    }
}

std::string	get_now_time()
{
	time_t rawtime;
    time(&rawtime);

    struct tm *timeinfo;
    timeinfo = localtime(&rawtime);

    char buffer[80];
    strftime(buffer, 80, "%a %b %d %H:%M:%S %Y", timeinfo);

	return std::string(buffer);
}
