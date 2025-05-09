Emanuel Fares

---- EX1 ----

A virtual file system (VFS) with a command-line interface. The implementation consists of three main components:

1. RCfstream (Reference Counted File Stream as we were taught in lectures):
-> Implements a reference-counted file stream that allows multiple objects to share the same file descriptor
- Uses a FileDesc struct to manage file content and reference counting
- Provides basic file operations like read, write, cat, and wc
-> Pay attention that files content are stored in a local string

2. Directory:
-> Uses unordered_map for efficient file and directory lookup...-
-... <directory_path, directory*> , <file_path, RCfstream>
.
.
-> Directory Represents a node in the directory tree structure
- Manages both files (RCfstream objects) and subdirectories
- Implements directory operations like mkdir, rmdir, chdir, and ls

3. Terminal:
- Provides a command-line interface to interact with the VFS
- Implements a REPL (Read-Eval-Print Loop) for command processing
- Supports various file and directory operations
-> Handles path processing and validation
-> Maintains a cursor to track current directory and root.

Besides Memory management through reference counting, I focused on 2 more concepts:
-> Efficient data structures for file and directory management
-> Clean separation of concerns between components

26/4/2025
