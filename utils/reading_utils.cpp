#include <iostream>
#include <cstdint>
#include <filesystem>
#include <fstream>


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
    char type_buf[4];
    HEAD.read(type_buf, 4);
    string type(type_buf, 4);
    uint64_t hash; 
    if (type == "path") {
        // Read the rest of the file as a string
        string path_temp((istreambuf_iterator<char>(HEAD)),
                    istreambuf_iterator<char>());
        fs::path path(gcn_dir / path_temp);
        ifstream commit_pointer(path, ios::binary | ios::in);

        if(fs::is_empty(path)) return pair<uint64_t, string>(0, path.string());

        commit_pointer.read(reinterpret_cast<char*>(&hash), sizeof(hash));
        return pair<uint64_t, string>(hash, path.string());
    } else if (type == "tree") {
        // Read the 8-byte hash
        HEAD.read(reinterpret_cast<char*>(&hash), sizeof(hash));
        return pair<uint64_t, string>(hash, "");
    } else {
        cerr << "HEAD is corrupted or unknown type\n";
        exit(1);
    }
}
