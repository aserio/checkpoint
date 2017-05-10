// Adrian Serio
// April 28, 2017
//
// file.hpp describes a class which encapsulates a file accessed via HPXIO
// This class takes a file name and optionally takes the number of 
// bytes to read from a file and an offset to start reading from.
// Additionally the File class can make a new file from the old one. 

//#include <string>
//#include <vector>

#include <hpx/hpx.hpp>
#include <hpx/hpx_main.hpp>
#include <hpx/include/iostreams.hpp>

#include <hpxio/server/local_file.hpp>
#include <hpxio/base_file.hpp>

class File {
 public:
  hpx::io::base_file file_handle;
  hpx::serialization::serialize_buffer<char> data; 
  File (std::string file_name); // Load a file
  File (std::string file_name, size_t count); // Load a file
  File (std::string file_name, size_t count, off_t offset); // Load a file
  void copy(std::string new_file_name); // Make a new copy of a file
  void print(); // Print out the contents of file_buffer
  ~File () {
   file_handle.close();
  }
};

File::File(std::string file_name) {
 size_t count;
 std::ifstream temp_handle (file_name);
 if (temp_handle) {
  temp_handle.seekg(0, temp_handle.end);
  count=temp_handle.tellg();
  temp_handle.close();
 }
 else std::cerr<<"No file found!"<<std::endl;
 file_handle = hpx::new_<hpx::io::server::local_file>(hpx::find_here());
 file_handle.open(hpx::launch::sync, file_name, O_RDONLY);
 data=file_handle.read(hpx::launch::sync, count);
}

File::File(std::string file_name, size_t count) {
 file_handle = hpx::new_<hpx::io::server::local_file>(hpx::find_here());
 file_handle.open(hpx::launch::sync, file_name, O_RDONLY);
 data=file_handle.read(hpx::launch::sync, count);
}

File::File(std::string file_name, size_t count, off_t offset) {
 file_handle = hpx::new_<hpx::io::server::local_file>(hpx::find_here());
 file_handle.open(hpx::launch::sync, file_name, O_RDONLY);
  data=file_handle.pread(hpx::launch::sync, count, offset);
}

void File::copy(std::string new_file_name) {
 if (data.size() == 0) {
  std::cerr<<"No file to copy!"<<std::endl;
 }
 else {
  hpx::io::base_file new_file_handle =
   hpx::new_<hpx::io::server::local_file>(hpx::find_here());
  new_file_handle.open(hpx::launch::sync, 
                       new_file_name, 
                       O_WRONLY | O_CREAT);
  new_file_handle.write(hpx::launch::sync, data);  
  new_file_handle.close();
 }
}

void File::print() {
 hpx::cout<<data.data()<<std::endl;
}
