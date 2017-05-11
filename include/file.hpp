// Adrian Serio
// April 28, 2017
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
#include <hpx/hpx_main.hpp>
#include <hpx/include/iostreams.hpp>

#include <hpxio/server/local_file.hpp>
#include <hpxio/base_file.hpp>

class File {
 public:
  File (std::string file_name_arg); // Load a file
  File (std::string file_name_arg, size_t count); // Load a file
  File (std::string file_name_arg, size_t count, off_t offset); // Load a file
  
  std::string file_name;
  hpx::io::base_file file_handle_read;
  hpx::io::base_file file_handle_write;
  off_t position;
  hpx::serialization::serialize_buffer<char> data; 
   
  bool is_open();
  void write();
  void write(off_t const offset);
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
 std::ifstream temp_handle (file_name_arg);
 if (temp_handle) {
  temp_handle.seekg(0, temp_handle.end);
  count=temp_handle.tellg();
  temp_handle.close();
 }
 else std::cerr<<"Error: No file found!"<<std::endl;
 
 //Instantiate Read and Write handles
 file_handle_read = hpx::new_<hpx::io::server::local_file>(hpx::find_here());
 file_handle_write = hpx::new_<hpx::io::server::local_file>(hpx::find_here());
 
 //Open handles
 file_handle_read.open(hpx::launch::sync, file_name_arg, O_RDONLY);
 file_handle_write.open(hpx::launch::sync, file_name_arg, O_WRONLY);
 
 if(file_handle_read.is_open(hpx::launch::sync)) {
  file_name=file_name_arg;
  data=file_handle_read.read(hpx::launch::sync, count);
 }
 else std::cerr<<"Error: No file found!"<<std::endl;
}

File::File(std::string file_name_arg, size_t count) {
 
 //Instantiate Read and Write handles
 file_handle_read = hpx::new_<hpx::io::server::local_file>(hpx::find_here());
 file_handle_write = hpx::new_<hpx::io::server::local_file>(hpx::find_here());
 
 //Open handles
 file_handle_read.open(hpx::launch::sync, file_name_arg, O_RDONLY);
 file_handle_write.open(hpx::launch::sync, file_name_arg, O_WRONLY);
 
 if(file_handle_read.is_open(hpx::launch::sync)) {
  file_name=file_name_arg;
  data=file_handle_read.read(hpx::launch::sync, count);
 }
 else std::cerr<<"Error: No file found!"<<std::endl;
}

File::File(std::string file_name_arg, size_t count, off_t offset) {
 
 //Instantiate Read and Write handles
 file_handle_read = hpx::new_<hpx::io::server::local_file>(hpx::find_here());
 file_handle_write = hpx::new_<hpx::io::server::local_file>(hpx::find_here());
 
 //Open handles
 file_handle_read.open(hpx::launch::sync, file_name_arg, O_RDONLY);
 file_handle_write.open(hpx::launch::sync, file_name_arg, O_RDONLY);
 
 if(file_handle_read.is_open(hpx::launch::sync)) {
  file_name=file_name_arg;
  data=file_handle_read.pread(hpx::launch::sync, count, offset);
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

// write()
void File::write() {
 file_handle_write.write(hpx::launch::sync, data);
}

void File::write(off_t const offset) {
 file_handle_write.pwrite(hpx::launch::sync, data, offset);
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
 file_handle_write.write(hpx::launch::sync, data);
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
   file_handle_write.open(hpx::launch::sync, file_name, O_WRONLY | O_CREAT);
   if(file_handle_write.is_open(hpx::launch::sync)) {
    file_handle_write.write(hpx::launch::sync, data);
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
   new_file_handle.write(hpx::launch::sync, data);  
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
