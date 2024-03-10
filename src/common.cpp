#include "common.h"


in_port_t parse_port(const char *buff, int radix) {
    char *end;
    long sl;
    in_port_t port;
    const char *msg;

    sl = strtol(buff, &end, radix);

    if (end == buff) {
        msg = "not a decimal number";
    } else if (*end != '\0') {
        msg = "%s: extra characters at end of input";
    } else if ((LONG_MIN == sl || LONG_MAX == sl) && ERANGE == errno) {
        msg = "out of range of type long";
    } else if (sl > UINT16_MAX) {
        msg = "greater than UINT16_MAX";
    } else if (sl < 0) {
        msg = "less than 0";
    } else {
        msg = NULL;
    }

    if (msg) {
        fatal_message(__FILE__, __func__, __LINE__, msg, 2);
    }

    port = (in_port_t) sl;

    return port;
}


string extract_public_key(int socketfd) {
    char buf[128] = {0};
    read(socketfd, buf, 255);

    string bufStr(buf); // Convert the char array to a string
    string prefix = "[public_key]";
    cout << "server: " << buf << endl;
    size_t startPos = bufStr.find(prefix);
    if (startPos != string::npos) {
        startPos += prefix.length() + 1; // +1 for the space after the prefix

        // Extract the substring starting from startPos to the end of the input string
        string pk = bufStr.substr(startPos);
        return pk;
    }
    else {
        cout << "Prefix '[public_key]' not found in the input string." << endl;
        return "";
    }
}


void fatal_errno(const char *file, const char *func, size_t line,
                           int err_code, int exit_code) {
    const char *msg;

    msg = strerror(err_code); // NOLINT(concurrency-mt-unsafe)
    fprintf(stderr, "Error (%s @ %s:%zu %d) - %s\n", file, func, line, err_code,
            msg);    // NOLINT(cert-err33-c)
    exit(exit_code); // NOLINT(concurrency-mt-unsafe)
}


void fatal_message(const char *file, const char *func, size_t line,
                             const char *msg, int exit_code) {
    fprintf(stderr, "Error (%s @ %s:%zu) - %s\n", file, func, line,
            msg);    // NOLINT(cert-err33-c)
    exit(exit_code); // NOLINT(concurrency-mt-unsafe)
}


string print_shared_key(const unsigned char* data, size_t length) {
    ostringstream oss;
    oss << hex << uppercase << setfill('0');
    for (size_t i = 0; i < length; ++i) {
        oss << setw(2) << static_cast<int>(data[i]);
    }
    return oss.str();
}