#ifndef TERMINAL_H
#define TERMINAL_H

#include "Directory.hpp"
#include <string>
#include <map>
#include <functional>

// Terminal class implements a virtual file system with a command-line interface
// It manages a directory tree structure and provides file operations
class Terminal {
	public:
		// Terminal is explicitly constructed with a root directory name
		explicit Terminal(const char* root_dir_name);
		Terminal(const Terminal& other)=delete;
		Terminal& operator=(const Terminal& other)=delete;
		~Terminal();

		// File operations
		void read(const char* path, int position);    
		void write(const char* path, int position, char c);  
		void touch(const char* path);                
		void copy(const char* src_path, const char* target_path);  
		void remove(const char* path);               
		void move(const char* src_path, const char* target_path);  
		void cat(const char* path);                  
		void wc(const char* path);                   
		void ln(const char* src_path, const char* link_name);  

		// Directory operations
		void mkdir(const char* path);               
		void chdir(const char* path);               
		void rmdir(const char* path);               
		void ls(const char* path);                  
		void lproot()const;                         
		void pwd()const;                            
		
		// Start the terminal REPL (Read-Eval-Print Loop)
		void start();

	private:
		std::string root_name_;
		Directory* root_;
		Directory* cursor_;

		// Helper method to process file path and move cursor
		// Returns false if path is invalid, true otherwise
		// original_cursor is restored if path processing fails
		bool processFilePath(const char* path, std::string& full_path, Directory*& original_cursor);

		// Moves cursor to a desired directory
		// Throws runtime_error if path doesn't exist
		void moveCursor(const char* dir_path);
		
		// Given an absolute path - returns parent directory path
		// Caller must delete returned string
		const char* parseParentDir(const char* path);

		// Returns full file path relative to current directory
		std::string getFullFilePath(const char* path);
		
		// Path validation helpers
		bool isValidDir(const char* dir_path);
		bool isValidFile(const char* path);
		bool isAbsolutePath(const char* path);
};

#endif // TERMINAL_H
