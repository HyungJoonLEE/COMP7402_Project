#include "ServerHelper.h"

void close_client_socket(int epoll_fd, int client_socket);

int main() {
    array<User, 10> uv;
    array<thread, 10> threads;

    int serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket == -1)
        error_exit("error in socket()");
    configure_server_socket(serverSocket);

    // create epoll id
    int epollId = epoll_create1(0);
    if (epollId == -1)
        error_exit("error in epoll_create1()");

    // register server in epoll struct
    register_in_epoll(serverSocket, epollId);

    // go to infinite cycle
    while (true) {
        struct epoll_event currentEvents[MAXEVENTS];
        int nEvents = epoll_wait(epollId, currentEvents, MAXEVENTS, -1);

        for (int i = 0; i < nEvents; ++i) {
            int sock = currentEvents[i].data.fd;

            if (sock == serverSocket) {
                // Accept client
                register_new_client(serverSocket, epollId, uv);
            }
            else {
                serve_client(sock, uv);
            }
        }
    }
}

void close_client_socket(int epoll_fd, int client_socket) {
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_socket, nullptr) == -1) {
        perror("epoll_ctl: EPOLL_CTL_DEL");
    }

    // Close the socket
    if (close(client_socket) == -1) {
        perror("close");
    }
}

