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
 
 //Test 1
 File file("test.txt");
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
 
 return 0;
}
