// Copyright (c) 2017 Adrian Serio
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// This example tests the store and resurrect functions for their ability to 
// checkpoint and application.
// 

#include <hpx/hpx_main.hpp>
#include <hpxio_file.hpp>
#include <checkpoint.hpp>

// Main 
int main() {

 char character='d';
 char character2;
 int integer=10;
 int integer2;
 float flt=10.01;
 float flt2;
 bool boolean=true; 
 bool boolean2; 
 std::string str="I am a string of characters";
 std::string str2;
 std::vector<char> vec(str.begin(),str.end());
 std::vector<char> vec2;
// std::vector<char> archive;
 checkpoint<> archive;
 checkpoint<> archive2;
 
 //Test 1
 hpx::cout<<"Test 1:"<<std::endl;

 //Print out initial variables
 hpx::cout<<"Initial Variables"<<std::endl
          <<" char= "<<character<<std::endl
          <<" int= "<<integer<<std::endl
          <<" float= "<<flt<<std::endl
          <<" bool= "<<boolean<<std::endl
          <<" string= "<<str<<std::endl
          <<" vector<char>= ";
 for (int i=0; i<vec.size(); i++) {
  if (vec.size() != i+1 ) hpx::cout<<vec[i];
  else hpx::cout<<vec[i]<<std::endl;
 }
 
 store(archive, character, integer, flt, boolean, str, vec);
 resurrect(archive, character2, integer2, flt2, boolean2, str2, vec2);
 
 //Print out resurected variables 
 hpx::cout<<"Resurrected Variables"<<std::endl
          <<" char= "<<character2<<std::endl
          <<" int= "<<integer2<<std::endl
          <<" float= "<<flt2<<std::endl
          <<" bool= "<<boolean2<<std::endl
          <<" string= "<<str2<<std::endl
          <<" vector<char>= ";
 for (int i=0; i<vec2.size(); i++) {
  if (vec2.size() != i+1 ) hpx::cout<<vec2[i];
  else hpx::cout<<vec2[i]<<std::endl;
 }

 if (character == character2 && integer == integer2 && flt == flt2
      && boolean == boolean2 && str == str2 
      && std::equal(vec.begin(), vec.end(), vec2.begin())){
  hpx::cout<<"I work!"<<std::endl;
 }

 //Test 2
 hpx::cout<<"Test 2:"<<std::endl;
 
 archive2=archive;
 resurrect(archive, character2, integer2, flt2, boolean2, str2, vec2);
 
 if (character == character2 && integer == integer2 && flt == flt2
      && boolean == boolean2 && str == str2 
      && std::equal(vec.begin(), vec.end(), vec2.begin())){
  hpx::cout<<"I work!"<<std::endl;
 }

 //Test 3
 hpx::cout<<"Test 3:"<<std::endl;
 
 checkpoint<> archive3;
 store(archive3, vec, integer);
 resurrect(archive3, vec2, integer2);
 
 if (std::equal(vec.begin(), vec.end(), vec2.begin()) && integer==integer2){
  hpx::cout<<"I work!"<<std::endl;
 }

 //Test 4
 hpx::cout<<"Test 4"<<std::endl;
 hpxio_file omg("omg.txt");
// std::vector<char> char_buff("OMG! This is a test!");
 std::vector<char> char_buff={'O', 'M', 'G', '!', ' ','T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 't', 'e', 's', 't', '!'};
 omg.data=char_buff;
 omg.print(); 
 omg.write();

 //Test 5
 checkpoint<hpxio_file> archive4("test4.archive");
 std::vector<int> test_vec;
 for (int c=0; c<101; c++) {
  test_vec.push_back(c);
 }
 store(archive4, test_vec);
 archive4.data.write();
 
 //Test 6    
 hpx::cout<<"Test 6"<<std::endl;
 std::vector<int> test_vec2;
 resurrect(archive4, test_vec2);
 if (std::equal(test_vec.begin(), test_vec.end(), 
          test_vec2.begin())){
  hpx::cout<<"I work!"<<std::endl;
 }
 
 return 0;
}
