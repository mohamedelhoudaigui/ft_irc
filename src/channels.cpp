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

void Channel::remove_user(User* user) {
    users.erase(std::remove(users.begin(), users.end(), user), users.end());
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
    return user_limits;
}

bool Channel::is_invited(User* user) {
    return std::find(invited.begin(), invited.end(), user) != invited.end();
}

void Channel::add_invited(User* user) {
    invited.push_back(user);
}

bool Channel::get_channel_status(void) const{
    return is_invite_only;
}

//mode command


bool Channel::has_mode(char mode){
    return (modes.find(mode) != std::string::npos);
}

void Channel::set_mode(char mode, bool add){
    if (add && !has_mode(mode))
        modes += mode;
    else if (!add)
        modes.erase(std::remove(modes.begin(), modes.end(), mode), modes.end());
}

void Channel::set_key_mode(std::string key, bool add) {
    if (add) {
        this->key = key;
        set_mode('k', true);
    } else {
        this->key.clear();
        set_mode('k', false);
    }
}


void Channel::set_user_limits(bool add, unsigned long limit) {
    if (add) {
        user_limits = limit;
        set_mode('l', true);
    } else {
        user_limits = 0;
        set_mode('l', false);
    }
}


void Channel::set_operators_mode(bool add, User *user) {
    if (add) {
        if (!is_operator(user))
            operators.push_back(user);
        set_mode('o', true);
    } else {
        remove_operator(user);
        if (operators.empty())
            set_mode('o', false);
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