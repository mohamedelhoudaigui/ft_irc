#pragma once

#include "includes.hpp"
#include "utils.hpp"

//Errors :
#define ERR_NOSUCHNICK(nick) ": 401 " + nick + " :No such nick/channel" POSTFIX 
#define ERR_NOSUCHCHANNEL(channel) ": 403 " + channel + " :No such channel" POSTFIX 
#define ERR_NORECIPIENT(nickname, command) ": 411 " + nickname + " :No recipient given (" + command + ")" + POSTFIX
#define ERR_NOTEXTTOSEND() ": 412 :No text to send" POSTFIX 
#define ERR_UNKNOWNCOMMAND(command) ": 421 " + command + " :Unknown command" POSTFIX 
#define ERR_NONICKNAMEGIVEN(target) ": 431 " + target + " :No nickname given" POSTFIX 
#define ERR_ERRONEUSNICKNAME(nick) ": 432 " + nick + " :Erroneus nickname" POSTFIX 
#define ERR_NICKNAMEINUSE(target) ": 433 " + target + " :Nickname is already in use" POSTFIX 
#define ERR_NOTONCHANNEL(target, channel) ": 442 " + target + " " + channel + " :You're not on that channel" POSTFIX 
#define ERR_NOAUTH(target) ": 451 " + target + ":Not authenticated" POSTFIX
#define ERR_NEEDMOREPARAMS(command) ": 461 " + command + " :Not enough parameters" + POSTFIX
#define ERR_ALREADYREGISTERED(target) ": 462 " + target + " :You may not reregister" POSTFIX 
#define ERR_PASSWDMISMATCH(target) ": 464 " + target + " :Password incorrect" POSTFIX 
#define ERR_CHANOPRIVSNEEDED(channel) ": 482 " + channel + " :You're not channel operator" POSTFIX
#define ERR_NOKEYTOSET() ": No key to set" POSTFIX
#define ERR_CHANNELISFULL(client, channel) ": 471 " + client + " " + channel + " :Cannot join channel (+l)" POSTFIX
#define ERR_INVITEONLYCHAN(client, channel) ": 473 " + client + " " + channel + " :Cannot join channel (+i)" POSTFIX
#define ERR_BADCHANNELKEY(nickname, channel)": 475 " + nickname + " " + channel + " :Cannot join channel (+k)" + POSTFIX

//Replies :
#define RPL_PONG(server_name, token) "PONG " + server_name + token + POSTFIX
#define RPL_WELCOME(sender, msg) ": 001 " + sender + " : " + msg + POSTFIX
#define RPL_YOURHOST(nick) ": 002 " + nick + " :Your host is FT_IRC running version 1.0" + POSTFIX
#define RPL_CREATED(nick, datetime) ": 003 " + nick + " :This server was created " + datetime + POSTFIX
#define RPL_UMODEIS(nickname, modes) ": 221 " + nickname + " :" + modes + POSTFIX
#define RPL_NOTOPIC(sender, channel) ": 331 " + sender + " " + channel + " :No topic is set" + POSTFIX
#define RPL_TOPIC(sender, channel, topic) ": 332 " + sender + " " + channel + " :" + topic + POSTFIX    
#define RPL_TOPICWHOTIME(nick, channel, setter, time)  ": 333 " + nick + " " + channel + " " + setter + " " + to_string(time) + POSTFIX
#define RPL_INVITING(nickname, targnick, targchan) ": 341 " + nickname + " " + targnick + " " + targchan + POSTFIX
#define RPL_NAMREPLY(sender, channel, users) ": 353 " + sender + " = " + channel + " :" + users + POSTFIX
#define RPL_ENDOFNAMES(sender, channel) ": 366 " + sender + " " + channel + " :End of /NAMES list." POSTFIX
#define RPL_NICK(sender, nick) ":" + sender + " NICK " + nick + POSTFIX
#define RPL_INVITE(sender, target, channel) ":" + sender + " INVITE " + target + " " + channel + POSTFIX
#define RPL_JOINMSG(nickname, username, channel) ":" + nickname + "!" + username + "@FT_IRC JOIN " + channel + POSTFIX
#define RPL_JOIN(sender, channel) ":" + sender + " JOIN :" + channel + POSTFIX
#define RPL_CHANNELMODEIS(hostname, channel, mode) ":" + hostname + " MODE " + channel + " " + mode + POSTFIX
#define RPL_BROADTOPIC(nickname, channel, topic) ":" + nickname + " TOPIC " + channel + " :" + topic + POSTFIX
#define RPL_BROADMODE(nickname, channel, adding, mode, param) ":" + nickname + " MODE " + channel + " :" + adding + mode + " " +  param + POSTFIX
#define RPL_NICKCHANGE(old_nick, user_name, new_nick) ":" + old_nick + "!" + user_name + "@FT_IRC NICK :" + new_nick + POSTFIX
#define RPL_MODEOPERATOR(nickname, channel, adding, param) ":" + nickname + "!" + nickname + "@" + nickname + " MODE " + channel + " " + adding + "o" + " " +  param + POSTFIX