#include "../headers/parser.hpp"
#include "../headers/user_management.hpp"


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


void    Parser::parse(User & user, UserManag & user_manag)
{
	if (!isEnded(user)) // here we still buffer (we need \r\n)
		return ;

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

	std::cout << c << std::endl;
	redirect_cmd(user, c, user_manag);
	c.cmd.clear();
	c.args.clear();
	c.trailing.clear();
}

void	Parser::redirect_cmd(User & user, cmd_line c, UserManag & user_manag)
{
	std::string&				cmd = c.cmd;
	std::vector<std::string>&	args = c.args;
	std::string&				trailing = c.trailing;

	// maybe we could restrict the command args numbers ?

	if (cmd == "PASS")
	{
		if (user.get_auth() == true)
			user.send_reply(ERR_ALREADYREGISTERED(user.get_nick_name()));
		else if (args.size() != 1)
			user.send_reply(ERR_NEEDMOREPARAMS(std::string("PASS")));
		else if (args[0] != server_password)
			user.send_reply(ERR_PASSWDMISMATCH(user.get_nick_name()));
		else
			user.set_auth(true);
	}

	else if (cmd == "NICK")
	{
		if (args.size() > 1)
			user.send_reply(ERR_NEEDMOREPARAMS(std::string("NICK")));
		else if (args.size() < 1)
			user.send_reply(ERR_NONICKNAMEGIVEN(user.get_nick_name()));
		else if (user_manag.check_nick_name(args[0]) == false)
			user.send_reply(ERR_NICKNAMEINUSE(user.get_nick_name()));
		else if (!valid_nick_name(args[0]))
			user.send_reply(ERR_ERRONEUSNICKNAME(args[0]));
		else
			user.set_nick_name(args[0]);
	}

	else if (cmd == "USER")
	{
		if (args.size() == 4 || (args.size() == 3 && !trailing.empty()))
		{
			std::string	user_name = args[0];
			std::string	cmp_arg1 = args[1];
			std::string	cmp_arg2 = args[2];
			std::string real_name = args.size() == 3 ? trailing : args[3];

			if (cmp_arg1 == "0" &&
				cmp_arg2 == "*" &&
				!user_name.empty() &&
				!real_name.empty())
			{
				user.set_real_name(real_name);
				user.set_user_name(user_name);
			}
			else
			{
				user.send_reply(ERR_NEEDMOREPARAMS(std::string("USER")));
			}
		}
		else
			user.send_reply(ERR_NEEDMOREPARAMS(std::string("USER")));
	}
	else // unknow command
	{
		user.send_reply(ERR_UNKNOWNCOMMAND(cmd));
	}

	user.clear_buffer();
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