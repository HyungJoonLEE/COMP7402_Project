#include "Client.h"
#include "common.h"
#include "brainpool.h"

#define BUFFER_SIZE 1024

//Client side
int main(int argc, char *argv[]) {
    int fd;
    struct sockaddr_in sockaddr;
    int socklen;
    array<char, BUFFER_SIZE> buffer = {};
    const EC_GROUP* group = EC_GROUP_new_by_curve_name(NID_brainpoolP256r1);
    EC_KEY* priv_key = nullptr;
    const EC_POINT * pub_key = nullptr;
    unsigned char * shared_secret_key = nullptr;
    size_t secret_len;

    if((fd = socket(AF_INET,SOCK_STREAM, 0)) < 0) {
        return 1;
    }
    Client client;
    client.parse_arguments(argc, argv);

    create_private_key(priv_key);
    string pub_str = create_public_key(pub_key, priv_key);

    client.print();

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr(client.get_ip().c_str());
    sockaddr.sin_port = htons(client.get_port());
    string ip = inet_ntoa(sockaddr.sin_addr);

    socklen = sizeof(sockaddr);
    if (connect(fd,(struct sockaddr*)&sockaddr,socklen) == -1){
        perror("connect error");
        return 1;
    }


    // TODO: key exchange
    // send public key
    string message_pubkey = "[public_key] " + pub_str;
    write(fd, message_pubkey.c_str(), message_pubkey.size() + 1);

    // read public key
    recv(fd, buffer.data(), BUFFERSIZE, MSG_NOSIGNAL);
    cout << ip << ": " << buffer.data() << endl;

    string prefix = "[public_key]";
    string buffer_str(buffer.data());
    size_t startPos = buffer_str.find(prefix);
    if (startPos != string::npos) {
        startPos += prefix.length() + 1;
        string public_key = buffer_str.substr(startPos);
        EC_POINT* point = hexStringToEcPoint(group, public_key);
        string ssk = set_secret(priv_key, point, shared_secret_key, &secret_len);
        client.set_shared_key(ssk);
    }
    cout << "Shared secret key [" << ip << "]: " << client.get_shared_secret_key() << endl;

    memset( buffer.data(), 0, BUFFER_SIZE);





    // TODO: check file extension (.txt or .bmp to DD)

    // TODO: if .bmp, get headers using DD

    // TODO: encrypt

    // TODO: send




    return 0;
}


void Client::parse_arguments(int argc, char **argv) {
    int c;
    while ((c = getopt(argc, argv, ":s:p:f:")) != -1) {
        switch (c) {
            case 's': {
                server_ip = optarg;
                break;
            }
            case 'p': {
                server_port= parse_port(optarg, 10);
                break;
            }
            case 'f': {
                file_name = optarg;
                break;
            }
            case ':': {
                fatal_message(
                        __FILE__, __func__, __LINE__,
                        "\"Option requires an operand\"",
                        5);
            }
            case '?': {
                fatal_message(
                        __FILE__, __func__, __LINE__,
                        "Unknown",
                        6);
            }
            default: {
                cout << "should not get here" << endl;
            };
        }
    }
}


void Client::print() {
    cout << "server ip = " << server_ip << endl;
    cout << "server port = " << server_port << endl;
}


//



