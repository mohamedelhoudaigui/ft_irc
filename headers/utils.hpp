#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <ctime>

#define MAX_PORT 65535

int         parse_port(std::string port);
std::string int_to_string(int n);
std::string convert_to_lower(std::string s);
bool        valid_nick_name(std::string s);
std::string	get_now_time();

#endif