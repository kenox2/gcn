#include <iostream>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include "../headers/constants.h"
#include <unordered_map>
#include <string>
#include "../headers/reading_utils.h"
#include <vector>


namespace fs = std::filesystem;
using namespace std;

pair<uint64_t, string> get_hash_from_HEAD(const fs::path& head_path) {
    ifstream HEAD(head_path, ios::binary);
    if (!HEAD) {
        cerr << "Failed to open HEAD\n";
        exit(1);
    }

    //  Get root dir
    fs::path gcn_dir = head_path.parent_path();
    // Read the 4-byte type
    uint32_t type;
    char type_buf[4];
    HEAD.read(reinterpret_cast<char*>(&type), sizeof(type));
    uint64_t hash; 
    if (type == static_cast<uint32_t>(FileMode::Path)) {
        // Read the rest of the file as a string
        string path_temp((istreambuf_iterator<char>(HEAD)),
                    istreambuf_iterator<char>());
        fs::path path(gcn_dir / path_temp);
        if(fs::is_empty(path)) return pair<uint64_t, string>(0, path.string());
        
        ifstream commit_pointer(path, ios::binary | ios::in);
        commit_pointer.read(reinterpret_cast<char*>(&hash), sizeof(hash));
        return pair<uint64_t, string>(hash, path.string());
    } else if (type == static_cast<uint32_t>(FileMode::Commit)) {
        // Read the 8-byte hash
        HEAD.read(reinterpret_cast<char*>(&hash), sizeof(hash));
        return pair<uint64_t, string>(hash, "");
    } else {
        cerr << "HEAD is corrupted or unknown type\n";
        exit(1);
    }
}


unordered_map<string, fs::path> get_branches(const fs::path& gcn_dir){
    fs::path branch_dir = gcn_dir / "refs" /"heads";
    fs::directory_iterator it(branch_dir);
    unordered_map<string, fs::path> map;
    for(auto entry : it){
        if(entry.is_regular_file()){
            map[entry.path().filename().string()] = entry.path();
        }
    }
    return map;
}


vector<TreeEntry> get_tree_entries(const fs::path& tree_path){

    vector<TreeEntry> tree_entries;
    ifstream tree_file(tree_path, ios::in | ios::binary);
    if (!tree_file) {
        throw runtime_error("Invalid tree path");
    }

    while (true) {
        TreeEntry entry;

        tree_file.read(reinterpret_cast<char*>(&entry.mode), sizeof(entry.mode));
        if (tree_file.eof()) break; // No more entries
        if (!tree_file) throw runtime_error("Failed to read mode");

        char c;
        while (tree_file.get(c)) {
            if (c == '\0') break;
            entry.filename += c;
        }
        if (!tree_file) throw runtime_error("Failed to read filename");

        tree_file.read(reinterpret_cast<char*>(&entry.hash), sizeof(entry.hash));
        if (!tree_file) throw runtime_error("Failed to read hash");

         tree_entries.push_back(entry);
    }

    return tree_entries;
}



void get_commit_content(CommitEntry& entry, const fs::path& commit_path) {
    std::ifstream commit(commit_path, std::ios::binary);
    if (!commit) {
        throw std::runtime_error("Failed to open commit file: " + commit_path.string());
    }

    uint32_t tree_mode;
    uint64_t tree_hash;
    commit.read(reinterpret_cast<char*>(&tree_mode), sizeof(tree_mode));
    commit.read(reinterpret_cast<char*>(&entry.tree_hash), sizeof(entry.tree_hash));

    while (true) {
        std::string id(6, '\0');
        commit.read(&id[0], 6);
        if (!commit || id != "parent") {
            break;
        }

        uint64_t parent_hash;
        commit.read(reinterpret_cast<char*>(&parent_hash), sizeof(parent_hash));
        if (!commit) break;

        entry.parents.push_back(parent_hash);
    }

    std::getline(commit, entry.author, '\0');

    std::string message((std::istreambuf_iterator<char>(commit)), std::istreambuf_iterator<char>());
    entry.message = message;
}


uint64_t get_hash_from_commit(const fs::path& commit_path){
    if(fs::is_empty(commit_path)) throw std::invalid_argument("commit is empty");
    uint64_t hash;
    ifstream commit(commit_path, ios::binary);
    commit.read(reinterpret_cast<char *>(&hash), sizeof(hash));
    return hash;
}