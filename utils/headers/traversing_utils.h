#pragma once
#include <filesystem>
#include <cstdint>
#include <vector>
#include <unordered_set>
#include <optional>
#include "reading_utils.h"


std::vector<CommitEntry> traverse_DAG();
std::vector<CommitEntry> traverse_DAG(std::string branch_name);
void post_order_dfs(const std::filesystem::path& object_path, const uint64_t& hash, std::unordered_set<uint64_t>& visited, std::vector<uint64_t>& ans);
uint64_t get_merge_base(const std::filesystem::path& object_path, const uint64_t& hash_A, const uint64_t& hash_B);