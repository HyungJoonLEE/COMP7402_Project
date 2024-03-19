#include "Client.h"
#include "common.h"
#include "brainpool.h"
#include "key.h"
#include "Feistel.h"

#define BUFFER_SIZE 256

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
    client.set_fd(fd);
    client.parse_arguments(argc, argv);

    create_private_key(priv_key);
    string pub_str = create_public_key(pub_key, priv_key);

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr(client.get_ip().c_str());
    sockaddr.sin_port = htons(client.get_port());
    string ip = inet_ntoa(sockaddr.sin_addr);

    int flag = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));

    socklen = sizeof(sockaddr);
    if (connect(fd,(struct sockaddr*)&sockaddr,socklen) == -1){
        perror("connect error");
        return 1;
    }


    string message;
    message.reserve(1025);
    // TODO: key exchange
    // send public key
    message = "[public_key] " + pub_str;
    write(fd, message.c_str(), message.size());
    message.clear();

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
        string ssk = hexToASCII(set_secret(priv_key, point, shared_secret_key, &secret_len));
        string ssk_cut = ssk.substr(0, 16);
        cout << "Shared secret key [" << ip << "]: " << strToHex(ssk_cut) << endl;
        client.set_shared_key(strToBin(ssk_cut));
    }
    memset( buffer.data(), 0, BUFFER_SIZE);
    message.clear();

    // TODO: generate key
    Key key;
    key.generateRoundKeys(client.get_shared_secret_key());
    client.set_round_keys(key.getRK());

    char buf[6] = {0};

    // TODO: send iv;
    string iv_message = "[iv] " + client.get_iv();
    write(fd, iv_message.c_str(), iv_message.size());
    read(fd, buf, 6);

    // TODO: send file name (.txt or .bmp to DD)
    string fn_message = "[file_name] " + client.get_file_name();
    write(fd, fn_message.c_str(), fn_message.size());
    read(fd, buf, 6);

    // TODO: if .bmp, send header
//    string ext = getFileExtension(client.get_file_name());
//    if (ext == "bmp") {
//        vector<char> header = client.read_header(client.get_file_name(), 54);
//        string header_str(header.begin(), header.end());
//        cout << header_str << endl;
//        string hdr_message = "[header] " + header_str;
//        write(fd, hdr_message.c_str(), hdr_message.size());
//        read(fd, buf, 6);
//    }

    // TODO: send file size
    int file_size = calculate_file_size(client.get_file_name());
    client.set_file_size(file_size);
    string fs_message = "[file_size] " + to_string(client.get_file_size());
    write(fd, fs_message.c_str(), fs_message.size());
    read(fd, buf, 6);


    // TODO: encrypt & send
    Feistel f;
    f.CBC_encrypt(client);
    write(client.get_fd(), "EOT", 4);

    // TODO: recv ACK
    read(fd, buf, 6);
    cout << "server will close this socket" << endl;

    close(fd);



    return 0;
}


void Client::parse_arguments(int argc, char **argv) {
    int c;
    while ((c = getopt(argc, argv, ":s:p:f:i:")) != -1) {
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
                string fn(optarg);
                file_name = fn;
                break;
            }
            case 'i': {
                string is(optarg);
                iv = strToBin(is);
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


vector<char> Client::read_header(const string& fileName, size_t numBytes) {
    vector<char> buffer(numBytes);

    ifstream file(fileName, ios::binary);
    if (!file) {
        throw runtime_error("Error opening file.");
    }

    file.read(buffer.data(), numBytes);

    if (file.gcount() != numBytes) {
        throw runtime_error("Error reading from file or file too small.");
    }

    return buffer;
}