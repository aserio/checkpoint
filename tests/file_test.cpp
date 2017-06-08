// Copyright (c) 2017 Adrian Serio
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// This example creats a class which can load a file and can create 
// a new file that is a copy of the old one. This example uses HPX
// and HPXIO.

#include <hpx/hpx_main.hpp>
#include <file.hpp>

int main() {

 size_t count;
 off_t offset=17;
 
 //Test 1 - Create a file, fill it, save it, and save a new copy
 File file("test.txt");
 std::vector<char> vec_char={'T','e','s','t',' ','t','e','x','t','\n'};
 for (int i=0; i<vec_char.size(); i++) {
  file.data.push_back(vec_char[i]);
 }
 file.save();
 count=file.data.size();
 file.save("test2.txt");
 
 //Test 2 - Test constructor with count
 File file2("test2.txt", count);
 file2.save("test3.txt");
 
 //Test 3 - Test that all is working correctly
 File file3("test3.txt");
 File file4("test.txt");
 
 if(file.data == file3.data) {
  hpx::cout<<"I work!"<<std::endl;
 }
 file3.save();
 
 //Clean up
 file.remove_file();
 file2.remove_file();
 file3.remove_file();
 
 return 0;
}
