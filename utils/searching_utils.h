#pragma once
#include <iostream>
#include <filesystem>
#include <string>

std::filesystem::path find_gnc_dir(const std::filesystem::path& cur_dir);
std::filesystem::path find_file(const std::filesystem::path& cur_dir, const std::string& relative_file_path, bool is_dir = 0);

