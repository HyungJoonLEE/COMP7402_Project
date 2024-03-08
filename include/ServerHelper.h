#ifndef COMP7402_PROJECT_SERVERHELPER_H
#define COMP7402_PROJECT_SERVERHELPER_H

#include <array>
#include <map>
#include <set>
#include <vector>
#include <string>

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

using namespace std;

static const int PORT = 12345;
static const int MAXEVENTS = 32;
static const int BUFFERSIZE = 1024;
static set<int> activeUsers;
static vector<string> messages;
static map<int, string> ipAddresses;

class ServerHelper {
private:
public:
    void error_exit(const char *msg);

    int set_nonblock(int fd);

    void configure_server_socket(int serverSocket);

    void register_in_epoll(int sock, int epollId);

    void register_new_client(int serverSocket, int epollId);

    void serve_client(int clientSocket);

    void send_messages();

};


#endif //COMP7402_PROJECT_SERVERHELPER_H
