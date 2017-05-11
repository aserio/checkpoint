// Adrian Serio
// April 28, 2017
//
// This example creats a class which can load a file and can create 
// a new file that is a copy of the old one. This example uses HPX
// and HPXIO.

#include <file.hpp>

int main (int argc, char* argv[]){

 return hpx::init(argc, argv);
}

int hpx_main() {
 
 size_t count;
 off_t offset=17;
 
 //Test 1
 File file("test.txt");
 if(file.is_open()) {
  hpx::cout<<"I am open!"<<std::endl;
 }
 count=file.data.size();
 hpx::cout<<"Size of buffer: "<<count<<std::endl;
 file.print();
 file.save("test2.txt");
 
 //Test 2
 File file2("test2.txt", count);
 file2.print();
 file2.save("test3.txt");
 
 //Test 3
 File file3("test3.txt", count, offset);
 file3.print();
 hpx::cout<<file3.file_name<<std::endl;
 file3.save();
 file3.remove_file();
 
 hpx::finalize();
}
