#pragma once
#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <ctime>

class User;
class Parser;
class Channel{
    std::string name;
    std::vector<User *> users;

    //Needed for topic command
    std::string topic;
    std::string topic_author;
    time_t topic_time_change;


    //Needed fo mode command
    std::vector<User *> invited;
    std::string modes;
    std::string key;
    unsigned long user_limits;
    std::vector<User *> operators;
    
    std::string creationTime;
    bool is_invite_only;

    public:
        Channel();
        ~Channel();
        Channel(const Channel &c);
        Channel operator=(const Channel &c);

        std::string get_name();
        std::vector<User *>& get_users();
        void    set_topic(const std::string &name, const std::string &author);
        std::string get_topic() const;
        void add_user(User* user);
        std::string get_topic_author() const;
        time_t get_topic_time() const;
        void remove_user(User* user);

        std::string get_modes() const;
        std::string get_key() const;
        unsigned long get_user_limit();

        bool is_operator(User *user);

        bool is_invited(User* user);
        void add_invited(User* user);
        bool get_channel_status(void) const;

        bool has_mode(char mode);
        void set_mode(char mode, bool add);
        void set_key_mode(std::string key, bool add);
        void set_user_limits(bool add, unsigned long limit);
        void set_operators_mode(bool add, User *user);
        void remove_operator(User *user);
        void apply_modes(const std::string &mode_string, const std::vector<std::string> &params, Parser &parser);
};

