#pragma once

#include <iostream>

#define LOG_ERROR   std::cout << "\033[1m\033[31m"  // bold red
#define LOG_WARNING std::cout << "\033[1m\033[33m"  // bold yellow
#define LOG_INFO    std::cout << "\033[1m\033[32m"  // bold green
#define LOG_INFOL   std::cout << "\033[32m"         // green
#define LOG_DEBUG   std::cout << "\033[1m\033[36m"  // bold cyan
#define LOG_REND    "\033[0m" << std::endl

#define LOG_TITLE   std::cout << "\033[1m\033[35m"  // bold magenta
#define LOG_MSG     std::cout << "\033[1m\033[37m"  // bold white
