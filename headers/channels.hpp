#pragma once
#include <map>
#include <string>
#include <iostream>
#include <vector>

class User;

class Channel{
    std::string name;
    std::vector<User *> users;

    public:
    Channel();
    ~Channel();
    Channel(const Channel &c);
    Channel operator=(const Channel &c);


    std::string get_name();
    std::vector<User *>& get_users();
    void add_user(User* user);
    
};
