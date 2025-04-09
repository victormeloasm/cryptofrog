#include "encrypt.h"
#include "eccfrog512ck2.h"
#include <sodium.h>
#include <fstream>
#include <vector>
#include <cstring>

bool encrypt_file(const std::string& input_file, const std::string& output_file, const std::string& password) {
    std::ifstream in(input_file, std::ios::binary);
    if (!in) return false;

    std::vector<unsigned char> plaintext((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    // ECCFrog512CK2 não usado diretamente aqui — removido o uso da chave
    unsigned char salt[crypto_pwhash_SALTBYTES];
    randombytes_buf(salt, sizeof(salt));

    unsigned char key[crypto_aead_aes256gcm_KEYBYTES];
    if (crypto_pwhash(key, sizeof(key), password.c_str(), password.size(), salt,
                      crypto_pwhash_OPSLIMIT_MODERATE, crypto_pwhash_MEMLIMIT_MODERATE, crypto_pwhash_ALG_DEFAULT) != 0)
        return false;

    unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES];
    randombytes_buf(nonce, sizeof(nonce));

    std::vector<unsigned char> ciphertext(plaintext.size() + crypto_aead_aes256gcm_ABYTES);
    unsigned long long clen;

    if (crypto_aead_aes256gcm_encrypt(ciphertext.data(), &clen, plaintext.data(), plaintext.size(), NULL, 0, NULL, nonce, key) != 0)
        return false;

    std::ofstream out(output_file, std::ios::binary);
    if (!out) return false;

    out.write(reinterpret_cast<const char*>(salt), sizeof(salt));
    out.write(reinterpret_cast<const char*>(nonce), sizeof(nonce));
    out.write(reinterpret_cast<const char*>(ciphertext.data()), clen);

    return true;
}
