// Adrian Serio
// April 28, 2017
//
// This example creats a class which can load a file and can create 
// a new file that is a copy of the old one. This example uses HPX
// and HPXIO.


#include <iostream>
#include <fstream>
#include <future>
#include <string>
#include <vector>

#include <hpx/hpx.hpp>
#include <hpx/hpx_main.hpp>
#include <hpx/include/iostreams.hpp>

#include <hpxio/server/local_file.hpp>
#include <hpxio/base_file.hpp>

class File {
 public:
  std::vector<std::string> file_buffer;
  hpx::serialization::serialize_buffer<char> data; 
  void print(); // Print out the contents of file_buffer
  void attach(std::string file_name); // Load a file
  void attach(std::string file_name, size_t count); // Load a file
  void attach(std::string file_name, size_t count, off_t offset); // Load a file
  void copy(std::string new_file_name); // Make a new copy of a file
};

void File::attach(std::string file_name) {
 size_t count;
 std::ifstream temp_handle (file_name);
 if (temp_handle) {
  temp_handle.seekg(0, temp_handle.end);
  count=temp_handle.tellg();
  temp_handle.close();
 }
 else std::cerr<<"No file found!"<<std::endl;
 hpx::io::base_file file_handle =
  hpx::new_<hpx::io::server::local_file>(hpx::find_here());
 file_handle.open(hpx::launch::sync, file_name, O_RDONLY);
 data=file_handle.read(hpx::launch::sync, count);
 file_handle.close();
}

void File::attach(std::string file_name, size_t count) {
 hpx::io::base_file file_handle =
  hpx::new_<hpx::io::server::local_file>(hpx::find_here());
 file_handle.open(hpx::launch::sync, file_name, O_RDONLY);
 data=file_handle.read(hpx::launch::sync, count);
 file_handle.close();
}

void File::attach(std::string file_name, size_t count, off_t offset) {
 hpx::io::base_file file_handle =
  hpx::new_<hpx::io::server::local_file>(hpx::find_here());
 file_handle.open(hpx::launch::sync, file_name, O_RDONLY);
  data=file_handle.pread(hpx::launch::sync, count, offset);
 file_handle.close();
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

int main (int argc, char* argv[]){

 return hpx::init(argc, argv);
}

int hpx_main() {
 
 size_t count;
 
 //Test 1
 File file;
 file.attach("test.txt");
 count=file.data.size();
 hpx::cout<<"Size of buffer: "<<count<<std::endl;
 file.print();
 file.copy("test2.txt");
 
 //Test 2
 File file2;
 file2.attach("test2.txt", count);
 file2.print();

 hpx::finalize();
}
