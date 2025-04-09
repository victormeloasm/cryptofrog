#ifndef DECRYPT_H
#define DECRYPT_H

#include <string>

// Descriptografa o arquivo de entrada para o arquivo de saída utilizando a senha.
bool decrypt_file(const std::string& input_file, const std::string& output_file, const std::string& password);

#endif
