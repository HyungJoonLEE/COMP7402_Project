#include "ServerHelper.h"

void error_exit(const char *msg) {
    error(EXIT_FAILURE, errno, "%s\n", msg);
}


int set_nonblock(int fd) {
#if defined(O_NONBLOCK)
    int flags;
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    int flags = 1;
    return ioctl(fd, FIOBIO, &flags);
#endif
}


void configure_server_socket(int serverSocket) {
    // bind the socket to the address
    struct sockaddr_in socketAddress;
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(PORT);
    socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(serverSocket, (struct sockaddr *)(&socketAddress), sizeof(socketAddress)) == -1)
        error_exit("error in bind()");

    // unblock the master socket
    if (set_nonblock(serverSocket) == -1)
        error_exit("cannot unblock the master socket");

    // listen sockets
    if (listen(serverSocket, SOMAXCONN) == -1)
        error_exit("cannot listen");
}


void register_in_epoll(int sock, int epollId) {
    struct epoll_event event;
    event.data.fd = sock;
    event.events = EPOLLIN;
    if (epoll_ctl(epollId, EPOLL_CTL_ADD, sock, &event) == -1)
        error_exit("cannot register in epoll");
}


void register_new_client(int serverSocket, int epollId) {
    socklen_t clientAddrSize = sizeof(struct sockaddr_in);
    struct sockaddr_in socketAddress;
    int clientSocket = accept(serverSocket, (struct sockaddr *) &socketAddress, &clientAddrSize);
    if (clientSocket == -1)
        error_exit("cannot accept new client");

    if (set_nonblock(clientSocket))
        error_exit("cannot unblock");

    activeUsers.insert(clientSocket);
    char *ipAddress = inet_ntoa(socketAddress.sin_addr);
    ipAddresses.insert(make_pair(clientSocket, ipAddress));
    register_in_epoll(clientSocket, epollId);
}


void serve_client(int clientSocket) {
    array<char, BUFFERSIZE> buffer = {};
    int recvSize = recv(clientSocket, buffer.data(), BUFFERSIZE, MSG_NOSIGNAL);
    if (recvSize <= 0 && errno != EAGAIN) {
        shutdown(clientSocket, SHUT_RDWR);
        close(clientSocket);
        activeUsers.erase(clientSocket);
    } else if (recvSize > 0) {
        string message = ipAddresses.find(clientSocket)->second + ": " + buffer.data();
        messages.push_back(message);
    }
}


void send_messages() {
    string totalMessage = "";
    for (string &message : messages)
        totalMessage += message;
    size_t messageLength = totalMessage.size() + 1;

    for (int sock : activeUsers) {
        send(sock, totalMessage.c_str(), messageLength, MSG_NOSIGNAL);
    }
    messages.clear();
}