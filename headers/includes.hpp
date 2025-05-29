#pragma once

#include <map>
#include <vector>
#include <poll.h>
#include <cstdio>
#include <fcntl.h>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <limits.h>
#include <iostream>
#include <signal.h>
#include <algorithm>
#include <arpa/inet.h>

#define POSTFIX "\r\n"
#define MAX_PORT 65535
#define MAX_EVENTS 500
#define BUFFER_SIZE 512
