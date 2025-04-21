#include "../headers/parser.hpp"
#include "../headers/replys.hpp"
#include "../headers/channels.hpp"
// canonical form :

Parser::Parser(): server_password("") {}

Parser::Parser(std::string password): server_password(password) {}

const Parser & Parser::operator=(const Parser & other) {
   if (this != &other)
   {
		this->server_password = other.server_password;
		this->users = other.users;
		this->epoll_fd = other.epoll_fd;
   }
   return (*this); 
}

Parser::Parser(const Parser & other)
{
	*this = other;
}

Parser::~Parser()
{
	close(epoll_fd);
	for (size_t i = 0 ; i < this->users.size(); ++i)
	{
		close(users[i].get_fd());
	}
}

// ---------------------------

void    Parser::set_epoll_fd(int _epoll_fd)
{
	epoll_fd = _epoll_fd;
}

bool    Parser::check_user(int fd) {
	for (size_t i = 0; i < users.size(); ++i)
	{
		if (users[i].get_fd() == fd)
			return (true);
	}
	return (false);
}

User &    Parser::get_user(int fd) { // use check user before calling this !!
	for (size_t i = 0; i < users.size(); ++i)
	{
		if (users[i].get_fd() == fd)
			return (users[i]);
	}
	throw std::runtime_error("user not found");
}

void    Parser::add_user(int fd) {
	if (!check_user(fd)) {
		User new_user(fd);
		new_user.get_socket_address();
		users.push_back(new_user);
	}
}

void    Parser::remove_user(int fd)
{
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
	close(fd);
	const User & user = get_user(fd);
	users.erase(find(users.begin(), users.end(), user));
}

bool    Parser::check_nick_name(std::string nick)
{
	for (size_t i = 0; i < users.size(); ++i)
	{
		if (users[i].get_nick_name() == nick)
			return (false);
	}
	return (true);
}



//-------------------------------

void	cmd_line::clear()
{
	this->cmd.clear();
	this->args.clear();
	this->trailing.clear();
}

//-------------------------------

bool    Parser::isEnded(User & user)
{
	std::string user_buffer = user.get_buffer();
	if (user_buffer.size() < 2)
		return (false);
	if (user_buffer.substr(user_buffer.size() - 2, 2) == POSTFIX)
		return (true);
	return (false);
}

bool	Parser::check_auth(User & user)
{
	if (!user.get_auth())
	{
		user.send_reply(ERR_NOAUTH(user.get_nick_name()));
		return (false);
	}
	return (true);
}

void	Parser::process(struct epoll_event event) {

	int user_fd = event.data.fd;

	add_user(user_fd);
	User & user = get_user(user_fd);

	if (event.events == EPOLLIN)
	{
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
	
		ssize_t bytes_recv = recv(user_fd, buffer, BUFFER_SIZE, 0);
	
		switch (bytes_recv)
		{
			case -1:
				if (errno == EAGAIN || errno == EWOULDBLOCK) { // no more data to read
					break;
				} else {
					perror("recv");
					remove_user(user_fd);
					break;
				}

			case 0:
				remove_user(user_fd);
				break;
	
			default:
				process_buffer(user, buffer);
				break ;
		}
	}
}

void Parser::process_buffer(User &user, char* buffer)
{
	std::string new_data(buffer);

	if (new_data.size() > BUFFER_SIZE)
		new_data = new_data.substr(new_data.size() - BUFFER_SIZE);

	if (user.get_buffer().size() + new_data.size() > BUFFER_SIZE)
	{
		size_t excess = (user.get_buffer().size() + new_data.size()) - BUFFER_SIZE;
		if (excess >= user.get_buffer().size())
			user.clear_buffer();
		else
			user.set_buffer(user.get_buffer().substr(excess));
	}

	user.add_to_buffer(buffer);
	parse(user);
}

size_t count_crlf(const std::string& str)
{
    size_t count = 0;
    size_t pos = 0;
    const std::string target = "\r\n";
    
    while ((pos = str.find(target, pos)) != std::string::npos) {
        ++count;
        pos += target.length();
    }
    
    return count;
}

void Parser::parse(User &user) {
	std::string buffer = user.get_buffer();
	
	// If buffer doesn't contain at least one complete command, return
	if (!isEnded(user))
		return;

	std::vector<cmd_line> cmds;

	size_t pos = 0;
	size_t end_pos;

	while ((end_pos = buffer.find("\r\n", pos)) != std::string::npos)
	{
		std::string command = buffer.substr(pos, end_pos - pos);
		pos = end_pos + 2;
		
		cmd_line c;
		command.erase(command.find_last_not_of(" \t\r\n") + 1);
		
		size_t cmd_end = command.find(' ');
		c.cmd = command.substr(0, cmd_end);
		std::string remaining = (cmd_end != std::string::npos) ? command.substr(cmd_end + 1) : "";
		
		size_t colon_pos = remaining.find(" :");
		
		if (colon_pos != std::string::npos)
		{
			std::string non_trailing = remaining.substr(0, colon_pos);
			c.trailing = remaining.substr(colon_pos + 2);
			
			std::istringstream ss(non_trailing);
			std::string param;
			while (ss >> param)
				c.args.push_back(param);
		}
		else 
		{
			std::istringstream ss(remaining);
			std::string param;
			while (ss >> param)
				c.args.push_back(param);
		}
		
		cmds.push_back(c);
	}

	if (pos > 0)
		user.set_buffer(buffer.erase(0, pos));

	for (std::vector<cmd_line>::iterator it = cmds.begin(); it != cmds.end(); ++it)
	{
		redirect_cmd(user, *it);
		std::cout << *it << std::endl;
	}
}

void	Parser::redirect_cmd(User & user, cmd_line & c)
{
	std::string					cmd = c.cmd;
	std::vector<std::string>	args = c.args;
	std::string					trailing = c.trailing;
	
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
		else if (check_nick_name(args[0]) == false)
			user.send_reply(ERR_NICKNAMEINUSE(user.get_nick_name()));
		else if (!valid_nick_name(args[0]))
			user.send_reply(ERR_ERRONEUSNICKNAME(args[0]));
		else
		{
			std::string old_nick = user.get_nick_name();
			user.set_nick_name(args[0]);
			user.send_reply(RPL_NICK(old_nick, user.get_nick_name()));
		}
	}

	else if (cmd == "USER")
	{
		if (args.size() == 3 && !trailing.empty())
		{
			std::string	user_name = args[0];
			std::string	cmp_arg1 = args[1];
			std::string	cmp_arg2 = args[2];
			std::string real_name = trailing;


			if (!cmp_arg1.empty() &&
				!cmp_arg2.empty() &&
				!user_name.empty() &&
				!real_name.empty())
			{
				user.set_real_name("~" + real_name);
				user.set_user_name(user_name);
			}
			else
				user.send_reply(ERR_NEEDMOREPARAMS(std::string("USER")));
		}
		else
			user.send_reply(ERR_NEEDMOREPARAMS(std::string("USER")));
	}

	else if (cmd == "CAP")
	{
		if (args.size() != 1)
			user.send_reply(ERR_NEEDMOREPARAMS(std::string("CAP")));
		else if (args[0] == "LS")
			user.send_reply(":localhost CAP * ACK :\r\n");
		else if (args[0] == "END" && user.get_auth())
			user.send_reply(RPL_WELCOME(user.get_nick_name(), std::string("Welcome to the irc server !")));
	}

	else if (cmd == "QUIT")
	{
		std::string reason = args.empty() ? "Client quit" : args[0];
		user.send_reply("ERROR :Closing link: " + reason);
		remove_user(user.get_fd());
	}

	else if (cmd == "PING")
	{
		if (args.size() != 1)
			user.send_reply(ERR_NEEDMOREPARAMS(std::string("PING")));
		else
		{
			std::string	server_name = "localhost";
			std::string	token = args[0];
			user.send_reply(RPL_PONG(server_name, args[0]));
		}
	}
	else if (cmd == "PRIVMSG")
	{
		if (args.size() < 1 || trailing.empty())
			user.send_reply(ERR_NOTEXTTOSEND());
		else
			privmsg(user.get_fd(), args[0], trailing, user);
		
	}
	else if (cmd == "JOIN") {
		if (args.size() != 1)
			user.send_reply(ERR_NEEDMOREPARAMS(std::string("JOIN")));
		else
		{
			std::string channel_name = args[0];
			if (channel_name[0] != '#')
				user.send_reply(ERR_NOSUCHCHANNEL(channel_name));
			else
			{
				std::map<std::string, Channel>::iterator it = channels.find(channel_name);
				if (it != channels.end())
				{
					Channel &existing_channel = it->second;
					bool userAlreadyInChannel = false;
					const std::vector<User *> &current_users = existing_channel.get_users();
					for (size_t i = 0; i < current_users.size(); i++) {
						if (current_users[i]->get_fd() == user.get_fd()) {
							userAlreadyInChannel = true;
							break;
						}
					}
					if (!userAlreadyInChannel) {
						existing_channel.add_user(&user);
					}
				}
				else
				{
					Channel new_channel;
					channels[channel_name] = new_channel;
					Channel &channel_ref = channels[channel_name];
					channel_ref.add_user(&user);
				}
				
				user.send_reply(RPL_JOIN(user.get_nick_name(), channel_name));
				
				// Debug
				std::map<std::string, Channel>::iterator debug_it = channels.find(channel_name);
				if (debug_it != channels.end()) {
					const std::vector<User *> &users_in_channel = debug_it->second.get_users();
					std::cout << "Channel " << channel_name << " now has " 
							  << users_in_channel.size() << " users" << std::endl;
					std::cout << "Users in channel " << channel_name << ":" << std::endl;
					for (size_t i = 0; i < users_in_channel.size(); i++) {
						std::cout << " - " << users_in_channel[i]->get_nick_name() << std::endl;
					}
				}
			}
		}
	}
	else if (cmd == "TOPIC")
	{
		if (args.size() != 1)
			user.send_reply(ERR_NEEDMOREPARAMS(std::string("TOPIC")));
		
	}
	else if (cmd == "MODE")
	{
		handleModeCommand(&user, args);
	}
	else
		user.send_reply(ERR_UNKNOWNCOMMAND(cmd));
}

void Parser::privmsg(int fd, std::string receiver, std::string msg, User &user)
{
	(void)fd;
    if (receiver[0] == '#')
    {
        std::map<std::string, Channel>::iterator it = channels.find(receiver);
        if (it != channels.end())
        {
            Channel &channel = it->second;
            const std::vector<User *> &channel_users = channel.get_users();
            std::string formatted_msg = ":" + user.get_nick_name() + "!" + user.get_user_name() + "@localhost PRIVMSG " + receiver + " :" + msg + "\r\n";
            for (size_t i = 0; i < channel_users.size(); ++i)
            {
                User *target = channel_users[i];
                if (target->get_fd() != user.get_fd())
                    send(target->get_fd(), formatted_msg.c_str(), formatted_msg.size(), 0);
            }
        }
        else
            user.send_reply(ERR_NOSUCHCHANNEL(receiver));
    }
    else
    {
        bool found = false;
        for (size_t i = 0; i < users.size(); i++)
        {
            if (users[i].get_nick_name() == receiver)
            {
                std::string formatted_msg = ":" + user.get_nick_name() + "!" + user.get_user_name() + "@localhost PRIVMSG " + receiver + " :" + msg + "\r\n";
                send(users[i].get_fd(), formatted_msg.c_str(), formatted_msg.size(), 0);
                found = true;
                break;
            }
        }        
        if (!found)
            user.send_reply(ERR_NOSUCHNICK(receiver));
    }
}

// void	Parser::topic(User &user, std::string channel_name, std::string new_topic,  Channel &channel){
// 	if (channel_name.empty())
// 		user.send_reply(ERR_NOSUCHCHANNEL(channel_name));
// 	else{
		
// 	}	
// }


// void	Parser::mode(User &user, std::string target, std::string mode, Channel &channel){
// 	if (target[0] == '#')
//     {
//         std::map<std::string, Channel>::iterator it = channels.find(target);
//         if (it != channels.end())
//         {
//             Channel &channel = it->second;
            
//         }
//         else
//             user.send_reply(ERR_NOSUCHCHANNEL(receiver));
//     }

// }


void Parser::handleModeCommand(User* user, const std::vector<std::string>& args) {
    if (args.size() < 1) {
        user->send_reply(ERR_NEEDMOREPARAMS("MODE"));
        return;
    }

    const std::string& target = args[0];
    
    if (target[0] != '#') {
        user->send_reply(ERR_NOSUCHCHANNEL(target));
        return;
    }
    std::map<std::string, Channel>::iterator it = channels.find(target);
    if (it == channels.end())
		user->send_reply(ERR_NOSUCHCHANNEL(target));
	else{
	Channel &channel = it->second;

    if (args.size() == 1) {
        std::string modeStr = "+" + channel.get_modes();
        if (channel.has_mode(std::string("k"))) modeStr += " " + channel.get_key();
        if (channel.has_mode(std::string("k")))
		{
			std::ostringstream oss;
			oss << channel.get_user_limit();
			modeStr += " " + oss.str();
		}
        
        user->send_reply(RPL_CHANGEMODE(user->get_nick_name(), target, modeStr));
        user->send_reply(RPL_CHANGEMODE(user->get_nick_name(), target, channel.getCreationTime()));
        return;
    }

    if (!channel.is_operator(user)) {
        user->send_reply(ERR_CHANOPRIVSNEEDED(target));
        return;
    }
    const std::string& modestring = args[1];
    std::vector<std::string> modeArgs(args.begin() + 2, args.end());
    size_t argIndex = 0;
    char currentAction = '+';

    for (size_t i = 0; i < modestring.length(); ++i) {
		char c = modestring[i];
        if (c == '+' || c == '-') {
            currentAction = c;
            continue;
        }

        switch (c) {
            case 'i':
            case 't':
                channel.set_mode(std::string(1,c), currentAction == '+');
                break;
                
            case 'k':
                if (currentAction == '+') {
                    if (argIndex < modeArgs.size()) {
                        channel.set_key_mode(modeArgs[argIndex++], 1);
                    }
                } else {
                    channel.set_key_mode("", 1);
                }
                break;
                
            case 'l':
                if (currentAction == '+') {
                    if (argIndex < modeArgs.size()) {
                        channel.set_user_limits(1,std::atoi(modeArgs[argIndex++].c_str()));
                    }
                } else {
                    channel.set_user_limits(1,0);
                }
                break;
                
            case 'o': {
                if (argIndex < modeArgs.size()) {
                    User* targetUser = &get_user(atoi(modeArgs[argIndex++].c_str()));
                    if (targetUser) {
                        if (currentAction == '+') {
                            channel.set_operators_mode(1,targetUser);
                        } else {
                            channel.remove_operator(targetUser);
                        }
                    }
                }
                break;
            }
                
            default:
                user->send_reply(ERR_UNKNOWNMODE(user->get_nick_name(), target, std::string(1,c)));
                continue;
        }
	}
    }
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