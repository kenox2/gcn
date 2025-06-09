#include <iostream>
#include <string>
#include "../utils/headers/reading_utils.h"
#include "../utils/headers/searching_utils.h"
#include "../utils/headers/constants.h"
#include "../utils/headers/recreate_utils.h"
#include <filesystem>
#include <unordered_map>
#include <fstream>
#include <cstdint>

namespace fs = std::filesystem;
using namespace std;


void write_to_head (const fs::path& head_path,const string& path){
    uint32_t type = static_cast<uint32_t>(FileMode::Path); 
    ofstream HEAD(head_path,  ios::binary | ios::out);
    if(!HEAD){
        throw std::runtime_error("HEAD file can't be openend");
    }
    HEAD.write(reinterpret_cast<char *>(&type), sizeof(type));
    
    HEAD.write(path.data(), path.size());
}

void write_to_head(const fs::path& head_path, uint64_t& hash){
    uint32_t type = static_cast<uint32_t>(FileMode::Commit); 
    ofstream HEAD(head_path,  ios::binary | ios::out);
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
        fs::path branch_path = map[target];
        // insert new branch path into head
        fs::path relative_branch_path = fs::relative(branch_path, objects_path.parent_path());
        write_to_head(head_path, relative_branch_path.string());
        if(fs::is_empty(branch_path)){
            return 0;
        }
        // get commit hash
        uint64_t commit_hash;
        ifstream branch(branch_path, ios::binary);
        branch.read(reinterpret_cast<char *>(&commit_hash), sizeof(commit_hash));
        commit_path = find_file_by_hash(objects_path, commit_hash);
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
    if(argc != 2){
        cerr << "Usage: switch <branch_name/hash_commit>\n";
        exit(1);
    }
    string arg = argv[1];
    try{
        return switch_(arg);
    }catch(const runtime_error& e){
        cerr << "ERROR: " << e.what() << endl;
        return 1;
    }
    
}