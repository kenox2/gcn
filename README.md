
# GNC - Git-Inspired Version Control System

**GNC** is a lightweight version control system heavily inspired by Git. Built as a pet project, this system mimics Git’s core functionality, offering multiple commands for managing files, branches, commits, and merges — all written in C++ with a line count of ~1600-1700 across modular components.

---

## 🧱 Project Structure

The project is composed of several small programs, each representing a command. All commands except `merge` are implemented as separate programs. Shared logic and utilities are grouped under helper modules.

### Internal Directory Structure (after `init`):
```
/<your_project_dir>
├── .gnc/                  # Root VCS folder (hidden)
│   ├── objects/           # All objects stored by hash
│   │   └── <hash_prefix>/<hash_suffix>  
│   ├── INDEX              # Index file for staging changes
│   ├── refs/              # Reference directory
│   │   └── heads/         # Holds heads of branches
│   ├── HEAD               # Points to the current branch
│   ├── user_data          # Stores username and email
```

---

## 📦 Implemented Commands

- ### `init <username> <email>`
  Initializes the VCS structure and sets up necessary directories and metadata files.

- ### `add <dir_path>`
  Adds all files in the specified directory to the staging area (index).  
  ⚠️ **Currently does not support adding individual files.**

- ### `commit <message>`
  Creates a commit based on the current index. Updates the current branch head if one is active.

- ### `switch <branch_name | commit_hash>`
  Recreates the state of a specific branch or commit in the working directory.  
  ⚠️ Uncommitted and unstaged files will be **lost**.

- ### `branch <branch_name>`
  Creates a new branch from the current commit.

- ### `branch_display`
  Lists all branches.

- ### `branch_rm <branch_name>`
  Deletes a specified branch.

- ### `merge <branch_name>`
  Merges changes from the given branch into the current one. If a conflict is detected, the merge is aborted and conflicts are reported for manual resolution.

- ### `log <branch_name | all>`
  Displays commit history for a branch or all branches in topological order (via DFS reverse postorder).

- ### `print_head`
  Prints the current HEAD commit or branch.

---

## 🧬 File Types

All internal files are stored in **binary** format for efficiency:

### 🔹 **Commit Files**
- Store metadata:
  ```
  <mode> <tree_hash>
  parent: <hash> (one or more)
  author: <username>
  message: <commit_message>
  ```

### 🔹 **Tree Files**
- Represent directory structures. Each entry contains:
  ```
  <32-bit mode><filename><64-bit hash>
  ```

### 🔹 **Blob Files**
- Compressed file contents using `zlib`.
- Stored in chunks. Initially read into memory for hashing (via `xxh64`), then compressed and saved.

### 🔹 **Index Files**
- Temporary staging areas for changes.
- Format starts with a 4-byte entry count, followed by tree-like entries.

---

## 🔐 Hashing Strategy

All content is hashed using the [xxHash](https://github.com/Cyan4973/xxHash) library (`xxh64`), chosen for its speed and simplicity.

- First **2 characters** of the hash = **directory name**
- Remaining characters = **filename**

This structure prevents performance issues related to large numbers of files in a single directory.

---

## 🚧 Future Improvements

- Add support for individual file staging.
- Enhance merge conflict resolution.
- Implement diff and status commands.

---

## 🤖 Disclaimer

This is a learning project and not intended for production use. It mimics Git behavior at a high level but omits many safety features, edge-case handling, and performance optimizations.

---

## 📜 License

Feel free to fork, copy, or extend — it's a pet project made for fun and learning.
