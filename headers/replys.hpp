#ifndef REPLYS_HPP
#define REPLYS_HPP

#include "utils.hpp"

#define PREFIX ":IRC "
#define POSTFIX "\r\n"

//Errors :

#define ERR_ERRONEUSNICKNAME(nick)PREFIX "432 " + nick + " :Erroneus nickname" POSTFIX 
#define ERR_NICKNAMEINUSE(target)PREFIX "433 " + target + " :Nickname is already in use" POSTFIX 
#define ERR_NONICKNAMEGIVEN(target)PREFIX "431 " + target + " :No nickname given" POSTFIX 
#define ERR_NEEDMOREPARAMS(command) (std::string(PREFIX) + "461 " + command + " :Not enough parameters" + POSTFIX) 
#define ERR_PASSWDMISMATCH(target)PREFIX "464 " + target + " :Password incorrect" POSTFIX 
#define ERR_NOAUTH(target)PREFIX "464 " + target + ":Not authenticated" POSTFIX
#define ERR_ALREADYREGISTERED(target)PREFIX "462 " + target + " :You may not reregister" POSTFIX 
#define ERR_UNKNOWNCOMMAND(command)PREFIX "421 " + command + " :Unknown command" POSTFIX 
#define ERR_CHANOPRIVSNEEDED(channel)PREFIX "482 " + channel + " :You're not channel operator" POSTFIX
#define ERR_NOTEXTTOSEND()PREFIX "412 :No text to send" POSTFIX 
#define ERR_NOSUCHCHANNEL(channel)PREFIX "403 " + channel + " :No such channel" POSTFIX 
#define ERR_NOSUCHNICK(nick)PREFIX "401 " + nick + " :No such nick/channel" POSTFIX 
#define ERR_NOTONCHANNEL(target, channel)   PREFIX "442 " + target + " " + channel + " :You're not on that channel" POSTFIX 
#define ERR_USERNOTINCHANNEL(user, channel) PREFIX "441 " + user + " " + channel + " :They aren't on that channel" POSTFIX 
#define ERR_ALREADYREGISTRED(target)PREFIX "462 " + target + ":You may not reregister" POSTFIX 
#define ERR_CHANNELISFULL(client, channel)PREFIX " 471 " + client + " " + channel + " :Cannot join channel (+l)" POSTFIX
#define ERR_INVITEONLYCHAN(client, channel)PREFIX " 473 " + client + " " + channel + " :Cannot join channel (+i)" POSTFIX
#define ERR_NEEDMODEPARM(channel, mode)(std::string(": 696 ") + channel + " * You must specify a parameter for the key mode. " + mode + POSTFIX)
#define ERR_INVALIDMODEPARM(channel, mode)(std::string(": 696 ") + channel + " Invalid mode parameter. " + mode + POSTFIX)
#define ERR_UNKNOWNMODE(nickname, channel, mode)(std::string(": 472 ") + nickname + " " + channel + " " + mode + " :is not a recognized channel mode" + POSTFIX)
#define ERR_INCORPASS(nickname)(": 464 " + nickname + " :Password incorrect !" + POSTFIX )
#define ERR_NOTREGISTERED(nickname)(": 464 " + nickname + " :You are not registered !" + POSTFIX )
#define ERR_BADCHANNELKEY(nickname, channel)(": 475 " + nickname + " " + channel + " :Cannot join channel (+k)" + POSTFIX)
#define ERR_NORECIPIENT(nickname, command)(": 411 " + nickname + " :No recipient given (" + command + ")" + POSTFIX)
//Replies :

#define RPL_WELCOME(sender, msg) PREFIX "001 " + sender + " : " + msg + POSTFIX
#define RPL_YOURHOST(nick) PREFIX "002 " + nick + " :Your host is FT_IRC running version 1.0" + POSTFIX
#define RPL_CREATED(nick, datetime) PREFIX "003 " + nick + " :This server was created " + datetime + POSTFIX
#define RPL_NAMREPLY(sender, channel, users)    PREFIX "353 " + sender + " = " + channel + " :" + users + POSTFIX
#define RPL_ENDOFNAMES(sender, channel)        PREFIX "366 " + sender + " " + channel + " :End of /NAMES list." POSTFIX
#define RPL_TOPIC(sender, channel, topic)PREFIX " 332 " + sender + " " + channel + " :" + topic + POSTFIX    
#define RPL_PRIVMSG(sender, target, msg)":" + sender + " PRIVMSG " + target + " :" + msg + POSTFIX 
#define RPL_NICK(sender, nick)":" + sender + " NICK " + nick + POSTFIX
#define RPL_NOTOPIC(sender, channel)PREFIX " 331 " + sender + " " + channel + " :No topic is set" + POSTFIX
#define RPL_INVITING(nickname, targnick, targchan)  ": 341 " + nickname + " " + targnick + " " + targchan + POSTFIX
#define RPL_INVITE(sender, target, channel)":" + sender + " INVITE " + target + " " + channel + POSTFIX
#define RPL_JOINMSG(nickname, username, channel)(":" + nickname + "!" + username + "@localhost JOIN " + channel + POSTFIX)
#define RPL_JOIN(sender, channel)":" + sender + " JOIN :" + channel + POSTFIX
#define RPL_CHANNELMODEIS(hostname, channel, mode)(":" + hostname + " MODE " + channel + " " + mode + POSTFIX)
#define RPL_UMODEIS(nickname, modes) PREFIX "221 " + nickname + " :" + modes + POSTFIX
#define RPL_PONG(server_name, token) "PONG " + server_name + token + POSTFIX
#define RPL_KICK(sender, target, channel)":" + sender + " KICK " + target + " " + channel + POSTFIX
#define RPL_BROADTOPIC(nickname, channel, topic)( ":" + nickname + " TOPIC " + channel + " :" + topic + POSTFIX)
#define RPL_TOPICWHOTIME(nick, channel, setter, time) PREFIX "333 " + std::string(nick) + " " + std::string(channel) + " " + std::string(setter) + " " + to_string(time) + POSTFIX

#endif