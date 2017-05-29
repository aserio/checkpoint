// Copyright (c) 2017 Adrian Serio
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// This header defines the store and resurrect functions. These functions
// are designed to help HPX application developers checkpoint thier
// applications. Store serializes an object and saves it as a
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
//    - Pass a component to store
//    - Add tests
//
// TRYING TO GET A COMPONENT TO TAKE TEMPLATES D:  D:  D:

#include <hpx/include/serialization.hpp>
#include <hpx/include/components.hpp>

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
template<typename T = std::vector<char>>
struct Checkpoint {
 T data;
 int size() const { return data.size();}
 void resize(int n) { data.resize(n); }
 void operator=(T v) {
 data = v;
 }
 char& operator[](int i) { return data[i]; }
 const char& operator[](int i) const { return data[i]; }
};

/*
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

/////// Registrations to include in .cpp file //////// 
//HPX_REGISTER_COMPONENT(hpx::components::simple_component<checkpoint>,
//                        checkpoint );
//HPX_REGISTER_ACTION(checkpoint::hi_action, test);
///////////////////////////////////////////////////////////
*/
