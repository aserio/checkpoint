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
 
 //Test 1
 File file("test.txt");
 if(file.is_open()) {
  hpx::cout<<"I am open!"<<std::endl;
 }
 count=file.data.size();
 hpx::cout<<"Size of buffer: "<<count<<std::endl;
 file.print();
 file.copy("test2.txt");
 
 //Test 2
 File file2("test2.txt", count);
 file2.print();

 hpx::finalize();
}
