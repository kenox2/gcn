#include <iostream>
#include <cstdint>
#include <string>
#include "../utils/headers/creating_utils.h"
#include "../utils/headers/searching_utils.h"
#include "../utils/headers/reading_utils.h"
#include <fstream>

namespace fs = std::filesystem;

using namespace std;

int commit(const string& message){
    
    //  create commit
    fs::path cur = fs::current_path();
    cout << cur.filename();
    fs::path index = find_file(cur, "INDEX");
    fs::path HEAD = find_file(cur, "HEAD");
    // check if INDEX is empty
    if(fs::file_size(index) == 0){
        cout << "Nothing to commit: INDEX is empty.\n";
        return 1; 
    } 
    fs::path objects = find_file(cur,"objects", true);
    fs::path user_data = find_file(cur,"user_data");
    uint64_t hash_tree = create_tree(objects.string(), index.string());
//  debuggin info
    cout << "hash_tree in value: " << hash_tree << endl;
    // get information from user_data 
    ifstream user_file(user_data, ios::in);
    string name;
    string email;
    getline(user_file, name);
    getline(user_file, email);
    name = name.substr(9);
    email = email.substr(6);
    string author = name + " " + email;

    // get parent hash 
    pair<uint64_t, string> parent_hash = get_hash_from_HEAD(HEAD);
    // create commit
    uint64_t hash_commit = create_commit(objects.string(), message, hash_tree, author, parent_hash.first);
//  debuggin info
    cout << "hash_commit value: " << hash_commit << endl;
    // save commit to branch 
    if(parent_hash.second == ""){
        cout << "Loose commit (not assigned to anything)";
        return 0;
    }
    fs::path branch_path(parent_hash.second);
    ofstream branch_head(branch_path, ios::binary | ios::out);
    if(!branch_head){
        cerr << "Failed to open branch\n";
        exit(1);
    }
    branch_head.write(reinterpret_cast<char *>(&hash_commit), sizeof(hash_commit));

    // empty out INDEX 
    ofstream(index, ios::binary | ios::out);
    return 0;
}



int main(int argc, char* argv[]){
   if(argc != 2){
        cerr << "Usage: commit <message>\n";
        exit(1);
    }
    string message = argv[1];
    try {
        return commit(message);
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << '\n';
        return 1;
    }
}
