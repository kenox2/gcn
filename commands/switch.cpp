#include <iostream>
#include <string>
#include "../utils/reading_utils.h"
#include "../utils/searching_utils.h"
#include "../utils/constants.h"
#include "../utils/recreate_utils.h"
#include <filesystem>
#include <unordered_map>
#include <fstream>
#include <cstdint>

namespace fs = std::filesystem;
using namespace std;


void write_to_head (const fs::path& head_path,const string& path){
    uint32_t type = static_cast<uint32_t>(FileMode::Path); 
    ofstream HEAD(head_path, ios::binary | ios::in);
    if(!HEAD){
        throw std::runtime_error("HEAD file can't be openend");
    }
    HEAD.write(reinterpret_cast<char *>(&type), sizeof(type));
    HEAD.write(path.data(), path.size());
}

void write_to_head(const fs::path& head_path, uint64_t& hash){
    uint32_t type = static_cast<uint32_t>(FileMode::Commit); 
    ofstream HEAD(head_path, ios::binary | ios::in);
    if(!HEAD){
            throw std::runtime_error("HEAD file can't be openend");
    }
    HEAD.write(reinterpret_cast<char *>(&type), sizeof(type));
    HEAD.write(reinterpret_cast<char *>(&hash), sizeof(hash));
}
    




int switch_(const string& target){
// change pointer in HEAD
    fs::path objects_path = find_file(fs::current_path(), "objects", true);
    fs::path head_path = objects_path.parent_path() / "HEAD";
    fs::path root_path = objects_path.parent_path().parent_path();
    unordered_map<string, fs::path> map = get_branches(objects_path.parent_path());
    //  get hash
    

    uint64_t hash;
    fs::path commit_path;
    if(map.count(target) > 0){
        // case target is branch
        fs::path commit_path = map[target];
        ifstream branch(commit_path, ios::binary | ios::in);
        if(!branch){
            cerr << "Could not open branch file";
            return 1;
        }
        branch.read(reinterpret_cast<char*>(&hash), sizeof(hash));
        branch.close();
        commit_path= find_file_by_hash(objects_path, hash);

        // insert new branch path into head
        write_to_head(head_path, commit_path.string());
    }else{
        // case target is a hash
        if(target.size() < 4){
            cout << "Target hash must be atleat 4 characters long.";
            return 2;
        }
        commit_path = find_file_by_hash(objects_path, target);
        // get uint_64 has from file
        string hash_str = commit_path.parent_path().filename().string() + commit_path.filename().string();
        hash = stoull(hash_str);

        //insert new commit into head
        write_to_head(head_path, hash);
    }
    // get tree hash
    ifstream commit(commit_path, ios::binary);
    if(!commit){
        cerr << "Commit can't be opened";
        return 3;
    }
    uint32_t mode;
    uint64_t tree_hash;
    commit.read(reinterpret_cast<char *>(&mode), sizeof(mode));
    commit.read(reinterpret_cast<char *>(&tree_hash), sizeof(tree_hash));
    if(mode != static_cast<uint32_t>(FileMode::Tree)){
        cerr << "Commit is corrupted";
        return 4;
    }
    // recreate commit 
    recreate_dir_from_tree(root_path, objects_path, tree_hash);
    return 0;
}


int main(int argc, char **argv){
    string arg = argv[1];
    return switch_(arg);;
}