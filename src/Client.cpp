#include "Client.h"
#include "common.h"
#include "brainpool.h"
#include "key.h"

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


    string message;
    message.reserve(1025);
    // TODO: key exchange
    // send public key
    message = "[public_key] " + pub_str;
    write(fd, message.c_str(), message.size() + 1);

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

    // TODO: send iv;
    message = "[iv] " + client.get_iv();
    write(fd, message.c_str(), message.size() + 1);
    message.clear();

    // TODO: send file name (.txt or .bmp to DD)
    message = "[file_name] " + client.get_file_name();
    write(fd, message.c_str(), message.size() + 1);
    message.clear();

    // TODO: send file size
    int file_size = get_file_size(client.get_file_name());
    message = "[file_size] " + to_string(file_size);
    write(fd, message.c_str(), message.size() + 1);
    message.clear();

    // TODO: if .bmp, get headers using DD
    string ext = getFileExtension(client.get_file_name());
    if (ext == ".bmp") {
        vector<char> header = client.read_header(client.get_file_name(), 54);
        // TODO: send header
    }

    // TODO: encrypt
    Feistel f;
    f.CBCencrypt()
    // TODO: send




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
                file_name = optarg;
                break;
            }
            case 'i': {
                iv = optarg;
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
    cout << "file name = " << file_name << endl;
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