#pragma once
#include <cstdint>
#include <filesystem>
#include <string>

std::pair<uint64_t, std::string> get_hash_from_HEAD(const std::filesystem::path& head_path);