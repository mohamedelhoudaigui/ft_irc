#include "../headers/replies.hpp"

std::string RPL_WELCOME(std::string nickname, std::string username)
{
	std::stringstream ss;
	ss << PREFIX << "001 " << nickname << " :Welcome to the Internet Relay Network " << nickname << "!" << username << "@localhost" POSTFIX;
	return ss.str();
}

std::string RPL_YOURHOST(const std::string &servername, const std::string &nickname)
{
	std::stringstream ss;
	ss << PREFIX << servername << " 002 " << nickname << " :Your host is " << servername << ", running version 1.0" POSTFIX;
	return ss.str();
}

std::string RPL_CREATED(const std::string &servername, const std::string &nickname)
{
	std::stringstream ss;
	time_t tmp = time(NULL);
	struct tm *tv = localtime(&tmp);
	char buffer[80];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tv);
	ss << PREFIX << servername << " 003 " << nickname << " :This server was created " << buffer << POSTFIX;
	return ss.str();
}

std::string RPL_MYINFO(const std::string &servername, const std::string &nickname)
{
	std::stringstream ss;

	std::string userMods = "iwso";
	std::string chanMods = "mtov";
	ss << PREFIX << servername << " 004 " << nickname << " " << servername << " 1.0 " << userMods << " " << chanMods << POSTFIX;
	return ss.str();
}

std::string RPL_ISUPPORT(const std::string &servername)
{
	std::stringstream ss;
	
	ss << PREFIX << servername << " 005 * " << "CHANTYPES=#& " << "PREFIX=(OV)@+ " << "MODES=4 " << "CHANLIMIT=#&:2 " << "NICKLEN=9 " << "TOPICLEN=390 " << "KICKLEN=390 "
	<< "NETWORK=MyNetwork " << "CASEMAPPING=ascii " << "CHARSET=utf-8 " << "CHANNELLEN=59 " << "SAFELIST " << ":are supported by this server" << POSTFIX;

	return ss.str();
}