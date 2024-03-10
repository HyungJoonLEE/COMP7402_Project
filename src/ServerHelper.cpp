#include "ServerHelper.h"


void configure_server_socket(int serverSocket) {
    // bind the socket to the address
    struct sockaddr_in socketAddress;
    int option = true;
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(PORT);
    socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
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


void register_new_client(int serverSocket, int epollId, array<User, 10>& users) {
    User u;
    struct sockaddr_in socketAddress;
    socklen_t clientAddrSize = sizeof(struct sockaddr_in);
    EC_KEY* priv_key = nullptr;
    const EC_POINT * pub_key = nullptr;

    int clientSocket = accept(serverSocket, (struct sockaddr *) &socketAddress, &clientAddrSize);
    if (clientSocket == -1)
        error_exit("cannot accept new client");

    if (set_nonblock(clientSocket))
        error_exit("cannot unblock");

    u.set_ip(inet_ntoa(socketAddress.sin_addr));
    u.set_fd(clientSocket);
    string ip = inet_ntoa(socketAddress.sin_addr);
    cout << "Client IP: " << ip << " (fd = " << clientSocket << ") " << "connected" << endl;

    register_in_epoll(clientSocket, epollId);
    users[clientSocket] = u;

    // Server generates key
    create_private_key(priv_key);
    users[clientSocket].set_server_hex_pub(create_public_key(pub_key, priv_key));
    users[clientSocket].set_server_private_key(priv_key);
    users[clientSocket].set_server_public_key(const_cast<EC_POINT *>(pub_key));
}


void serve_client(int clientSocket, array<User, 10>& users) {
    array<char, BUFFERSIZE> buffer = {};
    int recvSize = recv(clientSocket, buffer.data(), BUFFERSIZE, MSG_NOSIGNAL);
    if (recvSize <= 0 && errno != EAGAIN) {
        shutdown(clientSocket, SHUT_RDWR);
        close(clientSocket);
    }
    else if (recvSize > 0) {
        cout << users[clientSocket].get_ip() + " : " + buffer.data() << endl;

        // TODO: pub key exchange
        if (!users[clientSocket].is_key_flag()) {
            string buffer_str(buffer.data());
            string prefix = "[public_key]";
            size_t startPos = buffer_str.find(prefix);
            if (startPos != string::npos) {
                startPos += prefix.length() + 1;
                string public_key = buffer_str.substr(startPos);
                users[clientSocket].set_client_hex_pub(public_key);

                unsigned char* shared_secret_key = nullptr;
                size_t secret_len;
                const EC_GROUP* group = EC_GROUP_new_by_curve_name(NID_brainpoolP256r1);
                EC_POINT* point = hexStringToEcPoint(group, public_key);
                string ssk = set_secret(users[clientSocket].get_server_private_key(),
                                        point,
                                        shared_secret_key, &secret_len);
                users[clientSocket].set_shared_secret_key(ssk);
                cout << "Shared secret key [" << users[clientSocket].get_ip() << "]: " << ssk << endl;
            }
            users[clientSocket].set_key_flag(true);
            string message_pub = "[public_key] " + users[clientSocket].get_server_hex_pup();
            write(clientSocket, message_pub.c_str(), message_pub.size());
        }
    }
}


void send_messages(array<User, 10>& users) {
    string totalMessage = "hello";
    size_t messageLength = totalMessage.size() + 1;

    for (const auto& user : users) {
        send(user.get_fd(), totalMessage.c_str(), messageLength, MSG_NOSIGNAL);
    }
    totalMessage.clear();
}



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
