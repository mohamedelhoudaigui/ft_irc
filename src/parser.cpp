#include "../headers/parser.hpp"


Parser::Parser(): server_password("") {}

Parser::Parser(std::string password): server_password(password) {}

const Parser & Parser::operator=(const Parser & other) {
   if (this != &other) {
		// nothing to copy
   }
   return (*this); 
}

Parser::Parser(const Parser & other) {
	*this = other;
}

Parser::~Parser() {}


//-------------------------------


void    Parser::parse(User & user)
{
	if (!isEnded(user)) // here we still buffer (we need \r\n)
	{
		std::cout << "still buffring" << std::endl;
		return ;
	}

	cmd_line	c;
	std::string	buffer = user.get_buffer();


	buffer = buffer.substr(0, buffer.find_last_not_of("\r\n") + 1);

	size_t cmd_end = buffer.find(' ');
	c.cmd = buffer.substr(0, cmd_end);
	buffer = (cmd_end != std::string::npos) ? buffer.substr(cmd_end + 1) : "";

	size_t colon_pos = buffer.find(" :");

	if (colon_pos != std::string::npos)
	{
		std::string non_trailing = buffer.substr(0, colon_pos);
		c.trailing = buffer.substr(colon_pos + 2);

		std::istringstream ss(non_trailing);
		std::string param;
		while (ss >> param)
			c.args.push_back(param);
	}
	else
	{
		std::istringstream ss(buffer);
		std::string param;
		while (ss >> param)
			c.args.push_back(param);
	}

	std::cout << c;
	redirect_cmd(user, c);
}

void	Parser::redirect_cmd(User & user, cmd_line c)
{
	std::string&				cmd = c.cmd;
	std::vector<std::string>&	args = c.args;
	std::string&				trailing = c.trailing;
	(void)trailing;

	if (cmd == "PASS")
	{
		if (user.get_auth() == true) // user already registered
		{
			user.send_reply(ERR_ALREADYREGISTERED(user.get_nick_name()));
			return ;
		}

		if (args.size() < 1) // pass with no params
		{
			user.send_reply(ERR_NEEDMOREPARAMS(std::string("PASS")));
		}
		else if (args[0] != server_password) // password not correct
		{
			user.set_auth(false);
			user.send_reply(ERR_PASSWDMISMATCH(user.get_nick_name()));
		}
		else // password is correct
			user.set_auth(true);
	}
	else if (cmd == "NICK")
	{
		// not implemented yet
	}
	else if (cmd == "USER")
	{
		// not implemented yet
	}
	else
	{
		user.send_reply(ERR_UNKNOWNCOMMAND(cmd));
	}
}


bool    Parser::isEnded(User & user)
{
	std::string & user_buffer = user.get_buffer();
	if (user_buffer.size() < 2)
		return (false);
	if (user_buffer.substr(user_buffer.size() - 2, 2) == POSTFIX)
		return (true);
	return (false);
}

// -------------------------------

std::ostream& operator<<(std::ostream& os, const cmd_line& c)
{
    os << "Command: " << c.cmd << std::endl;
    
    if (!c.args.empty())
	{
        os << "Arguments:";
        for (size_t i = 0; i < c.args.size(); ++i)
            os << " " << c.args[i];
        os << std::endl;
    }
    
    if (!c.trailing.empty())
        os << "Trailing: " << c.trailing << std::endl;

    return os;
}