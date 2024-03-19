#ifndef COMP7402_PROJECT_SERVERHELPER_H
#define COMP7402_PROJECT_SERVERHELPER_H

#include <array>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <error.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <functional>
#include "User.h"
#include "brainpool.h"
#include "common.h"
#include "key.h"


using namespace std;

void error_exit(const char *msg);

int set_nonblock(int fd);

void configure_server_socket(int serverSocket);

void register_in_epoll(int sock, int epollId);

void register_new_client(int serverSocket, int epollId, array<User, 10>& users);

void serve_client(int clientSocket, array<User, 10>& users);

void send_messages(array<User, 10>& users);

void* handleClient(void* socket);

void check_and_print(const array<char, BUFFERSIZE>& buffer, const User& user);

bool contains_word(const array<char, BUFFERSIZE>& buffer, const string& word);

string extract_str_after_marker(const string& input, const string& marker);

void thread_create_file(User &u);

#endif //COMP7402_PROJECT_SERVERHELPER_H
