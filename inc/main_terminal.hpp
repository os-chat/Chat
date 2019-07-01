#ifndef MAIN_TERMINAL_HPP
#define MAIN_TERMINAL_HPP

#include "common.hpp"
#include "receive_msg.hpp"
#include "send_msg.hpp"
#include "cmd_list.hpp"

void main_terminal(const string user_name, int opcao);
void individual(string user_name);
void group(string user_name);

#endif
