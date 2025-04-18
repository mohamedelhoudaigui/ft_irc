#include "../headers/channels.hpp"

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
    }
    return *this;
}

std::string Channel::get_name(){
    return name;
}

const std::vector<User *>& Channel::get_users() const{
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

