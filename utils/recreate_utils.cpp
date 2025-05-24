#include <iostream>
#include <cstdint>
#include <filesystem>
#include "searching_utils.h"
#include "reading_utils.h"
#include "recreate_utils.h"
#include <vector>
#include "constants.h"
#include <fstream>
#include "hashing_utils.h"

#ifdef __linux__
    #include <sys/stat.h>
#endif

namespace fs = std::filesystem;
using namespace std;



void recreate_dir_from_tree(const fs::path& root_dir, const fs::path& object_dir, const uint64_t& tree_hash) {
    fs::path tree_path = find_file_by_hash(object_dir, tree_hash);
    vector<TreeEntry> entries = get_tree_entries(tree_path);

    for (auto entry : entries) {
#ifdef _WIN32
        if (entry.mode == static_cast<uint32_t>(FileMode::Exec)) {
            if (entry.filename.size() < 4 || entry.filename.substr(entry.filename.size() - 4) != ".exe") {
                entry.filename += ".exe";
            }
        }
#endif
        fs::path target_path = root_dir / entry.filename;

        if (entry.mode == static_cast<uint32_t>(FileMode::Blob) || entry.mode == static_cast<uint32_t>(FileMode::Exec)) {
            fs::path blob_path = find_file_by_hash(object_dir, entry.hash);
            ifstream blob(blob_path, ios::binary);
            ofstream file(target_path, ios::binary);
            if (!blob || !file) throw runtime_error("Failed to open file");
            decompress_zlib(blob, file);
            file.close();
            blob.close();
#ifdef __linux__
            if (entry.mode == static_cast<uint32_t>(FileMode::Exec)) {
                chmod(target_path.c_str(), 0755);
            }
#endif
        } else {
            fs::create_directory(target_path);
            recreate_dir_from_tree(target_path, object_dir, entry.hash);
        }
    }
}