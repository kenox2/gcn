#pragma once
#include <cstdint>
#include <vector>
#include <iostream>
#include <map>
#include <string>


struct IndexEntry {
    std::string path;
    uint32_t mode;
    uint64_t hash;
};


uint64_t create_blob(const std::string& input_path, const std::string& object_dir);
std::vector<IndexEntry> read_index(const std::string& index_path);
uint64_t write_tree(const std::string& dir,
     const std::string& object_directory,
     const std::map<std::string,
     std::vector<IndexEntry>>& trees,
     const std::vector<IndexEntry>& entries);
uint64_t create_tree(const std::string& dir, const std::string& object_directory, std::string index_path);
int create_commit(const std::string& base_directory,
     const std::string& message,
     const uint64_t& tree_hash,
     const std::string& author,
     uint64_t parent_hash = 0);
