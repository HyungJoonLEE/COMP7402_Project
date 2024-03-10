#ifndef COMP7402_PROJECT_BRAINPOOL_H
#define COMP7402_PROJECT_BRAINPOOL_H

#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/evp.h>
#include <cassert>
#include <iostream>

#include "common.h"
using namespace std;

void create_private_key(EC_KEY* &key);
string create_public_key(const EC_POINT* &pub, EC_KEY* &key);
EC_POINT* hexStringToEcPoint(const EC_GROUP* group, const string& hexString);
string set_secret(EC_KEY *key, const EC_POINT *peer_pub_key, unsigned char * shared_secret_key, size_t *secret_len);


#endif //COMP7402_PROJECT_BRAINPOOL_H
