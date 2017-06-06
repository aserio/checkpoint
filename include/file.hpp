// Copyright (c) 2017 Adrian Serio
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// file.hpp describes a class which encapsulates a file accessed via HPXIO
// This class takes a file name and optionally takes the number of 
// bytes to read from a file and an offset to start reading from.
// Additionally the File class can make a new file from the old one.
//
// My next steps are to:
//    Fix hang!
//    create an = overload
//    Create a checkpoint?

#include <hpx/hpx.hpp>
#include <hpx/include/iostreams.hpp>

#include <hpxio/server/local_file.hpp>
#include <hpxio/base_file.hpp>

class File {
 public:
  typedef hpx::serialization::serialize_buffer<char> buffer_type; 
  File (std::string file_name_arg); // Load a file
  File (std::string file_name_arg, size_t count); // Load a file
  File (std::string file_name_arg, size_t count, off_t offset); // Load a file
  
  std::string file_name;
  hpx::io::base_file file_handle_read;
  hpx::io::base_file file_handle_write;
  off_t position;
//  hpx::serialization::serialize_buffer<char> buffer; 
  std::vector<char> data; 
   
  bool is_open();
//  ssize_t size();                   ADD AFTER YOU UPDATE PXFS
  size_t size_data();
  void resize_data(size_t count);
  void write();
  void write(off_t const offset);
  void write_data(void* address, size_t count, size_t current);
  void read_data(void* address, size_t count, size_t current);
  void lseek(int whence); // Seek a point in the file
  void lseek(off_t offset, int whence); // Seek a point in the file
  void save (); // Write data to file
  void save(std::string new_file_name); // Write to a possibly new file  
  void print(); // Print out the contents of file_buffer
  void remove_file(); // Delete file 
  
  ~File () {
   file_handle_read.close();
   file_handle_write.close();
  }
};

// Constructors
File::File(std::string file_name_arg) {
 size_t count;
 //Instantiate Read and Write handles
 file_handle_read = hpx::new_<hpx::io::server::local_file>(hpx::find_here());
 file_handle_write = hpx::new_<hpx::io::server::local_file>(hpx::find_here());

 if (access(file_name_arg.c_str(), F_OK) == 0) { 
   std::ifstream temp_handle (file_name_arg);
   if (temp_handle) {
    //Find the size of the file
    temp_handle.seekg(0, temp_handle.end);
    count=temp_handle.tellg();
    temp_handle.close();
   } 
   else std::cerr<<"Error: File not Accessed!"<<std::endl;
 
  //Open handles
  file_handle_read.open(hpx::launch::sync, file_name_arg, O_RDONLY);
  file_handle_write.open(hpx::launch::sync, file_name_arg, O_WRONLY);
  
  //Get file size                        Add after you update PXFS
//  count = file_handle_read.size();

  //Read in file
  if(file_handle_read.is_open(hpx::launch::sync)) {
   file_name=file_name_arg;
   data.resize(count);
   buffer_type buffer(data.data(), data.size(), buffer_type::reference); 
   buffer=file_handle_read.read(hpx::launch::sync, count);
   hpx::cout<<buffer[0]<<buffer[1]<<buffer[2]<<std::endl;
   hpx::cout<<data[0]<<data[1]<<data[2]<<std::endl;
  }
  else std::cerr<<"Error: No file found!"<<std::endl;
 }
 else {
  //Create file and open handles
  file_handle_read.open(hpx::launch::sync, file_name_arg, O_RDONLY | O_CREAT);
  file_handle_write.open(hpx::launch::sync, file_name_arg, O_WRONLY);
 }
}

File::File(std::string file_name_arg, size_t count) {
 
 //Instantiate Read and Write handles
 file_handle_read = hpx::new_<hpx::io::server::local_file>(hpx::find_here());
 file_handle_write = hpx::new_<hpx::io::server::local_file>(hpx::find_here());
 
 //Open handles
 file_handle_read.open(hpx::launch::sync, file_name_arg, O_RDONLY);
 file_handle_write.open(hpx::launch::sync, file_name_arg, O_WRONLY);
 
 //Read in file
 if(file_handle_read.is_open(hpx::launch::sync)) {
  file_name=file_name_arg;
  buffer_type buffer(data.data(),data.size(),buffer_type::reference); 
  buffer=file_handle_read.read(hpx::launch::sync, count);
 }
 else std::cerr<<"Error: No file found!"<<std::endl;
}

File::File(std::string file_name_arg, size_t count, off_t offset) {
 
 //Instantiate Read and Write handles
 file_handle_read = hpx::new_<hpx::io::server::local_file>(hpx::find_here());
 file_handle_write = hpx::new_<hpx::io::server::local_file>(hpx::find_here());
 
 //Open handles
 file_handle_read.open(hpx::launch::sync, file_name_arg, O_RDONLY);
 file_handle_write.open(hpx::launch::sync, file_name_arg, O_WRONLY);
 
 //Read in file
 if(file_handle_read.is_open(hpx::launch::sync)) {
  file_name=file_name_arg;
  buffer_type buffer(data.data(),data.size(),buffer_type::reference); 
  buffer=file_handle_read.pread(hpx::launch::sync, count, offset);
 }
 else std::cerr<<"Error: No file found!"<<std::endl;
}

//is_open()
bool File::is_open(){
 if(file_handle_read.is_open(hpx::launch::sync) &&
    file_handle_write.is_open(hpx::launch::sync) ) {
  return true;
 }
 else false;
}

//size()                                 ADD AFTER YOU UPDATE PXFS
//ssize_t File::size() {
// ssize_t size;
// file_handle_read.size(hpx::launch::sync);
// return size;
//}

size_t File::size_data() {
 return data.size();
}

//resize_data()
void File::resize_data(std::size_t count) {
 return data.resize(data.size() + count);
} 

// write()
void File::write() {
 
 buffer_type buffer(data.data(),data.size(),buffer_type::reference); 
// file_handle_write.write(hpx::launch::sync, data);
 file_handle_write.write(hpx::launch::sync, buffer);
}

void File::write(off_t const offset) {
 buffer_type buffer(data.data(),data.size(),buffer_type::reference); 
// file_handle_write.pwrite(hpx::launch::sync, data, offset);
 file_handle_write.pwrite(hpx::launch::sync, buffer, offset);
}

void File::write_data(void* address, size_t count, size_t current) {
 std::memcpy(address, &data[current], count);
}

//read()
void File::read_data(void* address, size_t count, size_t current) {
  std::memcpy(&data[current], address, count);
}

//lseek()
void File::lseek(int whence) {
 position = file_handle_read.lseek(hpx::launch::sync, 0, whence);
 file_handle_write.lseek(hpx::launch::sync, 0, whence);
}

void File::lseek(off_t offset, int whence) {
 position = file_handle_read.lseek(hpx::launch::sync, offset, whence);
 file_handle_write.lseek(hpx::launch::sync, offset, whence);
}

//save()
void File::save() {
 buffer_type buffer(data.data(),data.size(),buffer_type::reference); 
 file_handle_write.write(hpx::launch::sync, buffer);
}

void File::save(std::string new_file_name) {
 if (data.size() == 0) {
  std::cerr<<"Error: No data to copy!"<<std::endl;
 }
 else if (new_file_name == file_name) {
   //Close and delete current file
   file_handle_write.close();
   file_handle_read.remove_file(hpx::launch::sync, file_name);
   //Re-create and open
   file_handle_read.open(hpx::launch::sync, file_name, O_RDONLY | O_CREAT);
   file_handle_write.open(hpx::launch::sync, file_name, O_WRONLY);
   if(file_handle_write.is_open(hpx::launch::sync)) {
    buffer_type buffer(data.data(),data.size(),buffer_type::reference); 
    file_handle_write.write(hpx::launch::sync, buffer);
   }
   else std::cerr<<"Error: File not created!"<<std::endl;
 }
 else {
  hpx::io::base_file new_file_handle =
   hpx::new_<hpx::io::server::local_file>(hpx::find_here());
  new_file_handle.open(hpx::launch::sync, 
                       new_file_name, 
                       O_WRONLY | O_CREAT);
  if(new_file_handle.is_open(hpx::launch::sync)) {
   buffer_type buffer(data.data(),data.size(),buffer_type::reference); 
   new_file_handle.write(hpx::launch::sync, buffer);  
  }
  else std::cerr<<"Error: File not created!"<<std::endl;
  new_file_handle.close();
 }
}

//print()
void File::print() {
  hpx::cout<<data.data()<<std::endl;
}

//remove_file)
void File::remove_file() {
 file_handle_write.close();
 file_handle_read.remove_file(hpx::launch::sync, file_name);
}

namespace hpx { namespace traits 
 {
  template<>
  struct serialization_access_data<File>
   : default_serialization_access_data<File>
  {
   static std::size_t size(File& cont)
   {
    return cont.size_data();
   }
   
   static void resize(File& cont, std::size_t count)
   {
    cont.resize_data(count);
   }

   static void write(File& cont, std::size_t count, std::size_t current,
                     void* address)
   {
    cont.write_data(address, count, current);
   }
   
   // functions related to input operations   
   static void read(File& cont, std::size_t count, std::size_t current,
                    void* address)
   {
    cont.read_data(address, count, current);
   }
  };
 }
}
