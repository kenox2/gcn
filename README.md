# GCN – Git Clone, Nearly

GCN is a minimal, low-level version control system inspired by Git. It handles blobs, trees, commits, branching, and merging using binary formats and hashed content. The project is implemented in C++ and is intended as a learning tool to explore how Git-like systems work under the hood.

---

## 📁 Project Structure

- `programs/` – CLI commands like `add`, `commit`, `switch`, etc.
- `utils/` – Shared logic for hashing, reading/writing, traversing, etc.
- `libs/xxhash/` – Fast, non-cryptographic hash function (used for content hashing)
- `gcn` – Bash wrapper script to use commands like `gcn add`, `gcn commit`, etc.
- `compile.sh` – Shell script to compile the project modularly


## 🧱 Repository Structure

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

## 🛠️ Setup Instructions

1. **Clone the repository**
   ```bash
   git clone https://github.com/yourusername/gcn.git
   cd gcn
   ```

2. **Compile the project**
   ```bash
   ./compile.sh
   ```

   Or compile specific commands:
   ```bash
   ./compile.sh add commit switch
   ```

3. **Make the `gcn` wrapper globally available**

   You can add the project directory to your `PATH`, or symlink `gcn` to `/usr/local/bin`:

   ```bash
   echo 'export PATH="$PATH:/path/to/gcn"' >> ~/.bashrc
   source ~/.bashrc
   ```

   Or:

   ```bash
   sudo ln -s "$(pwd)/gcn" /usr/local/bin/gcn
   ```

---

## 🚀 Available Commands

| Command                           | Description                                                           |
|-----------------------------------|-----------------------------------------------------------------------|
| `gcn init <username> <email>`     | Initialize a new GCN repo and store user identity                     |
| `gcn add <dir_path>`              | Stage all files in a directory. Does not handle single files (yet)    |
| `gcn commit <message>`            | Commit staged changes with a message                                  |
| `gcn switch <branch or hash>`     | Switch to a branch or commit (first 4+ digits of hash supported)      |
| `gcn branch [<rm>][<branch_name>]`| Display all existing branches                                         |                                            
| `gcn merge <branch_name>`         | Merge given branch into current one. Conflicts abort the operation    |
| `gcn log <branch_name or all>`    | Show logs in topological order (via DFS + postorder traversal)        |
| `gcn print_head`                  | Show the current HEAD (commit or branch)                              |

> 💡 `gcn help` will show the most commonly used commands.

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

## 🔧 Development Notes

- Each command is compiled as a separate binary under `programs/`
- Shared code is located under `utils/`
- `compile.sh` supports compiling all or individual binaries
- Binaries are excluded from the repository for portability
- Project supports both **Linux** and **Windows** (tested via Git Bash/WSL)

---

## 📦 Optional Releases

For convenience, precompiled binaries for common platforms may be published under GitHub Releases in the future.

---

## 🐧 Requirements

- Linux or Windows (Git Bash / WSL)
- g++ 11+
- Bash
- zlib development library (`libz-dev` on Linux)
