#include <iostream>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include "../utils/headers/searching_utils.h"

namespace fs = std::filesystem;
using namespace std;



void print_index(fs::path index_path) {
    ifstream index(index_path, ios::binary);
    if (!index) throw runtime_error("Failed to open index file");
    uint32_t count;
    index.read(reinterpret_cast<char *>(&count), sizeof(count));
    cout << count << endl;
    while (true) {
        uint16_t path_len;
        uint64_t hash;
        uint32_t mode;

        // Exit cleanly if end-of-file is reached before reading the next entry
        if (!index.read(reinterpret_cast<char *>(&path_len), sizeof(path_len))) break;

        string path(path_len, '\0');  // allocate string of correct size
        if (!index.read(path.data(), path_len)) break;
        if (!index.read(reinterpret_cast<char *>(&mode), sizeof(mode))) break;
        if (!index.read(reinterpret_cast<char *>(&hash), sizeof(hash))) break;

        cout << path_len << " " << path << " " << mode << " " << hash << endl;
    }
}

void print_commit(const fs::path& commit_path){
//  debuggin function
    ostringstream commit_content;
    ifstream commit(commit_path, ios::binary);
    if(!commit){
        cerr << "File cant be opened \n";
        return ;
    }
    uint32_t mode;
    uint64_t hash;
    commit.read(reinterpret_cast<char *>(&mode), sizeof(mode));
    commit.read(reinterpret_cast<char *>(&hash), sizeof(hash));
   
    std::string id(6, '\0');
    commit.read(&id[0], 6);
    uint64_t parent_hash;
    if (id == "parent") {
        commit.read(reinterpret_cast<char*>(&parent_hash), sizeof(parent_hash));
    }
    
   

    
    char buffer[4096];
    while(commit.read(buffer, sizeof(buffer)) || commit.gcount() > 0){
        commit_content.write(buffer, commit.gcount());
    }
    // process information for printing
    string output = commit_content.str();
    for(char& c : output ){
        if(c == '\0') c = '\n';
    }
    cout <<  to_string(mode) + " " + to_string(hash) + '\n' + '\n' + "parent: " + to_string(parent_hash) + output;
}



void print_tree(const fs::path& tree_path) {
    ifstream tree_file(tree_path, ios::in | ios::binary);
    if (!tree_file) {
        throw runtime_error("Invalid tree path");
    }

    while (true) {
        uint32_t mode;
        uint64_t hash;
        string filename;

        tree_file.read(reinterpret_cast<char*>(&mode), sizeof(mode));
        if (tree_file.eof()) break; // No more entries
        if (!tree_file) throw runtime_error("Failed to read mode");

        char c;
        while (tree_file.get(c)) {
            if (c == '\0') break;
            filename += c;
        }
        if (!tree_file) throw runtime_error("Failed to read filename");

        tree_file.read(reinterpret_cast<char*>(&hash), sizeof(hash));
        if (!tree_file) throw runtime_error("Failed to read hash");

        cout << mode << " " << filename << " " << hash << "\n";
    }
}


void print_branch(fs::path branch_path){
    ifstream branch(branch_path, ios::binary);
    uint64_t hash;
    branch.read(reinterpret_cast<char*>(&hash), sizeof(hash));
    cout << "commit_hash: " << hash;
}





int main(int argc, char **argv){
    string hash = argv[1];
    string type = argv[2];
    fs::path object_dir = find_file(fs::current_path(), "objects", true);
    fs::path branch = object_dir.parent_path() / "refs/heads" / hash;
    if(type == "0"){
        fs::path file_path = find_file_by_hash(object_dir, hash);
        print_tree(file_path);
    }else if(type == "1"){
        fs::path file_path = find_file_by_hash(object_dir, hash);
        print_commit(file_path);
    }else if(type == "2"){
        fs::path index_path(object_dir.parent_path() / "INDEX");
        print_index(index_path);
    }else if(type == "3"){
        print_branch(branch);
    }
    return 0;
}