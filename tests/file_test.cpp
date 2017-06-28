// Copyright (c) 2017 Adrian Serio
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// This example creats a class which can load a file and can create 
// a new file that is a copy of the old one. This example uses HPX
// and HPXIO.

#include <hpx/hpx_main.hpp>
#include <hpxio_file.hpp>

int main() {

 //Set up testing infrastucture
 size_t pass_counter=0;
 size_t num_tests=3;

 size_t count;
 off_t offset=17;
 std::string file_name="test.txt";
 
 //Test 1 - Create a file, fill it, save it, save a new copy, and close file
 hpxio_file file(file_name);
 std::vector<char> vec_char={'T','e','s','t',' ','t','e','x','t','\n'};
 for (int i=0; i<vec_char.size(); i++) {
  file.data.push_back(vec_char[i]);
 }
 file.save();
 count=file.data.size();
 file.save("test2.txt");
 file.close();

 if (access(file_name.c_str(), F_OK) ==0 &&
     file.file_handle_read.is_open(hpx::launch::sync) ) {
   pass_counter++;
 }
 
 //Test 2 - Test constructor with count
 hpxio_file file2("test2.txt", count);
 file2.save("test3.txt");
 
 //Test 3 - Test that all is working correctly
 hpxio_file file3("test3.txt");
 hpxio_file file4("test.txt");
 
 if(file4.data == file3.data && file3.data == file2.data) {
  pass_counter+=2;
 }
 file3.save();
 
 //Clean up
 file2.remove_file();
 file3.remove_file();
 file4.remove_file();
 
 if (pass_counter == num_tests) { 
  hpx::cout<<"All File Tests Pass!"<<std::endl; 
 } 
 else { 
  hpx::cout<<(num_tests - pass_counter)<<" of "<<num_tests
           <<" File Tests Failed!"<<std::endl; 
 }
 
 return 0;
}
