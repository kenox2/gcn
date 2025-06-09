#include <iostream>
#include <filesystem>
#include <fstream>
#include <optional>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "../utils/headers/traversing_utils.h"
#include "../utils/headers/searching_utils.h"
#include "../utils/headers/reading_utils.h"
#include "../utils/headers/constants.h"

namespace fs = std::filesystem;
using namespace std;


uint64_t get_tree_hash(const fs::path& object_dir, uint64_t commit_hash) {
    fs::path commit_path = find_file_by_hash(object_dir, commit_hash);
    ifstream tree(commit_path, ios::binary);
    if (!tree) throw runtime_error("Cannot open tree file");

    uint32_t mode;
    uint64_t hash;
    tree.read(reinterpret_cast<char*>(&mode), sizeof(mode));
    tree.read(reinterpret_cast<char*>(&hash), sizeof(hash));
    return hash;
}





struct MergeResult {
    std::unordered_map<fs::path, TreeEntry> merged_files;
    std::unordered_map<fs::path, pair<TreeEntry, TreeEntry>> conflicting_files; // pair<a_entry, b_entry>
};



// get entries of all files recurisvely
void accumulate_entries(const fs::path& object_dir,
                        const fs::path root_dir,
                        const uint64_t hash,
                        std::unordered_map<fs::path, TreeEntry>& out_map) {

    // walk thru entries. regulat files add to map not regular recursively call the function and add them with prefix to the same map;
    auto entries = get_tree_entries(find_file_by_hash(object_dir, hash));

    for(auto& entry : entries){
        if(entry.mode == static_cast<uint32_t>(FileMode::Tree)){
            accumulate_entries(object_dir, root_dir / entry.filename, entry.hash, out_map);
        }else{
            out_map[root_dir / entry.filename] = entry;
        }
    }
}




// find conflicts
MergeResult find_conflicts(const std::unordered_map<fs::path, TreeEntry>& a_map,
                           const std::unordered_map<fs::path, TreeEntry>& b_map,
                           const std::unordered_map<fs::path, TreeEntry>& base_map) {
    MergeResult result;

    std::unordered_set<fs::path> all_paths;
    for (const auto& [k, _] : a_map) all_paths.insert(k);
    for (const auto& [k, _] : b_map) all_paths.insert(k);
    for (const auto& [k, _] : base_map) all_paths.insert(k);

    for (const auto& path : all_paths) {
        bool in_a = a_map.count(path) > 0;
        bool in_b = b_map.count(path) > 0;
        bool in_base = base_map.count(path) > 0;

        const auto& a_entry = in_a ? a_map.at(path) : TreeEntry{};
        const auto& b_entry = in_b ? b_map.at(path) : TreeEntry{};
        const auto& base_entry = in_base ? base_map.at(path) : TreeEntry{};

        bool type_conflict = (in_a && in_b) && (a_entry.mode != b_entry.mode);

        if (in_base && in_a && in_b) {
            // Conflict: modified differently on both sides
            if (a_entry.hash != base_entry.hash &&
                b_entry.hash != base_entry.hash &&
                a_entry.hash != b_entry.hash) {
                result.conflicting_files[path] = {a_entry, b_entry};
                continue;
            }

            // Conflict: file vs directory type mismatch
            if (type_conflict) {
                result.conflicting_files[path] = {a_entry, b_entry};
                continue;
            }

            // No conflict, prefer changed version or base
            if (b_entry.hash != base_entry.hash) {
                result.merged_files[path] = b_entry;
            } else if (a_entry.hash != base_entry.hash) {
                result.merged_files[path] = a_entry;
            } else {
                result.merged_files[path] = base_entry;
            }

        } else if (!in_base && in_a && in_b) {
            // Conflict: both added different content
            if (a_entry.hash != b_entry.hash || type_conflict) {
                result.conflicting_files[path] = {a_entry, b_entry};
            } else {
                result.merged_files[path] = a_entry; // or b_entry, same content
            }

        } else if (in_base && in_a && !in_b) {
            // Conflict: deleted in b but modified in a
            if (a_entry.hash != base_entry.hash) {
                result.conflicting_files[path] = {a_entry, TreeEntry{}};
            } else {
                // Deleted in b and unchanged in a -> do nothing (skip)
            }

        } else if (in_base && !in_a && in_b) {
            // Conflict: deleted in a but modified in b
            if (b_entry.hash != base_entry.hash) {
                result.conflicting_files[path] = {TreeEntry{}, b_entry};
            } else {
                // Deleted in a and unchanged in b -> do nothing (skip)
            }

        } else if (in_a && !in_b) {
            // Added or modified only in a
            result.merged_files[path] = a_entry;

        } else if (!in_a && in_b) {
            // Added or modified only in b
            result.merged_files[path] = b_entry;
        }
        // Deleted in both sides (in_base && !in_a && !in_b) does nothing, so omitted
    }

    return result;
}






void handle_conflict(unordered_map<fs::path, pair<TreeEntry, TreeEntry>> conflicting_files){
    cout << "Conflict arises in:  \n";
    for(auto& conflict : conflicting_files){
        auto [path, entry] = conflict;
        cout << path.string() << endl;
    }
    cout << "Solve conflicts manually before merging \n";
}




void write_to_index(uint32_t count, const fs::path& root_path, const fs::path& index_path, const unordered_map<fs::path, TreeEntry>& merge_files){
    // each merge_file write to index
    ofstream index(index_path, ios::binary);
    if(!index) throw runtime_error("Index can't be opened");
    index.write(reinterpret_cast<char *>(&count), sizeof(count));
    for(auto& file :merge_files){
        auto [path, entry] = file;
        string path_str = fs::relative(path, root_path).string();
        uint16_t path_len = path_str.size();
        index.write(reinterpret_cast<char*>(&path_len), sizeof(path_len));
        index.write(path_str.data(), path_str.size());
        index.write(reinterpret_cast<char*>(&entry.mode), sizeof(entry.mode));
        index.write(reinterpret_cast<char*>(&entry.hash), sizeof(entry.hash));
    }
}





int merge(const string& branch_name) {
    fs::path object_path = find_file(fs::current_path(), "objects", true);
    fs::path head_path = find_file(fs::current_path(), "HEAD");
    fs::path root_path = object_path.parent_path();
    fs::path index_path = root_path / "INDEX";

    auto [head_hash, _] = get_hash_from_HEAD(head_path);
    uint64_t other_hash = get_hash_from_commit(root_path / "refs/heads" / branch_name);

    uint64_t base_hash = get_merge_base(object_path, head_hash, other_hash);

    uint64_t tree_A = get_tree_hash(object_path, head_hash);
    uint64_t tree_B = get_tree_hash(object_path, other_hash);
    uint64_t tree_Base = get_tree_hash(object_path, base_hash);

    unordered_map<fs::path, TreeEntry> a_map;
    unordered_map<fs::path, TreeEntry> b_map;
    unordered_map<fs::path, TreeEntry> base_map;

    accumulate_entries(object_path, root_path, tree_A, a_map);
    accumulate_entries(object_path, root_path, tree_B, b_map);
    accumulate_entries(object_path, root_path, tree_Base, base_map);

    MergeResult conflicts = find_conflicts(a_map, b_map, base_map);

    if(conflicts.conflicting_files.size() > 0){
        handle_conflict(conflicts.conflicting_files);
        return 0;
    }

    uint32_t size = conflicts.conflicting_files.size() + conflicts.merged_files.size();
    write_to_index(size, root_path, index_path, conflicts.merged_files);


    cout << "Merge completed successfully.\n";
    return 0;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        cerr << "Usage: merge <branch-name>\n";
        return 1;
    }

    return merge(argv[1]);
}