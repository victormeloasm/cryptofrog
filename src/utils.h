#ifndef UTILS_H
#define UTILS_H

#include <sodium.h>
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <gmpxx.h>

inline bool derive_key_from_password(const std::string& password, unsigned char* key, unsigned char* salt) {
    return crypto_pwhash(
        key, 32,
        password.c_str(), password.length(),
        salt,
        crypto_pwhash_OPSLIMIT_MODERATE,
        crypto_pwhash_MEMLIMIT_MODERATE,
        crypto_pwhash_ALG_ARGON2ID13
    ) == 0;
}

inline bool secure_delete(const std::string& path) {
    std::error_code ec;
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) return false;

    std::streamsize size = file.tellg();
    file.close();

    std::ofstream wipe(path, std::ios::binary);
    if (!wipe) return false;

    std::vector<unsigned char> zeros(size, 0x00);
    wipe.write((char*)zeros.data(), size);
    wipe.close();

    return std::filesystem::remove(path, ec);
}

inline void generate_random_key(mpz_class& key, const mpz_class& n) {
    size_t num_bytes = 64;
    unsigned char* buffer = new unsigned char[num_bytes];
    randombytes_buf(buffer, num_bytes);

    std::stringstream ss;
    for (size_t i = 0; i < num_bytes; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(buffer[i]);
    }

    key.set_str(ss.str(), 16);
    key = key % (n - 2) + 1;

    sodium_memzero(buffer, num_bytes);
    delete[] buffer;
}

#endif // UTILS_H
