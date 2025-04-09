// decrypt.cpp (corrigido - leitura binária e validação AES-GCM)
#include "decrypt.h"
#include "eccfrog512ck2.h"
#include <sodium.h>
#include <fstream>
#include <vector>
#include <cstring>

bool decrypt_file(const std::string& input_file, const std::string& output_file, const std::string& password) {
    std::ifstream in(input_file, std::ios::binary);
    if (!in) return false;

    unsigned char salt[crypto_pwhash_SALTBYTES];
    unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES];

    in.read(reinterpret_cast<char*>(salt), sizeof(salt));
    in.read(reinterpret_cast<char*>(nonce), sizeof(nonce));

    std::vector<unsigned char> ciphertext((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    unsigned char key[crypto_aead_aes256gcm_KEYBYTES];
    if (crypto_pwhash(key, sizeof(key), password.c_str(), password.size(), salt,
                      crypto_pwhash_OPSLIMIT_MODERATE, crypto_pwhash_MEMLIMIT_MODERATE, crypto_pwhash_ALG_DEFAULT) != 0)
        return false;

    std::vector<unsigned char> decrypted(ciphertext.size());
    unsigned long long decrypted_len;

    if (crypto_aead_aes256gcm_decrypt(decrypted.data(), &decrypted_len, NULL,
                                      ciphertext.data(), ciphertext.size(), NULL, 0, nonce, key) != 0)
        return false;

    std::ofstream out(output_file, std::ios::binary);
    if (!out) return false;

    out.write(reinterpret_cast<const char*>(decrypted.data()), decrypted_len);
    return true;
}