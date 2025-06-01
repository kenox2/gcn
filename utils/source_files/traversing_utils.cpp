#include <fstream>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <vector>
#include "../headers/searching_utils.h"
#include "../headers/reading_utils.h"
#include "../headers/traversing_utils.h"
#include <deque>
#include <optional>


namespace fs = std::filesystem;
using namespace std;


uint64_t get_merge_base(const fs::path& object_path, const uint64_t& hash_A, const uint64_t& hash_B) {
    unordered_map<uint64_t, int> visited;
    deque<std::pair<uint64_t, int>> queue;

    queue.push_front({hash_A, 1});
    queue.push_front({hash_B, 2});

    while (!queue.empty()) {
        auto [hash, origin] = queue.front();
        queue.pop_front();

        if (visited.count(hash)) {
            if (visited[hash] != origin) {
                return hash; // found common ancestor
            }
        } else {
            visited[hash] = origin;

            fs::path path = find_file_by_hash(object_path, hash);
            CommitEntry entry;
            entry.hash = hash;
            get_commit_content(entry, path);

            for (auto& parent : entry.parents) {
                queue.push_back({parent, origin});
            }
        }
    }

    throw invalid_argument("No common ancestor found"); // no common ancestor found
}



void post_order_dfs(const fs::path& object_path, const uint64_t& hash, unordered_set<uint64_t>& visited, vector<CommitEntry>& ans){
    if(visited.find(hash) != visited.end()) return;
    visited.insert(hash);
    fs::path path = find_file_by_hash(object_path, hash);
    CommitEntry entry;
    entry.hash = hash;
    get_commit_content(entry, path);

    for(auto& parent : entry.parents){
        post_order_dfs(object_path, parent, visited, ans);
    }

    ans.push_back(entry);
}

vector<CommitEntry> traverse_DAG(){
    unordered_set<uint64_t> visited;
    vector<CommitEntry> ans;
    fs::path object_path = find_file(fs::current_path(), "objects", true);
    unordered_map<string, fs::path> map = get_branches(object_path.parent_path());
    vector<uint64_t> starting_points;
    for(auto& branch : map){
        try{
            starting_points.push_back(get_hash_from_commit(branch.second));
        }catch(const std::invalid_argument& e){
            continue;
        }
    }
    for(auto& start : starting_points){
        post_order_dfs(object_path, start, visited, ans);
    }

    return ans;

}

vector<CommitEntry> traverse_DAG(string branch_name){
    unordered_set<uint64_t> visited;
    vector<CommitEntry> ans;
    fs::path object_path = find_file(fs::current_path(), "objects", true);
    unordered_map<string, fs::path> map = get_branches(object_path.parent_path());
    uint64_t start;
    if(map.count(branch_name) == 0){
        throw std::invalid_argument("incorrect branch");
    }
    try{
        start = get_hash_from_commit(map[branch_name]);
    }catch(const std::invalid_argument& e){
        throw std::invalid_argument("branch does not have any commits.");
    }
    post_order_dfs(object_path, start, visited, ans);    
        
    return ans;

}