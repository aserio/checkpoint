// Adrian Serio
// April 28, 2017
//
// This example creats a class which can load a file and can create 
// a new file that is a copy of the old one.


#include <iostream>
#include <fstream>
#include <future>
#include <string>
#include <vector>

#include <hpx/hpx_main.hpp>
#include <hpx/include/iostreams.hpp>
/*
class File {
 public:
  std::vector<std::string> file_buffer;
  void print(); // Print out the contents of file_buffer
  void attach(std::string file_name); // Load a file
  void copy(std::string new_file_name); // Make a new copy of a file
};

void File::attach(std::string file_name) {
 std::ifstream file_handle;
 file_handle.open(file_name,std::fstream::in);
 std::string buff;
 while (std::getline(file_handle, buff)){
  buff.append("\n");
  file_buffer.push_back(buff);
 }
 file_handle.close();
}

void File::copy(std::string new_file_name) {
 if (file_buffer.size() == 0) {
  std::cerr<<"No file to copy!"<<std::endl;
 }
 else {
  std::ofstream new_file_handle;
  new_file_handle.open(new_file_name, std::ofstream::out);
  for (int i=0; i<file_buffer.size(); i++)
   new_file_handle<<file_buffer[i];
  new_file_handle.close();
 }
}

void File::print() {
 for (int i=0; i<file_buffer.size(); i++) {
  std::cout<<file_buffer[i];
 };
 std::cout<<std::endl;
}

int main (int argc, char* argv[]){
// std::cout<<"Hi"<<std::endl;
// std::future<int> f;
// f = std::async(std::launch::async, []{return 8;});
// std::cout<<f.get()<<std::endl;
 File file;
 file.attach("test.txt");
 file.print();
 file.copy("test2.txt");
 File file2;
 file2.attach("test2.txt");
 file2.print();

 return hpx::init(argc, argv);
}

int hpx_main() {
 hpx::cout<<"Hello_World"<<std::endl;
 hpx::finalize();
}
*/
int main (int argc, char* argv[])
{
 return hpx::init(argc, argv);
}

int hpx_main ()
{
 hpx::cout<<"Hello World"<<std::endl;
 return 0;
}
