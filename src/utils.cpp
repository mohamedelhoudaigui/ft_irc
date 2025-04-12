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
	if (!port.empty() && port.size() <= 5 && is_all_numeric(port))
	{
		std::stringstream s(port);
		int result;
		s >> result;
		if (result <= MAX_PORT)
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
	if (s.size() < 1 ||
		s == "*" || s[0] == ':' ||
		s[0] == '#' ||
		s.find(' ') != std::string::npos)
	{
		return (false);
	}
	return (true);
}

