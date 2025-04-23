#include "../headers/channels.hpp"
#include "../headers/parser.hpp"

Channel::Channel(){

}

Channel::~Channel(){

}

Channel::Channel(const Channel &c){
    this->name = c.name; 
}

Channel Channel::operator=(const Channel &c){
    if (this != &c)
    {
        this->name = c.name;
        this->users = c.users;
    }
    return *this;
}

std::string Channel::get_name(){
    return name;
}

std::vector<User *>& Channel::get_users() {
    return users;
}

void    Channel::set_topic(const std::string &name, const std::string &author){
    this->name = name;
    this->topic_author = author;
    this->topic_time_change = time(NULL);
}

std::string Channel::get_topic() const{
    return this->name;
}

void Channel::add_user(User* user) {
    users.push_back(user);
}

bool Channel::is_operator(User *user)
{
    return std::find(operators.begin(), operators.end(), user) != operators.end();
}

std::string Channel::get_modes() const{
    return modes;
}

std::string Channel::get_key() const{
    return key;
}

unsigned long Channel::get_user_limit(){
    return user_limts;
}


//mode command


bool Channel::has_mode(std::string mode){
    return (modes.find(mode) != std::string::npos);
}

void Channel::set_mode(std::string mode, bool add){
    if (add && !has_mode(mode))
        modes += mode;
    else if (!add){
        std::string::size_type pos = modes.find(mode[0]);
        if (pos != std::string::npos)
            modes.erase(pos, 1);
    }
}

void Channel::set_key_mode(std::string key, bool add){
    if (add)
    {
        this->key = key;
        if (!has_mode(std::string(1,'k')))
            modes += 'k';
    }
    else {
        this->key = "";
        std::string::size_type pos = modes.find('k');
        if (pos != std::string::npos)
            modes.erase(pos, 1);
    }
}

void Channel::set_user_limits(bool add, unsigned long limit){
    if (add){
        this->user_limts = limit;
        if (!has_mode(std::string(1,'l')))
            modes += 'l';
    }
    else{
        this->user_limts = 0;
        std::string::size_type pos = modes.find('l');
        if (pos != std::string::npos)
            modes.erase(pos, 1);
    }
}

void Channel::set_operators_mode(bool add, User *user){
    if (add){
        if (std::find(operators.begin(), operators.end(), user) == operators.end())
            operators.push_back(user);
    }
    else{
        std::vector<User*>::iterator it = std::find(operators.begin(), operators.end(), user);
        if (it != operators.end())
            operators.erase(it);
    }
}


void Channel::remove_operator(User *user)
{
    operators.erase(std::remove(operators.begin(), operators.end(), user), operators.end());
}

std::string Channel::get_topic_author() const{
    return topic_author;
}

time_t Channel::get_topic_time() const{
    return topic_time_change;
}