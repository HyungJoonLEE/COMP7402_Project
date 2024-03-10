#include "brainpool.h"


void create_private_key(EC_KEY* &key) {
    if (NULL == (key = EC_KEY_new_by_curve_name(NID_brainpoolP256r1))) {
        printf("Failed to create key curve\n");
        return;
    }

    // Creates a new ec private (and optional a new public) key.
    if (1 != EC_KEY_generate_key(key)) {
        printf("Failed to generate key\n");
        return;
    }
}


string create_public_key(const EC_POINT* &pub, EC_KEY* &key) {
    EC_GROUP *ec_group = EC_GROUP_new_by_curve_name(NID_brainpoolP256r1);
    pub = EC_KEY_get0_public_key(key);
    string str_pub;

    // Convert the public key to a hexadecimal string
    char *hex = EC_POINT_point2hex(ec_group, pub, POINT_CONVERSION_COMPRESSED, NULL);
    if (!hex) {
        cerr << "Failed to convert public key to hex." << endl;
    }

    str_pub = hex;
    // Free the hexadecimal string memory
    OPENSSL_free(hex);

    return str_pub;
}


EC_POINT* hexStringToEcPoint(const EC_GROUP* group, const string& hexString) {
    EC_POINT* point = NULL;
    BN_CTX* ctx = NULL;
    BIGNUM* bn = NULL;

    do {
        // Create a new BN_CTX
        ctx = BN_CTX_new();
        if (!ctx) break;

        // Convert the hexadecimal string to a BIGNUM
        bn = BN_new();
        if (!BN_hex2bn(&bn, hexString.c_str())) break;

        // Create a new EC_POINT object
        point = EC_POINT_new(group);
        if (!point) break;

        // Set the EC_POINT to the value from the BIGNUM
        if (!EC_POINT_bn2point(group, bn, point, ctx)) {
            EC_POINT_free(point);
            point = NULL;
        }
    } while (0);

    // Free resources
    if (bn) BN_free(bn);
    if (ctx) BN_CTX_free(ctx);

    return point;
}


string set_secret(EC_KEY *key, const EC_POINT *peer_pub_key,
               unsigned char * shared_secret_key,
               size_t *secret_len) {
    int field_size;

    field_size = EC_GROUP_get_degree(EC_KEY_get0_group(key));
    *secret_len = (field_size + 7) / 8;

    if (nullptr == (shared_secret_key = (unsigned char *)OPENSSL_malloc(*secret_len))) {
        printf("Failed to allocate memory for secret");
        return "";
    }

    *secret_len = ECDH_compute_key(shared_secret_key, *secret_len,
                                   peer_pub_key, key, NULL);

    if (*secret_len <= 0) {
        OPENSSL_free(shared_secret_key);
        return "";
    }

    string ssk = print_shared_key(shared_secret_key, *secret_len);
    return ssk;
}
