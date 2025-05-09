#include "Directory.hpp"
#include <iostream>
#include <queue>


Directory::~Directory(){
	for (auto& iter: dirs_) {
		if (iter.first != "..") delete iter.second;
	}
	dirs_.clear();
	files_.clear();
}

void Directory::mkdir(const char* dir_name){
	std::string str_name = dir_name;
	dirs_[str_name] = new Directory(dir_name);
	Directory* new_dir = dirs_[str_name];
	// Link parent dir to new dir
	new_dir->dirs_[".."] = this;
}

Directory* Directory::chdir(const char* dir_name){
	std::string str_name = dir_name;
	return dirs_[str_name];		
}

void Directory::rmdir(const char* dir_name){
	std::string str_name = dir_name;
	delete dirs_[str_name];
	dirs_.erase(str_name);		
}

void Directory::ls() const{
	std::cout << dir_name_ << ":\n";
	for (auto& iter: files_){
		std::cout << iter.first << " ";
	}
	for (auto& iter: dirs_){
		if (iter.first != "..")
		std::cout << iter.first << " ";
	}
	std::cout << '\n';
}



void Directory::pwd()const{
	std::cout << dir_name_ << '\n';	
}


const std::string& Directory::getName()const{return dir_name_;}	

void Directory::lproot()const{
	std::queue<const Directory*> bfs_q;

	bfs_q.push(this);

	while (!bfs_q.empty()) {
		const Directory* current_dir = bfs_q.front();
		bfs_q.pop();

		std::cout << current_dir->dir_name_ + ":" << '\n';

		for (auto& dir_entry : current_dir->dirs_){
			if (dir_entry.first != "..") bfs_q.push(dir_entry.second);
		}
		for (auto& file_entry : current_dir->files_){
			std::cout << file_entry.first << " " << file_entry.second.getRC() << '\n';
		}
	}
}


/*****  Utils *****/

void Directory::registerFile(const char* file_name){
			std::string entry(file_name);
			files_.insert({entry, RCfstream(file_name)});
}

void Directory::linkFile(const char* file_name, const RCfstream& file){
			std::string entry(file_name);
			files_.insert({entry, file}); 
}

void Directory::removeFile(const char* file_name){
	std::string entry(file_name);
	files_.erase(entry);

}


bool Directory::fileExists(const char* file_name){
	std::string entry(file_name);
	if (files_.find(entry) != files_.end()){
		return true;
	}
	return false;
}

bool Directory::dirExists(const char* file_name){
	std::string entry(file_name);
	if (dirs_.find(entry) != dirs_.end()){
		return true;
	}
	return false;
}

RCfstream& Directory::getFile(const char* file_name){
	std::string entry(file_name);
	return files_.at(entry);
}

Directory* Directory::getDir(const char* dir_name){
	std::string entry(dir_name);
	return dirs_.at(entry);
}
