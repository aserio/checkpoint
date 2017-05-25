// Copyright (c) 2017 Adrian Serio
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// This example tests the store function for its ability to 
// checkpoint and application.
// 
// Store is designed to serialize an object and save it as a
// byte stream. Resurrect converts the byte stream back into the 
// object.
// 
// Store takes a container and any number of objects
// which you wish to store. To use the provided 
// checkpoint object the user must ensure that the
// container exposes size() and resize().
//
// Resurrect takes the checkpoint object and the container
// which to store the objects in the byte stream (in the same order
// as they were placed in).
//
// Checkpoint is a container object which can be passed to store
// 
// To-Do:
//    - Create a type Checkpoint to manage data stream
//    - Wrap serialized data in a component?
//       -> Return a GID
//    - Add tests
//    - Create as a header file
//
// TRYING TO GET A COMPONENT TO TAKE TEMPLATES D:  D:  D:

#include <hpx/hpx_main.hpp>
#include <hpx/include/serialization.hpp>
#include <hpx/include/components.hpp>
#include <file.hpp>

//Checkpoint function - improved
template <typename C, typename ...T>
void store (C& c, T&& ...t) {
 {
  hpx::serialization::output_archive ar(c);
  int const sequencer[]= {  //Trick to expand the variable pack
   (ar<<t, 0)...};          //Takes advantage of the comma operator
 }
}

//Resurrect Function - Improved
template <typename C, typename ...T>
void resurrect (C& c, T& ...t) {
 {
  hpx::serialization::input_archive ar(c, c.size());
  int const sequencer[]= {  //Trick to exand the variable pack
   (ar >> t, 0)...};        //Takes advantage of the comma operator
 } 
}

//Checkpoint Object
//template<typename T = std::vector<char>>
//struct Checkpoint {
// T data;
// int size() const { return data.size();}
// void resize(int n) { data.resize(n); }
// void operator=(T v) {
// data = std::move(v);
// }
// char& operator[](int i) { return data[i]; }
// const char& operator[](int i) const { return data[i]; }
//};

/////////// Checkpoint Component ///////////////

//Server
//template<typename T = std::vector<char>>
struct checkpoint
 : hpx::components::simple_component_base<checkpoint>
{
 using T=std::vector<char>;
 T data;

 int size() const { return data.size(); }
 void resize(int n) { data.resize(n); }
 void operator=(T v) {
  data=std::move(v);
 }
 char& operator[](int i) { return data[i]; }
 const char& operator[](int i) const { return data[i]; }

 int hi() { 
  hpx::cout<<"I made it!"<<std::endl;
  int hi_int=10;
  hpx::cout<<"Do I still work"<<std::endl;
  return hi_int;
 }
 
 HPX_DEFINE_COMPONENT_ACTION(checkpoint, hi, hi_action);
};

HPX_REGISTER_ACTION_DECLARATION(checkpoint::hi_action, test)

//Client
struct checkpoint_client
 : hpx::components::client_base<checkpoint_client, checkpoint>
{
 using base_type = hpx::components::client_base<checkpoint_client, checkpoint>;
 
 checkpoint_client(hpx::future<hpx::id_type> && id)
  : base_type(std::move(id)) 
 {}

 checkpoint::hi_action act;
 
 hpx::future<int> hi() { return hpx::async(act, get_id()); }
};

/////// Registrations //////// 
HPX_REGISTER_COMPONENT(hpx::components::simple_component<checkpoint>,
                        checkpoint );
HPX_REGISTER_ACTION(checkpoint::hi_action, test);

// Main 
int main() {
 hpx::id_type here = hpx::find_here();
 checkpoint_client bin = hpx::new_<checkpoint_client>(here);

 auto f =  bin.hi();
 int z = f.get();
 
 hpx::cout<<z<<" Bye!"<<std::endl;
/*
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
 std::vector<char> archive;
 Checkpoint<> archive2;
 
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
 
// archive=store(character, integer, flt, boolean, str, vec);
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
 
 Checkpoint<> archive3;
 store(archive3, vec, integer);
 resurrect(archive3, vec2, integer2);
 
 if (std::equal(vec.begin(), vec.end(), vec2.begin()) && integer==integer2){
  hpx::cout<<"I work!"<<std::endl;
 }
*/
 return 0;
}
