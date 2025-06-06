#include <iostream>
#include <fstream>
#include "../../libs/xxhash/xxhash.h"
#include <filesystem>
#include <zlib.h>
#include <vector>
#include <algorithm>
#include <map>
#include <cstdint>
#include "../headers/hashing_utils.h"
#include "../headers/creating_utils.h"
#include "../headers/constants.h"
#include "../headers/searching_utils.h"

namespace fs = std::filesystem;
using namespace std;




uint64_t create_blob(const string& input_path, const string& object_dir) {
    ifstream input_file(input_path, ios::in | ios::binary);
    if (!input_file) {
        cerr << "Invalid file path: " << input_path << "\n";
        return 0;
    }

    uint64_t hash = get_hash(input_file);
    if (hash == 0 || hash == 1) {
        cerr << "Failed to compute hash\n";
        return 2;
    }

    string blob_path = object_dir + hash_to_path(hash);
    fs::create_directories(fs::path(blob_path).parent_path());

    ofstream output_file(blob_path, ios::out | ios::binary);
    if (!output_file) {
        cerr << "Failed to open output file: " << blob_path << "\n";
        return 3;
    }

    // Rewind file and compress
    input_file.clear();
    input_file.seekg(0);
    compress_zlib(input_file, output_file);

    return hash;
}



std::vector<IndexEntry> read_index(const std::string& index_path) {
    std::ifstream in(index_path, std::ios::binary);
    std::vector<IndexEntry> entries;

    if (!in) {
        std::cerr << "Failed to open index file for reading\n";
        return entries;
    }

    uint32_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (uint32_t i = 0; i < count; ++i) {
        uint16_t path_len;
        in.read(reinterpret_cast<char*>(&path_len), sizeof(path_len));

        std::string path(path_len, '\0');
        in.read(&path[0], path_len);

        uint32_t mode;
        in.read(reinterpret_cast<char*>(&mode), sizeof(mode));

        uint64_t hash;
        in.read(reinterpret_cast<char*>(&hash), sizeof(hash));

        entries.push_back(IndexEntry{path, mode, hash});
    }

    return entries;
}



uint64_t write_tree(const string& dir,
                    const fs::path& root,
                    const string& object_directory,
                    const map<string, vector<IndexEntry>>& trees,
                    const vector<IndexEntry>& entries) {
    std::ostringstream buffer;

    for (const auto& entry : entries) {
        fs::path path(root/ entry.path);
        if (path.parent_path().string() == dir) {
            buffer.write(reinterpret_cast<const char*>(&entry.mode), sizeof(entry.mode));

            string file_name = path.filename().string();
            buffer.write(file_name.c_str(), file_name.size() + 1);

            buffer.write(reinterpret_cast<const char*>(&entry.hash), sizeof(entry.hash));
        }
    }

    for (const auto& [subdir, entry] : trees) {
        if (fs::path(subdir).parent_path().string() == dir) {
            
            uint64_t sub_hash = write_tree(subdir, root, object_directory, trees, entries);

            uint32_t mode = static_cast<uint32_t>(FileMode::Tree);
            string dirname = fs::path(subdir).filename().string();

            buffer.write(reinterpret_cast<const char*>(&mode), sizeof(mode));
            buffer.write(dirname.c_str(), dirname.size() + 1);
            buffer.write(reinterpret_cast<const char*>(&sub_hash), sizeof(sub_hash));
        }
    }

    string data = buffer.str();
    uint64_t hash = XXH64(data.data(), data.size(), 0);
    string tree_path = object_directory + hash_to_path(hash);
    fs::create_directories(fs::path(tree_path).parent_path());

    std::ofstream out(tree_path, std::ios::binary);
    out.write(data.data(), data.size());

    return hash;
}



uint64_t create_tree(const string& object_directory,const string& index_path) {
    fs::path root = find_gnc_dir(fs::path(object_directory)).parent_path();
    vector<IndexEntry> entries = read_index(index_path);
    sort(entries.begin(), entries.end(),[](const IndexEntry& a, const IndexEntry& b){
        return a.path < b.path;
    });
    map<string, vector<IndexEntry>> trees;
    for (const auto& entry : entries) {
        fs::path p(entry.path);
        fs::path parent = (root / p).parent_path();
        trees[parent.lexically_normal().string()].push_back(entry);
    }
    return write_tree(root.string(), root, object_directory, trees, entries);

}


uint64_t create_commit(const string& base_directory, const string& message, const uint64_t& tree_hash, const string& author, const uint64_t& parent_hash ){
    
    ostringstream buffer;
    uint32_t tree_label = static_cast<uint32_t>(FileMode::Tree);
    buffer.write(reinterpret_cast<const char*>(&tree_label),sizeof(tree_label));
    buffer.write(reinterpret_cast<const char*>(&tree_hash), sizeof(tree_hash));

    if (parent_hash != 0) {
        string parent_label = "parent";
        buffer.write(parent_label.c_str(), parent_label.size());
        buffer.write(reinterpret_cast<const char*>(&parent_hash), sizeof(parent_hash));
    }

    string author_label = "author";
    buffer.write(author_label.c_str(), author_label.size());
    buffer.write(author.c_str(), author.size() + 1);

    buffer.write(message.c_str(), message.size());

    string data = buffer.str();
    uint64_t hash = XXH64(data.data(), data.size(), 0);
//  debuggin info
    cout << "hash in create_commit: " << hash << endl;
    string commit_path_str = base_directory + hash_to_path(hash);
    fs::path commit_path(commit_path_str);
    fs::create_directories(commit_path.parent_path());
    ofstream out(commit_path, ios::binary);
    out.write(data.data(), data.size());

    return hash;
}

