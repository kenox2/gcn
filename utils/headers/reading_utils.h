#pragma once
#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

struct IndexEntry {
    std::string path;
    uint32_t mode;
    uint64_t hash;
};


struct TreeEntry{
    uint32_t mode;
    std::string filename;
    uint64_t hash;
};

std::pair<uint64_t, std::string> get_hash_from_HEAD(const std::filesystem::path& head_path);
std::unordered_map<std::string, std::filesystem::path> get_branches(const std::filesystem::path& gcn_dir);
std::vector<TreeEntry> get_tree_entries(const std::filesystem::path& tree_path);
std::vector<IndexEntry> read_index(const std::string& index_path);