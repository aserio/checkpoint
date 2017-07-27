// Copyright (c) 2017 Adrian Serio
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// This example tests the save_checkpoint and restore_checkpoint functions for their ability to 
// checkpoint and application.
// 

#include <hpx/hpx_main.hpp>
#include <hpxio_file.hpp>
#include <checkpoint.hpp>

// Main 
int main() {

 //Set up testing infrastructure
 size_t pass_counter=0;
 size_t num_tests=3;
 
 //Initialize variables
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
 checkpoint<> archive;
 checkpoint<> archive2;
 
 //Test 1 - Store arbitrary variables
/*
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
*/
 
 save_checkpoint(archive, character, integer, flt, boolean, str, vec);
 restore_checkpoint(archive, character2, integer2, flt2, boolean2, str2, vec2);

 //Print out resurected variables 
/*
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
*/

 if (character == character2 && integer == integer2 && flt == flt2
      && boolean == boolean2 && str == str2 
      && std::equal(vec.begin(), vec.end(), vec2.begin())){
  pass_counter++;
 }

 //Test 2 - Test equal assignment
 //hpx::cout<<"Test 2:"<<std::endl;
 
 archive2=archive;
 restore_checkpoint(archive, character2, integer2, flt2, boolean2, str2, vec2);
 
 if (character == character2 && integer == integer2 && flt == flt2
      && boolean == boolean2 && str == str2 
      && std::equal(vec.begin(), vec.end(), vec2.begin())){
  pass_counter++;
 }

 //Test 3 - Test checkpoint<hpxio_file>
 //hpx::cout<<"Test 3:"<<std::endl;
 
 checkpoint<hpxio_file> archive3("archive3.archive");
 std::vector<char> vec_char={'T', 'e', 's', 't', ' ', 't', 'e', 'x', 't', '.'};
 std::vector<char> vec_char2;
 std::vector<int>  vec_int={1,2,3,4,5};
 std::vector<int>  vec_int2;
 save_checkpoint(archive3, vec_char, vec_int);
 restore_checkpoint(archive3, vec_char2, vec_int2);
 
 if (std::equal(vec_char.begin(), vec_char.end(), vec_char2.begin()) 
      && std::equal(vec_int.begin(), vec_int.end(), vec_int2.begin())){
  pass_counter++;
 }
 
 //Clean up
 archive3.data.remove_file();
 
 //Report results of test
 if ( pass_counter == num_tests) { 
  hpx::cout<<"All Checkpoint Test Pass!"<<std::endl;
 }
 else {
  hpx::cout<<(num_tests - pass_counter)<<" of "<<num_tests
           <<" Checkpoint Tests Failed!"<<std::endl;
 }
 
 return 0;
}
