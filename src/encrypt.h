#ifndef ENCRYPT_H
#define ENCRYPT_H

#include <string>
#include "eccfrog512ck2.h"

// Criptografa o arquivo de entrada e salva em saída utilizando a senha para derivar a chave.
// A função espera que a derivação de chave (via Argon2ID) e os cálculos com a curva elíptica sejam feitos internamente.
bool encrypt_file(const std::string& input_file, const std::string& output_file, const std::string& password);

#endif
