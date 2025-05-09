#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "RCfstream.hpp"
#include <unordered_map>
#include <string>

// Directory class implements a node in a directory tree structure
// Each directory can contain files and subdirectories
class Directory {
	public:
		// Construct directory with given name
		explicit Directory(const char* dir_name):dir_name_(dir_name){}
		// Directories are explicitly created and cannot be copied
		Directory(const Directory& other)=delete;
		Directory& operator=(const Directory& other)=delete;
		~Directory();
		
		void mkdir(const char* dir_name);
		Directory* chdir(const char* dir_name);
		void rmdir(const char* dir_name);
		void ls()const;
		void lproot()const; 
		void pwd()const;

		// Get directory name
		const std::string& getName()const;

		void linkFile(const char* file_name, const RCfstream& file);
		void registerFile(const char* file_name);    
		void removeFile(const char* file_name);      

		bool fileExists(const char* file_name);
		bool dirExists(const char* file_name);
		
		RCfstream& getFile(const char* file_name);
		Directory* getDir(const char* dir_name);

	private:
		std::unordered_map<std::string, RCfstream> files_;  // Files in directory
		std::unordered_map<std::string, Directory*> dirs_;  // Subdirectories
		std::string dir_name_;
};

#endif // DIRECTORY_H
