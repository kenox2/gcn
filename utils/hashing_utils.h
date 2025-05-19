#pragma once
#include <string>
#include <fstream>
#include <cstdint>


uint64_t get_hash(std::ifstream& file);
std::string hash_to_path(const uint64_t& hash);
void compress_zlib(std::ifstream& in_stream, std::ofstream& out_stream);
