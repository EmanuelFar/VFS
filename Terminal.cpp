#include "Terminal.hpp"
#include <sstream>
#include <string>
#include <cstring>
#include <iostream>
#include <map>
#include <functional>


Terminal::Terminal(const char* root_dir_name){
	root_ = new Directory(root_dir_name);
	cursor_ = root_;

	root_name_ = root_dir_name;
}

Terminal::~Terminal(){
	delete root_;
}


////////////////////////////////////////
////////////////////////////////////////
// Terminal Command Line Implementation.
//
// 

void Terminal::read(const char* path, int position) {
    std::string full_path;
    Directory* original_cursor;
    
    if (!processFilePath(path, full_path, original_cursor)) {
        return;
    }

    const char* c_full_path = full_path.c_str();

    if (cursor_->fileExists(c_full_path)) {
        RCfstream& file = cursor_->getFile(c_full_path);
        char c;
        try {
            c = file[position];
        } catch(std::out_of_range& ex) {
            std::cout << ex.what() << '\n';
            cursor_ = original_cursor;
            return;
        }
        std::cout << '\n' << c << '\n';
    } else {
        std::cerr << "ERROR: File Doesn't Exist!\n";
    }
    cursor_ = original_cursor;
}

void Terminal::write(const char* path, int position, char c) {
    std::string full_path;
    Directory* original_cursor;
    
    if (!processFilePath(path, full_path, original_cursor)) {
        return;
    }

    const char* c_full_path = full_path.c_str();

    if (cursor_->fileExists(c_full_path)) {
        RCfstream& file = cursor_->getFile(c_full_path);
        try {
            file[position] = c;
        } catch(std::out_of_range& ex) {
            std::cout << ex.what() << '\n';
            cursor_ = original_cursor;
            return;
        }
    } else {
        std::cerr << "ERROR: File Doesn't Exist!\n";
    }
    cursor_ = original_cursor;
}

void Terminal::touch(const char* path) {
    std::string full_path;
    Directory* original_cursor;
    
    if (!processFilePath(path, full_path, original_cursor)) {
        return;
    }

    const char* c_full_path = full_path.c_str();

    if (!cursor_->fileExists(c_full_path)) {
        cursor_->registerFile(c_full_path);
    }

    RCfstream::ftouch(c_full_path);
    cursor_ = original_cursor;
}


void Terminal::copy(const char* src_path, const char* target_path) {
    if (!isValidFile(src_path)) {
        std::cerr << "ERROR: Wrong Source File!\n";
        return;
    }

    if (!isValidFile(target_path)) {
        std::cerr << "ERROR: Wrong Target File!\n";
        return;
    }

    Directory* original_cursor = cursor_;
    RCfstream* src_file = nullptr;
    // Process source path
    std::string full_src_path;
    if (!processFilePath(src_path, full_src_path, original_cursor)) {
        return;
    }

    // Get source file
    if (cursor_->fileExists(full_src_path.c_str())) {
        src_file = &cursor_->getFile(full_src_path.c_str());
    } else {
        std::cerr << "ERROR: Source File Doesn't Exist!\n";
        cursor_ = original_cursor;
        return;
    }

    // Process target path
    std::string full_target_path;
    if (!processFilePath(target_path, full_target_path, original_cursor)) {
        cursor_ = original_cursor;
        return;
    }

    // Ensure target file exists and get reference to it
    if (!cursor_->fileExists(full_target_path.c_str())) {
        cursor_->registerFile(full_target_path.c_str());
    }
    RCfstream& target_file = cursor_->getFile(full_target_path.c_str());

    // Perform the copy operation
    RCfstream::copy(*src_file, target_file);
    cursor_ = original_cursor;
}

void Terminal::remove(const char* path){
    std::string full_path;
    Directory* original_cursor;
    
    if (!processFilePath(path, full_path, original_cursor)) {
        return;
    }

    const char* c_full_path = full_path.c_str();
    if (cursor_->fileExists(c_full_path)) {
        cursor_->removeFile(c_full_path);
    }

    cursor_ = original_cursor;
}

void Terminal:: move(const char* src_path, const char* target_path){
	copy(src_path, target_path);
	remove(src_path);
}

void Terminal::cat(const char* path) {
    std::string full_path;
    Directory* original_cursor;
    
    if (!processFilePath(path, full_path, original_cursor)) {
        return;
    }

    const char* c_full_path = full_path.c_str();
    if (cursor_->fileExists(c_full_path)) {
        RCfstream& file = cursor_->getFile(c_full_path);
        file.cat();
    } else {
        std::cerr << "ERROR: File Doesn't Exist!\n";
    }
    cursor_ = original_cursor;
}

void Terminal::wc(const char* path) {
    std::string full_path;
    Directory* original_cursor;
    
    if (!processFilePath(path, full_path, original_cursor)) {
        return;
    }

    const char* c_full_path = full_path.c_str();
    if (cursor_->fileExists(c_full_path)) {
        RCfstream& file = cursor_->getFile(c_full_path);
        file.wc();
    } else {
        std::cerr << "ERROR: File Doesn't Exist!\n";
    }
    cursor_ = original_cursor;
}

void Terminal::ln(const char* src_path, const char* link_name) {
    if (!isValidFile(src_path)) {
        std::cerr << "ERROR: Wrong Source File!\n";
        return;
    }

    if (!isValidFile(link_name)) {
        std::cerr << "ERROR: Wrong Link Name!\n";
        return;
    }

    Directory* original_cursor = cursor_;

    // Process source path
    std::string full_src_path;
    if (!processFilePath(src_path, full_src_path, original_cursor)) {
        return;
    }

    // Get source file
    RCfstream& src_file = cursor_->getFile(full_src_path.c_str());
    cursor_ = original_cursor;

    // Process target path
    std::string full_link_path;
    if (!processFilePath(link_name, full_link_path, original_cursor)) {
        cursor_ = original_cursor;
        return;
    }

    // if file already exists - can't create a link!
    if (cursor_->fileExists(full_link_path.c_str())) {
        std::cerr << "ERROR: Link name already exists!\n";
        cursor_ = original_cursor;
        return;
    }

    cursor_->linkFile(full_link_path.c_str(), src_file);
    cursor_ = original_cursor;
}

void Terminal::mkdir(const char* path){
	if (!isValidDir(path)){
		std::cerr << "ERROR: Not a Valid Path!\n";
		return;	
	}
	const char* parent_path = parseParentDir(path);
	try{
		moveCursor(parent_path);
	} catch(std::runtime_error& ex){
		std::cerr << ex.what() << '\n';
		delete parent_path;
		return;	
	}
	// call Directory::mkdir
	cursor_->mkdir(path);

	delete parent_path;
}

void Terminal::chdir(const char* path){
	if (!isValidDir(path)){
		std::cerr << "ERROR: Not a Valid Path!\n";
		return;
	}
	try{
		moveCursor(path);
	} catch(std::runtime_error& ex){
		std::cerr << ex.what() << '\n';
	}
}

void Terminal::rmdir(const char* path){
	std::string str_path = path;
	if (path == root_name_){
		std::cerr << "ERROR: Cannot remove root directory!\n";
		return;	
	}
	if (!isValidDir(path)){
		std::cerr << "ERROR: Not a Valid Path!\n";
		return;	
	}
	const char* parent_path = parseParentDir(path);
	try{
		moveCursor(parent_path);
	} catch(std::runtime_error& ex){
		std::cerr << ex.what() << '\n';
		delete parent_path;
		return;	
	}
	// call Directory::rmdir
	cursor_->rmdir(path);

	delete parent_path;
}

void Terminal::ls(const char* path){
	Directory* original_pos = cursor_;
	try{
		moveCursor(path);
		cursor_->ls();
	} catch(std::runtime_error& ex){
		std::cerr << ex.what() << '\n';
	}
	cursor_ = original_pos;
}

void Terminal::lproot()const{
	root_->lproot();
}

void Terminal::pwd()const{
	cursor_->pwd();
}


////////////////////////////////////////
////////////////////////////////////////

/***** Utils *****/

// This function moves cursor to the correct directory and returns full path.
bool Terminal::processFilePath(const char* path, std::string& full_path, Directory*& original_cursor) {
    if (!isValidFile(path)) {
        std::cerr << "ERROR: Wrong file Path!\n";
        return false;
    }

    original_cursor = cursor_;

    if (isAbsolutePath(path)) {
        full_path = path;
        const char* parent_path = parseParentDir(path);
        try {
            moveCursor(parent_path);
        } catch(std::runtime_error& ex) {
            std::cerr << ex.what() << '\n';
            delete parent_path;
            cursor_ = original_cursor;
            return false;
        }
        delete parent_path;
    } else {
        full_path = getFullFilePath(path);
    }

    return true;
}

std::string Terminal::getFullFilePath(const char* path) {
    // If path is already absolute, return it as is
    if (isAbsolutePath(path)) {
        return std::string(path);
    }
    // Otherwise, concatenate current directory path with relative path
    return cursor_->getName() + std::string(path);
}

const char* Terminal::parseParentDir(const char* path){
	std::string str_path(path);
	int len = str_path.size();

	// dir path ends with / - it must be skipped.
	int iter = len - 2;
	for (; len > 0 && str_path[iter] != '/'; iter--);
	iter++;

	char* parent_path = new char[iter + 1];
	std::strncpy(parent_path, path, iter);
	parent_path[iter] = '\0';
	return parent_path;
}

bool Terminal::isValidDir(const char* dir_path){
	std::string str_p(dir_path);
	int last_idx = str_p.size() - 1;
	return str_p[last_idx] == '/' && str_p.rfind(root_name_,0) == 0;
}

bool Terminal::isValidFile(const char* path){
	std::string str_p(path);
	int last_idx = str_p.size() - 1;
	// dir path given
	if (str_p[last_idx] == '/'){
		return false;
	}
	return true;				
}

bool Terminal::isAbsolutePath(const char* path){
	std::string str_p(path);
	return str_p.rfind(root_name_,0) == 0;
}

void Terminal::moveCursor(const char* dir_path){
	// save cursor in case path is wrong
	Directory* original_pos = root_;
	cursor_ = root_;
	std::string str_dir_path(dir_path);
	std::istringstream ss(str_dir_path);
	std::string tk;
	std::getline(ss, tk, '/');
	tk += '/';
	if (tk != root_name_){
		cursor_ = original_pos;
		throw std::runtime_error("ERROR: Path doesn't exist!");
	}
	while (std::getline(ss, tk, '/')) {
		// create next dir name in path
		std::string next_path = cursor_->getName();
		next_path += tk + "/";
		// check if dir exists in current path
		if (cursor_->dirExists(next_path.c_str())){
			cursor_ = cursor_->chdir(next_path.c_str());	
		} else{
			cursor_ = original_pos;
			throw std::runtime_error("ERROR: Path doesn't exist!");
		}
	}
}

void Terminal::start() {
    std::string line;
    while (true) {
        std::cout << "[input] ";
        std::getline(std::cin, line);
        
        if (line == "exit") {
            std::cout << "      [terminal  closed] \n";
            break;
        }

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        
        
        if (cmd == "read") {
            std::string filename;
            int position;
            iss >> filename >> position;
            read(filename.c_str(), position);
        }
        else if (cmd == "write") {
            std::string filename;
            int position;
            char c;
            iss >> filename >> position >> c;
            write(filename.c_str(), position, c);
        }
        else if (cmd == "touch") {
            std::string filename;
            iss >> filename;
            touch(filename.c_str());
        }
        else if (cmd == "copy") {
            std::string src, target;
            iss >> src >> target;
            copy(src.c_str(), target.c_str());
        }
        else if (cmd == "remove") {
            std::string filename;
            iss >> filename;
            remove(filename.c_str());
        }
        else if (cmd == "move") {
            std::string src, target;
            iss >> src >> target;
            move(src.c_str(), target.c_str());
        }
        else if (cmd == "cat") {
            std::string filename;
            iss >> filename;
            cat(filename.c_str());
        }
        else if (cmd == "wc") {
            std::string filename;
            iss >> filename;
            wc(filename.c_str());
        }
        else if (cmd == "ln") {
            std::string target, link;
            iss >> target >> link;
            ln(target.c_str(), link.c_str());
        }
        else if (cmd == "mkdir") {
            std::string dirname;
            iss >> dirname;
            mkdir(dirname.c_str());
        }
        else if (cmd == "chdir") {
            std::string dirname;
            iss >> dirname;
            chdir(dirname.c_str());
        }
        else if (cmd == "rmdir") {
            std::string dirname;
            iss >> dirname;
            rmdir(dirname.c_str());
        }
        else if (cmd == "ls") {
            std::string dirname;
            iss >> dirname;
            ls(dirname.c_str());
        }
        else if (cmd == "lproot") {
            lproot();
        }
        else if (cmd == "pwd") {
            pwd();
        }
        else {
            std::cout << "Unknown command: " << cmd << '\n';
        }
    }
}

