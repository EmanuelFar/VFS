#ifndef RCFSTREAM_HPP
#define RCFSTREAM_HPP
#include <string>
#include <fstream>

// RCfstream implements a reference-counted file stream
class RCfstream {
	public:
		// Construct with file name
		RCfstream(const char* file_name);
		// Copy and assignment handle reference counting - increment count on copy,
		// decrement and delete if count reaches 0
		RCfstream(const RCfstream& other);
		RCfstream& operator=(const RCfstream& other);
		~RCfstream();

		char operator[](int index)const;
		char& operator[](int index);


		// Changes timestamp in case file exists, else creates one.
		static void ftouch(const char* file_name);
		// if destination dir doesnt exist, it will be created.	
		static void copy(const RCfstream& source, RCfstream& destination);
		void cat()const;		
		void wc()const;		

		// Get current reference count
		int getRC()const{return desc_->ref_count;}

	private:
		// Shared file descriptor with reference counting
		struct FileDesc {
			int ref_count;
			std::fstream file;
			std::string content;
			FileDesc(const char* file_name);
			~FileDesc() = default;
		};

		FileDesc* desc_;
		std::string file_name_;

};


#endif // RCFSTREAM_HPP
