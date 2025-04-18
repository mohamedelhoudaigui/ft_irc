#pragma once
#include <map>
#include <string>
#include <iostream>
#include <vector>

class User;

class Channel{
    std::string name;
    std::vector<User *> users;

    //Needed for topic command
    std::string topic;
    std::string topic_author;
    time_t topic_time_change;

    public:
    Channel();
    ~Channel();
    Channel(const Channel &c);
    Channel operator=(const Channel &c);


    std::string get_name();
    const std::vector<User *>& get_users() const;
    void    set_topic(const std::string &name, const std::string &author);
    std::string get_topic() const;
};