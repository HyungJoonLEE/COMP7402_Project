#include "ServerHelper.h"


int main() {
    array<User, 10> uv;

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
                // client socket is ready to something
                serve_client(sock, uv);
            }
        }
//        send_messages(uv);
    }

    // stop the server
    if (shutdown(serverSocket, SHUT_RDWR) == -1)
        error_exit("error in shutdown()");

    if (close(serverSocket) == -1)
        error_exit("error in close()");

    return EXIT_SUCCESS;
}


