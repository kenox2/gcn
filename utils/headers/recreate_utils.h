#pragma once
#include <filesystem>

void recreate_dir_from_tree(const std::filesystem::path& root_dir, const std::filesystem::path& object_dir, const uint64_t& tree_hash);