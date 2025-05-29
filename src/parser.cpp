#include "../headers/parser.hpp"
#include "../headers/replys.hpp"
#include "../headers/channels.hpp"
#include "../headers/server.hpp"

// canonical form :

Parser::Parser(): server_password(""), server(NULL) {}

Parser::Parser(std::string password, Server* server): server_password(password), server(server) {}

const Parser & Parser::operator=(const Parser & other)
{
   if (this != &other)
   {
		this->server_password = other.server_password;
		for (size_t i = 0; i < this->users.size(); ++i) {
			delete this->users[i];
		}
		this->users.clear();
		for (size_t i = 0; i < other.users.size(); ++i) {
			User* new_user = new User(*other.users[i]);
			this->users.push_back(new_user);
		}
   }
   return (*this); 
}

Parser::Parser(const Parser & other)
{
	*this = other;
}

Parser::~Parser()
{
	for (size_t i = 0 ; i < this->users.size(); ++i)
	{
		delete users[i];
	}
}

// ---------------------------

bool    Parser::check_user(int fd) {
	for (size_t i = 0; i < users.size(); ++i)
	{
		if (users[i]->get_fd() == fd)
			return (true);
	}
	return (false);
}

User*    Parser::get_user(int fd)
{
	for (size_t i = 0; i < users.size(); ++i)
	{
		if (users[i]->get_fd() == fd)
			return (users[i]);
	}
	return (NULL);
}

void    Parser::add_user(int fd)
{
	if (!check_user(fd))
	{
		User* new_user = new User(fd);
		new_user->get_socket_address();
		users.push_back(new_user);
	}
}

void Parser::broadcast(User& user, std::string msg)
{
    std::map<std::string, Channel>::iterator it;
    for (it = channels.begin(); it != channels.end(); ++it)
    {
        if (it->second.has_user(&user))
        {
            const std::vector<User*>& channel_users = it->second.get_users();
            for (size_t i = 0; i < channel_users.size(); ++i)
            {
                if (channel_users[i]->get_fd() != user.get_fd())
                {
                    channel_users[i]->send_reply(msg);
                }
            }
        }
    }
}

void    Parser::remove_user(int fd)
{
	User* user = get_user(fd);
	if (!user)
		return ;

	std::vector<User*>::iterator user_it = std::find(users.begin(), users.end(), user);
	if (user_it != users.end())
		users.erase(user_it);

	std::map<std::string, Channel>::iterator it;
	for (it = channels.begin(); it != channels.end(); ++it)
	{
		User *u = find_user_by_nickname(it->second, user->get_nick_name());
		if (u)
		{
			it->second.remove_user(u);
		}
	}
	delete user;
}

bool    Parser::check_nick_name(std::string nick)
{
	for (size_t i = 0; i < users.size(); ++i)
	{
		if (users[i]->get_nick_name() == nick)
			return (false);
	}
	return (true);
}

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

void	Parser::process(struct pollfd event) {

	int user_fd = event.fd;

	add_user(user_fd);
	User* user = get_user(user_fd);
	if (!user)
		return ;

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);

	ssize_t bytes_recv = recv(user_fd, buffer, BUFFER_SIZE, 0);
	
	if (bytes_recv <= 0)
	{
		if (bytes_recv < 0)
			perror("recv");
		server->remove_client(user_fd);
		return ;
	}
	else
	{
		process_buffer(*user, buffer, bytes_recv);
	}
	
}

void Parser::process_buffer(User &user, char* buffer, ssize_t bytes_recv)
{
	std::string new_data(buffer, bytes_recv);

	if (bytes_recv > BUFFER_SIZE)
		new_data = new_data.substr(0, new_data.size() - BUFFER_SIZE);

	if (user.get_buffer().size() + new_data.size() > BUFFER_SIZE)
	{
		size_t excess = (user.get_buffer().size() + new_data.size()) - BUFFER_SIZE;
		if (excess >= user.get_buffer().size())
			user.clear_buffer();
		else
			user.set_buffer(user.get_buffer().substr(excess));
	}

	user.add_to_buffer(buffer, bytes_recv);
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

void Parser::parse(User &user)
{
	std::string buffer = user.get_buffer();

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
		
		size_t colon_pos = remaining.find(":");
		
		if (colon_pos != std::string::npos)
		{
			std::string non_trailing = remaining.substr(0, colon_pos);
			c.trailing = remaining.substr(colon_pos + 1);
			
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

	for (size_t i = 0; i < cmds.size() ; ++i)
	{
		redirect_cmd(user, cmds[i]);
		std::cout << cmds[i] << std::endl;
	}
}

void	Parser::process_auth(User & user)
{
	if (user.get_nick_step() && user.get_user_step() && user.get_pass_step())
	{
		user.send_reply(RPL_WELCOME(user.get_nick_name(), std::string("welcome to irc server !")));
		user.send_reply(RPL_YOURHOST(user.get_nick_name()));
		user.send_reply(RPL_CREATED(user.get_nick_name(), server->get_creation_time()));
		user.set_auth(true);
		user.set_nick_step(false);
		user.set_user_step(false);
		user.set_pass_step(false);
	}
}

std::string first_word(const std::string& input)
{
    std::string result;
    std::size_t i = 0;

    while (i < input.length() && !std::isspace(input[i])) {
        result += input[i];
        i++;
    }
    return result;
}


void	Parser::redirect_cmd(User & user, cmd_line & c)
{
	std::string					cmd = c.cmd;
	std::vector<std::string>	args = c.args;
	std::string					trailing = c.trailing;
	
	if (cmd == "PASS")
	{
		if (args.size() != 1)
			user.send_reply(ERR_NEEDMOREPARAMS(std::string("PASS")));
		else if (user.get_auth() == true)
			user.send_reply(ERR_ALREADYREGISTERED(user.get_nick_name()));
		else if (args[0] != server_password)
			user.send_reply(ERR_PASSWDMISMATCH(user.get_nick_name()));
		else
		{
			if (!user.get_auth())
				user.set_pass_step(true);
			process_auth(user);
		}
	}

	else if (cmd == "NICK")
	{
		if (args.size() < 1)
			user.send_reply(ERR_NONICKNAMEGIVEN(user.get_nick_name()));
		else if (check_nick_name(args[0]) == false)
			user.send_reply(ERR_NICKNAMEINUSE(user.get_nick_name()));
		else if (!valid_nick_name(args[0]))
			user.send_reply(ERR_ERRONEUSNICKNAME(args[0]));
		else
		{
			std::string old_nick = user.get_nick_name();
			user.set_nick_name(args[0]);
			if (!user.get_auth())
				user.set_nick_step(true);
	
			// this makes the client now the change
			user.send_reply(RPL_NICK(old_nick, user.get_nick_name()));

			// this is to inform other users:
       		broadcast(user, RPL_NICKCHANGE(old_nick, user.get_user_name(), user.get_nick_name()));

			process_auth(user);
		}
	}

	else if (cmd == "USER")
	{
		if ((args.size() == 3 && !trailing.empty()) || (args.size() == 4 && trailing.empty()))
		{
			std::string	user_name = args[0];
			std::string	cmp_arg1 = args[1];
			std::string	cmp_arg2 = args[2];
			std::string real_name = trailing.empty() ? args[3] : trailing;

			if (!cmp_arg1.empty() &&
				!cmp_arg2.empty() &&
				!user_name.empty() &&
				!real_name.empty())
			{
				user.set_real_name("~" + real_name);
				user.set_user_name(user_name);
				if (!user.get_auth())
					user.set_user_step(true);

				process_auth(user);
			}
			else
			{
				user.send_reply(ERR_NEEDMOREPARAMS(std::string("USER")));
			}
		}
		else
		{
			user.send_reply(ERR_NEEDMOREPARAMS(std::string("USER")));
		}
	}
	else if (cmd == "PING" || cmd == "PONG")
	{
		if (args.size() != 1)
			user.send_reply(ERR_NEEDMOREPARAMS(std::string("PING")));
		else
		{
			std::string	server_name = "FT_IRC";
			std::string	token = args[0];
			user.send_reply(RPL_PONG(server_name, args[0]));
		}
	}

	else if (cmd == "QUIT")
	{
		std::string reason;

		if (!trailing.empty())
			reason = trailing;
		else if (args.size() > 0)
			reason = args[0];
		else
			reason = "Leaving...";

		int user_fd = user.get_fd();
		server->remove_client(user_fd);
	}

	else if (cmd == "PRIVMSG")
	{
		if (!check_auth(user))
			return ;
		if (args[0].empty())
			user.send_reply(ERR_NORECIPIENT(user.get_nick_name(), "PRIVMSG"));
		if (args.size() < 1 && trailing.empty())
			user.send_reply(ERR_NOTEXTTOSEND());
		else if (!trailing.empty())
		{
			privmsg(args[0], trailing, user);
		}
		else if (args.size() < 2)
			user.send_reply(ERR_NOTEXTTOSEND());
		else
			privmsg(args[0], args[1], user);
		
	}
	else if (cmd == "JOIN")
	{
		if (!check_auth(user))
			return ;
		if (args.empty())
			user.send_reply(ERR_NEEDMOREPARAMS(std::string("JOIN")));
		else
		{
			std::vector<std::string> channel_names;
			std::vector<std::string> channel_keys;

			std::string channels_str = args[0];
			size_t pos = 0;
			while ((pos = channels_str.find(',')) != std::string::npos) {
				channel_names.push_back(channels_str.substr(0, pos));
				channels_str.erase(0, pos + 1);
			}
			channel_names.push_back(channels_str);

			if (args.size() > 1) {
				std::string keys_str = args[1];
				pos = 0;
				while ((pos = keys_str.find(',')) != std::string::npos) {
					channel_keys.push_back(keys_str.substr(0, pos));
					keys_str.erase(0, pos + 1);
				}
				channel_keys.push_back(keys_str);
			}

			bool userAlreadyInChannel = false;
			for (size_t i = 0; i < channel_names.size(); ++i) {
				std::string channel_name = channel_names[i];
				std::string provided_key = (i < channel_keys.size()) ? channel_keys[i] : "";

				if (channel_name[0] != '#') {
					user.send_reply(ERR_NOSUCHCHANNEL(channel_name));
					continue;
				}

				std::map<std::string, Channel>::iterator it = channels.find(channel_name);
				if (it != channels.end()) {
					Channel &existing_channel = it->second;
					if (existing_channel.has_mode('i') && !existing_channel.is_invited(&user)) {
						user.send_reply(ERR_INVITEONLYCHAN(user.get_nick_name(), channel_name));
						continue;
					}
					if (existing_channel.has_mode('k')) {
						if (provided_key.empty() || provided_key != existing_channel.get_key()) {
							user.send_reply(ERR_BADCHANNELKEY(user.get_nick_name(), channel_name));
							continue;
						}
					}
					if (existing_channel.has_mode('l') && 
						existing_channel.get_users().size() >= existing_channel.get_user_limit()) {
						std::cout << existing_channel.get_user_limit() << " and " << existing_channel.get_users().size() << std::endl;
						user.send_reply(ERR_CHANNELISFULL(user.get_nick_name(), channel_name));
						continue;
					}
					const std::vector<User *> &current_users = existing_channel.get_users();
					for (size_t j = 0; j < current_users.size(); j++) {
						if (current_users[j]->get_fd() == user.get_fd()) {
							userAlreadyInChannel = true;
							break;
						}
					}
					if (!userAlreadyInChannel) {
						existing_channel.add_user(&user);
						user.send_reply(RPL_JOIN(user.get_nick_name(), channel_name));
					}
				}
				else {
					Channel new_channel;
					channels[channel_name] = new_channel;
					Channel &channel_ref = channels[channel_name];
					channel_ref.set_name(channel_name.substr(1));
					channel_ref.add_user(&user);
					channel_ref.set_operators_mode(true, &user);
					user.send_reply(RPL_JOIN(user.get_nick_name(), channel_name));
				}

				if (!userAlreadyInChannel) {
					Channel *existing_channel = &channels[channel_name];
					if (existing_channel == NULL) {
						return;
					}
					const std::vector<User *> &current_users = existing_channel->get_users();
					
					std::string join_message = RPL_JOINMSG(user.get_nick_name(), user.get_user_name(), channel_name);
					for (size_t j = 0; j < current_users.size(); j++) {
						current_users[j]->send_reply(join_message);
					}

					std::string names_list;
					for (size_t j = 0; j < current_users.size(); j++) {
						if (j > 0)
							names_list += " ";
						if (existing_channel->is_operator(current_users[j]))
							names_list += "@";
						names_list += current_users[j]->get_nick_name();
					}
					user.send_reply(RPL_NAMREPLY(user.get_nick_name(), channel_name, names_list));
					user.send_reply(RPL_ENDOFNAMES(user.get_nick_name(), channel_name));

					if (!existing_channel->get_topic().empty())
						user.send_reply(RPL_TOPIC(user.get_nick_name(), channel_name, existing_channel->get_topic()));
				}
			}
		}
	}
	else if (cmd == "INVITE")
	{
		if (!check_auth(user))
			return ;
		if (args.size() < 2)
			user.send_reply(ERR_NEEDMOREPARAMS(std::string("INVITE")));
		else
		{
			std::string invited = args[0];
			std::string channel_name = args[1];

			if (channel_name[0] != '#') {
				user.send_reply(ERR_NOSUCHCHANNEL(channel_name));
			}
			else
			{
				std::map<std::string, Channel>::iterator it = channels.find(channel_name);
				if (it != channels.end())
				{
					Channel &channel = it->second;
					if (!channel.is_operator(&user)) {
						user.send_reply(ERR_CHANOPRIVSNEEDED(channel_name));
						return;
					}
					
					User *selected_user = find_invited_user(invited);
					
					if (selected_user)
					{
						channel.add_invited(selected_user);
						selected_user->send_reply(RPL_INVITE(user.get_nick_name(), invited, channel_name));
						user.send_reply(RPL_INVITING(user.get_nick_name(), invited, channel_name));
					}
					else {
						user.send_reply(ERR_NOSUCHNICK(invited));
					}
				}
				else {
					user.send_reply(ERR_NOSUCHCHANNEL(channel_name));
				}
			}
		}
	}
	else if (cmd == "KICK")
	{
		if (!check_auth(user))
			return ;
		if (args.size() < 2)
			user.send_reply(ERR_NEEDMOREPARAMS(std::string("KICK")));
		else {
			std::string channel_name = args[0];
			std::string kicked = args[1];
			std::string reason = trailing.empty() ? "Kicked by " + user.get_nick_name() : trailing.substr(1);
	
			if (channel_name[0] != '#')
				user.send_reply(ERR_NOSUCHCHANNEL(channel_name));
			else {
				std::map<std::string, Channel>::iterator it = channels.find(channel_name);
				if (it != channels.end()) {
					std::cout << "Channel found" << std::endl;
					
					Channel &channel = it->second;
					if (!channel.is_operator(&user)) {
						user.send_reply(ERR_CHANOPRIVSNEEDED(channel_name));
						return;
					}
					User *selected_user = find_user_by_nickname(it->second, kicked);
					if (selected_user && selected_user == &user)
						return ;
					if (selected_user) {
						std::string kick_message = ":" + user.get_nick_name() + "!" + user.get_user_name() 
						+ "@FT_IRC KICK " + channel_name + " " + kicked + " :" + reason + "\r\n";
						const std::vector<User *> &channel_users = channel.get_users();
						for (size_t i = 0; i < channel_users.size(); ++i) {
							send(channel_users[i]->get_fd(), kick_message.c_str(), kick_message.size(), 0);
						}
						channel.remove_user(selected_user);
						if (channel.is_operator(selected_user))
							channel.remove_operator(selected_user);
						std::cout << "User " << kicked << " removed from channel " << channel_name << std::endl;
					}
					else
						user.send_reply(ERR_NOSUCHNICK(kicked));
				}
				else
					user.send_reply(ERR_NOSUCHCHANNEL(channel_name));
			}
		}
	}

	else if (cmd == "TOPIC")
	{
		if (!check_auth(user))
			return ;
		if (args.size() < 1)
			user.send_reply(ERR_NEEDMOREPARAMS(std::string("TOPIC")));
		else if (!trailing.empty())
			topic_command(args[0], trailing, user, true);
		else
			topic_command(args[0], args[1], user, false);
		
	}

	else if (cmd == "MODE")
	{
		if (!check_auth(user))
			return ;
		handleModeCommand(&user, args);
	}
	else
		user.send_reply(ERR_UNKNOWNCOMMAND(cmd));
}

void Parser::privmsg(std::string receiver, std::string msg, User &user)
{
	if (receiver[0] == '#')
	{
		std::map<std::string, Channel>::iterator it = channels.find(receiver);
		if (it != channels.end())
		{
			Channel &channel = it->second;
			User *sender = find_user_by_nickname(it->second, user.get_nick_name());
			if (sender)
			{
				const std::vector<User *> &channel_users = channel.get_users();
				std::string formatted_msg = ":" + user.get_nick_name() + "!" + user.get_user_name() + "@" + user.get_ip_address() + " PRIVMSG " + receiver + " :" + msg + "\r\n";
				for (size_t i = 0; i < channel_users.size(); ++i)
				{
					User *target = channel_users[i];
					if (target->get_fd() != user.get_fd())
						send(target->get_fd(), formatted_msg.c_str(), formatted_msg.size(), 0);
				}
			}
			else
				user.send_reply(ERR_NOTONCHANNEL(user.get_nick_name(), channel.get_name()));
		}
		else
			user.send_reply(ERR_NOSUCHCHANNEL(receiver));
	}
	else
	{
		bool found = false;
		for (size_t i = 0; i < users.size(); i++)
		{
			if (users[i]->get_nick_name() == receiver)
			{
				std::string formatted_msg = ":" + user.get_nick_name() + "!" + user.get_user_name() + user.get_ip_address() + " PRIVMSG " + receiver + " :" + msg + "\r\n";
				send(users[i]->get_fd(), formatted_msg.c_str(), formatted_msg.size(), 0);
				found = true;
				break;
			}
		}        
		if (!found)
			user.send_reply(ERR_NOSUCHNICK(receiver));
	}
}

void Parser::send_mode_update(User *user, Channel& channel, char adding, char mode, std::string param, bool check) {
	if (check)
	{
		const std::vector<User *> &current_users = channel.get_users();
		std::string names_list;
		for (size_t j = 0; j < current_users.size(); j++) {
			if (j > 0)
				names_list += " ";
			if (channel.is_operator(current_users[j]))
				names_list += "@";
			names_list += current_users[j]->get_nick_name();
		}
		std::string mode_message = RPL_MODEOPERATOR(user->get_nick_name(), channel.get_name(), adding, param);
		const std::vector<User*>& channel_users = channel.get_users();
		for (size_t i = 0; i < channel_users.size(); i++) {
				channel_users[i]->send_reply(mode_message);
		}
	} else {
		std::string mode_message = RPL_BROADMODE(user->get_nick_name(), channel.get_name(), adding,  mode, param);
		const std::vector<User*>& channel_users = channel.get_users();
		for (size_t i = 0; i < channel_users.size(); i++) {
				channel_users[i]->send_reply(mode_message);
		}
	}
}

void Parser::send_topic_update(User& user, Channel& channel, std::string& channel_name) {
	std::string topic_message = RPL_BROADTOPIC(user.get_nick_name(), channel_name, channel.get_topic());
    const std::vector<User*>& channel_users = channel.get_users();
    for (size_t i = 0; i < channel_users.size(); i++) {
            channel_users[i]->send_reply(topic_message);
    }
}

void Parser::topic_command(std::string channel_name, std::string new_topic, User& user, bool istrail) {
    if (channel_name.empty())
        return;

    std::map<std::string, Channel>::iterator it = channels.find(channel_name);
    if (it != channels.end()) {
        Channel& channel = it->second;
        std::vector<User*>& channel_users = channel.get_users();
        bool user_in_channel = false;

        for (size_t i = 0; i < channel_users.size(); i++) {
            if (channel_users[i]->get_nick_name() == user.get_nick_name()) {
                user_in_channel = true;
                break;
            }
        }
        if (!user_in_channel) {
            user.send_reply(ERR_NOTONCHANNEL(user.get_nick_name(), channel_name));
        }
		else {
            if (!new_topic.empty()) {
                if (channel.has_mode('t') && !channel.is_operator(&user)) {
                    user.send_reply(ERR_CHANOPRIVSNEEDED(channel_name));
                    return;
                }
                if (new_topic == ":") {
                    channel.set_topic("", user.get_nick_name());
                } else if (istrail) {
                    channel.set_topic(new_topic.substr(1), user.get_nick_name());
                } else {
					channel.set_topic(new_topic, user.get_nick_name()); 
				}
                user.send_reply(RPL_TOPIC(user.get_nick_name(), channel_name, channel.get_topic()));
                user.send_reply(RPL_TOPICWHOTIME(user.get_nick_name(), channel_name, channel.get_topic_author(), channel.get_topic_time()));
                send_topic_update(user, channel, channel_name);
            }
			else
			{
				if ((channel.get_topic().empty()))
					user.send_reply(RPL_NOTOPIC(user.get_nick_name(), channel_name));
				else
					user.send_reply(RPL_TOPIC(user.get_nick_name(), channel_name, channel.get_topic()));
			}
        }
    }
	else {
        user.send_reply(ERR_NOSUCHCHANNEL(channel_name));
		return ;
    }
}

User*    Parser::find_user_by_nickname(Channel &channel,const std::string nickname)
{
	const std::vector<User *> &user = channel.get_users();
	for (size_t i = 0; i < user.size(); ++i)
	{
		User *target = user[i];
		if (target->get_nick_name() == nickname)
			return target;
	}
	return NULL;
}

User*	Parser::find_invited_user(const std::string nickname)
{
	for (size_t i = 0; i < users.size(); ++i)
	{
		if (users[i]->get_nick_name() == nickname)
			return users[i];
	}
	return NULL;
}


void Channel::apply_modes(const std::string &mode_string, const std::vector<std::string> &params, Parser &parser, User *user, Channel &channel)
{
	bool adding = true;
	size_t param_index = 0;

	for (size_t i = 0; i < mode_string.length(); ++i)
	{
		char c = mode_string[i];
		if (c == '+')
			adding = true;
		else if (c == '-')
			adding = false;
		else if (c == 'i')
		{
			set_mode(c, adding);
			if (adding){
				is_invite_only = true;
				parser.send_mode_update(user, channel, '+' , c, "", false);
			}
			else{
				is_invite_only = false;
				parser.send_mode_update(user, channel, '-', c, "", false);
			}
		}
		else if (c == 't'){
			set_mode(c, adding);
			char a;
			if (adding)
				a = '+';
			else
				a = '-';
			parser.send_mode_update(user, channel, a, c, "", false);
		}
		else if (c == 'k')
		{
			if (adding)
			{
				if (param_index >= params.size())
					break;
				if (params[param_index].empty())
					user->send_reply(ERR_NOKEYTOSET());
				set_key_mode(params[param_index], true);
				parser.send_mode_update(user, channel, '+', c, "", false);
				param_index++;
			}
			else
			{
				set_key_mode("", false);
				parser.send_mode_update(user, channel, '-', c, "", false);
			}
		}
		else if (c == 'o')
		{
			if (param_index >= params.size())
				break;
			User *target = parser.find_user_by_nickname(*this, params[param_index]);
			if (target && target != user){
				set_operators_mode(adding, target);
				char a;
				if (adding)
					a = '+';
				else
					a = '-';
				parser.send_mode_update(user, channel, a, c, params[param_index], true);
			}
			param_index++;
		}
		else if (c == 'l')
		{
			if (adding)
			{
				if (param_index >= params.size()) break;
		
				unsigned long limit = strtoul(params[param_index].c_str(), NULL, 10);
				if (params[param_index][0] == '-')
					limit = 0;
				set_user_limits(true, limit);
				if (limit > 0 && limit <= INT_MAX)
					parser.send_mode_update(user, channel, '+', c, params[param_index], false);
				param_index++;
			}
			else
			{
				set_user_limits(false, 0);
				parser.send_mode_update(user, channel, '-', c, "", false);
			}
		}
		else
		{
			user->send_reply(RPL_UMODEIS(user->get_nick_name(), channel.get_modes()));
		}
		
	}
}



void Parser::handleModeCommand(User* user, std::vector<std::string>& args)
{
	if (args.size() < 1) {
		user->send_reply(ERR_NEEDMOREPARAMS(std::string("MODE")));
		return;
	}
	const std::string& target = args[0];
	
	if (target.empty() || target[0] != '#') {
		user->send_reply(ERR_NOSUCHCHANNEL(target));
		return;
	}

	std::string channel_name = target;
	std::map<std::string, Channel>::iterator it = channels.find(target);
	if (it == channels.end()){
		user->send_reply(ERR_NOSUCHCHANNEL(target));
		return ;
	}
	Channel &channel = it->second;

	if (args.size() == 1) {
        user->send_reply(RPL_CHANNELMODEIS(channel_name, channel_name, channel.get_modes()));
		user->send_reply(RPL_CREATED(channel_name, server->get_creation_time()));
        return;
    }

	std::string mode_string = args[1];
	if (!channel.is_operator(user)){
		user->send_reply(ERR_CHANOPRIVSNEEDED(target));
		return;
	}
	std::vector<std::string> mode_args;
	for (size_t i = 2; i < args.size(); ++i){
		mode_args.push_back(args[i]);
	}

	channel.apply_modes(mode_string, mode_args, *this, user, channel);
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

