#include "ServerHelper.h"
#include "Feistel.h"


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
    int recvSize = (int) read(clientSocket, buffer.data(), BUFFERSIZE);
    if (recvSize <= 0 && errno != EAGAIN) {
        shutdown(clientSocket, SHUT_RDWR);
        close(clientSocket);
    }
    else if (recvSize > 0) {
        if (users[clientSocket].is_key_flag() &&
            users[clientSocket].is_file_name_flag() &&
            users[clientSocket].is_file_size_flag()) {
            // TODO: EOT received
            string bin_data(buffer.data(), recvSize);
            if (!strstr(bin_data.c_str(), "EOT")) {
                Feistel f;
                f.CBC_decrypt(users[clientSocket], bin_data);
                write(users[clientSocket].get_fd(), "ACK", 4);
            }
            else {
                users[clientSocket].set_EOC_flag(true);
                return;
            }
        }
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
                string ssk = hexToASCII(set_secret(users[clientSocket].get_server_private_key(),
                                                   point,
                                                   shared_secret_key, &secret_len));
                string ssk_cut = ssk.substr(0, 16);
                cout << "Shared secret key :" << strToHex(ssk_cut) << " [ " << users[clientSocket].get_ip() << " ]" << endl;
                users[clientSocket].set_shared_secret_key(strToBin(ssk_cut));
            }
            users[clientSocket].set_key_flag(true);
            string message_pub = "[public_key] " + users[clientSocket].get_server_hex_pup();
            write(clientSocket, message_pub.c_str(), message_pub.size());

            Key k;
            k.generateRoundKeys(users[clientSocket].get_shared_key());
            k.generateReverseRoundKeys(k.getRK());
            users[clientSocket].set_reverse_round_keys(k.getRRK());
        }


        if (!users[clientSocket].is_file_name_flag() || !users[clientSocket].is_file_size_flag()) {
            // TODO: get IV
            if (contains_word(buffer, "[iv]")) {
                string iv(buffer.data(), recvSize);
                users[clientSocket].set_iv(extract_str_after_marker(iv, "[iv]"));
//                cout << "IV: " << users[clientSocket].get_iv() << endl;
                buffer.fill(0);
                write(users[clientSocket].get_fd(), "check", 6);
            }
            // TODO: get file name
            if (contains_word(buffer, "[file_name]")) {
                string file_name(buffer.data(), recvSize);
                string fn = extract_str_after_marker(file_name, "[file_name]");
                users[clientSocket].set_file_name(users[clientSocket].get_ip() + "_" + fn);
//                cout << "File name: " << users[clientSocket].get_file_name() << endl;
                buffer.fill(0);
                users[clientSocket].set_file_name_flag(true);
                write(users[clientSocket].get_fd(), "check", 6);
            }
//            if (contains_word(buffer, "[header]")) {
//                string file_hdr(buffer.data(), recvSize);
//                string hdr = extract_str_after_marker(file_hdr, "[header]");
//                buffer.fill(0);
//                users[clientSocket].set_file_header(hdr);
//                write(users[clientSocket].get_fd(), "check", 6);
//            }
            // TODO: get file size
            if (contains_word(buffer, "[file_size]")) {
                string file_size(buffer.data(), recvSize);
                unsigned long fs = stol(extract_str_after_marker(file_size, "[file_size]"));
                users[clientSocket].set_file_size(fs);
//                cout << "File size: " << users[clientSocket].get_file_size() << endl;
                buffer.fill(0);
                users[clientSocket].set_file_size_flag(true);
                write(users[clientSocket].get_fd(), "check", 6);
            }
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


bool contains_word(const array<char, BUFFERSIZE>& buffer, const string& word) {
    string bufferStr(buffer.data(), BUFFERSIZE);
    return bufferStr.find(word) != string::npos;
}


string extract_str_after_marker(const string& input, const string& marker) {
    size_t startPos = input.find(marker);
    if (startPos != string::npos) {
        startPos += marker.length() + 1;
    }

    string ext = input.substr(startPos);
    return ext;
}