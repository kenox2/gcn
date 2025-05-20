#pragma once
#include <iostream>;
#include <filesystem>
#include <string>

std::filesystem::path find_gnc_dir();
std::filesystem::path find_file(const std::string& relative_file_path, bool is_dir = 0);

