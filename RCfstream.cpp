#include "RCfstream.hpp"
#include <iostream>
#include <sstream> 

RCfstream::RCfstream(const char* file_name){
	desc_ = new FileDesc(file_name);
	file_name_ = std::string(file_name);
}

/*****          Big Three         *****/

// Each one of three functions carries out RC handling.
RCfstream::RCfstream(const RCfstream& other){
	// shallow copy of desc_
	desc_ = other.desc_;
	file_name_ = other.file_name_;

	++(desc_->ref_count);
}

RCfstream& RCfstream::operator=(const RCfstream& other){
	if (file_name_ == other.file_name_) return *this;
	if (--(desc_->ref_count) == 0) delete desc_;

	desc_ = other.desc_;
	(desc_->ref_count)++;
	file_name_ = other.file_name_;
	
	return *this;
}

RCfstream::~RCfstream(){
	if (--(desc_->ref_count) == 0) delete desc_;
}
////////////////////////////////////////
////////////////////////////////////////

char RCfstream::operator[](int index) const{
	if (0 < index || index >= static_cast<int>(desc_->content.size())){
		throw std::out_of_range("ERROR: Read index out of range\n");
	}
	return desc_->content[index];
}

char& RCfstream::operator[](int index) {
    if (index < 0 || index > static_cast<int>(desc_->content.size())) {
        throw std::out_of_range("ERROR: Write index out of range\n");
    }

    if (index == static_cast<int>(desc_->content.size())) {
	// Append dummy char
        desc_->content += "0"; 
    }

    return desc_->content[index];
}

////////////////////////////////////////
////////////////////////////////////////

void RCfstream::ftouch(const char* file_name){
	std::ofstream ofile;
	ofile.open(file_name);
	ofile.flush();
	ofile.close();
}


void RCfstream::copy(const RCfstream& source, RCfstream& destination){
	destination.desc_->content = source.desc_->content;
}

void RCfstream::cat()const{
	std::cout << desc_->content << '\n';
}

void RCfstream::wc()const{
	
	int lines = 0, words = 0, chrs = 0;
	std::istringstream file(desc_->content);
	std::string buf;
	while (std::getline(file, buf)){
		++lines;
		chrs += buf.size();

		std::istringstream buf_sstream(buf);
		std::string word;
		while (buf_sstream >> word) ++words;
	}
	std::cout << lines << " " << words << " " << chrs << " " << file_name_<< '\n';	
}


////////////////////////////////////////
////////////////////////////////////////
// RC struct implementation.
// @ref_count -  number of objects pointing to file
// @file_name - file name for identity purposes

RCfstream::FileDesc::FileDesc(const char* file_name){
	ref_count = 1;
	file = std::fstream(file_name);
	content = "";
}
