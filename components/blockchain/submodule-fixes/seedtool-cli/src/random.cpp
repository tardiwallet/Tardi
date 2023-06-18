//
//  random.cpp
//
//  Copyright © 2020 by Blockchain Commons, LLC
//  Licensed under the "BSD-2-Clause Plus Patent License"
//

#include <assert.h>
#include <string.h>
#include <stdexcept>
#include <algorithm>

#include "bc-crypto-base.h"

#include "random.hpp"
#include "randombytes.h"
#include "hkdf.h"

using namespace std;

void crypto_random(uint8_t* buf, size_t n, void* ctx) {
    assert(randombytes(buf, n) == 0);
}

static uint8_t deterministic_seed[SHA256_DIGEST_LENGTH];
static uint64_t deterministic_salt = 0;

void seed_deterministic_string(const string &string) {
    sha256_Raw((uint8_t*)string.c_str(), string.length(), deterministic_seed);
    deterministic_salt = 0;
}

void deterministic_random(uint8_t* buf, size_t n, void* ctx) {
    deterministic_salt += 1;

    hkdf_sha256(buf, n,
    (uint8_t*)&deterministic_salt, sizeof(deterministic_salt),
    deterministic_seed, SHA256_DIGEST_LENGTH,
    NULL, 0);
}

ByteVector deterministic_random(const ByteVector &entropy, size_t n) {
    ByteVector result;
    result.resize(n);

    auto seed = sha256(entropy);

    hkdf_sha256(result.data(), n,
    NULL, 0, // no salt
    seed.data(), SHA256_DIGEST_LENGTH,
    NULL, 0); // no info

    return result;
}

ByteVector sha256_deterministic_random(const ByteVector &entropy, size_t n) {
    auto seed = sha256(entropy);
    if(n <= seed.size()) {
        return take(seed, n);
    } else {
        throw runtime_error("Random number generator limits reached.");
    }
}

ByteVector sha256_deterministic_random(const string &string, size_t n) {
    ByteVector entropy;
    std::copy(string.begin(), string.end(), back_inserter(entropy));
    return sha256_deterministic_random(entropy, n);
}
