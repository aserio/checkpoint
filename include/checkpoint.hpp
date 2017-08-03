// Copyright (c) 2017 Adrian Serio
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// This header defines the save_checkpoint and restore_checkpoint functions. These functions
// are designed to help HPX application developers checkpoint thier
// applications. Store serializes an object and saves it as a
// byte stream. Resurrect converts the byte stream back into the 
// object.
// 
// Store takes a type 'checkpoint' and any number of objects
// which you wish to store. Users can create a custom type to 
// pass as a template argument to a checkpoint object. The 
// user must take care to ensure that the created type exposes
// the proper traits as required by serialization.
//
// Resurrect takes the checkpoint object and the container
// which to store the objects in the byte stream (in the same order
// as they were placed in).
//
// Checkpoint is the container object which must be passed to save_checkpoint.
// Checkpoint takes a user supplied type to store the data in. 
// By default checkpoint uses std::vector<char>.
// 
// To-Do:
//    - Wrap serialized data in a component?
//       -> Return a GID
//    - Pass a component to save_checkpoint
//

#if !defined(CHECKPOINT_HPP_07262017)
#define CHECKPOINT_HPP_07262017

#include <type_traits>

#include <hpx/include/serialization.hpp>
#include <hpx/include/components.hpp>

//Checkpoint Traits
template <typename T>
struct can_write
  : std::false_type
{}; // Create an template specialization to expose a write function

//Checkpoint Object
template<typename T = std::vector<char>>
struct checkpoint {
  checkpoint() {}
  checkpoint(checkpoint const & c) : data(c.data) {} 
  checkpoint(checkpoint && c) : data(std::move(c.data)) {} 
  ~checkpoint(){}

  //Constructors
  // the enable if is needed to force the compiler to use the move constructor 
  // if passing a type 'checkpoint'
  template <typename T_, typename U = 
    typename std::enable_if<
      !std::is_same<checkpoint<T>, typename std::decay<T_>::type>::value>::type>
  explicit checkpoint(T_ && t)
   : data(std::forward<T_>(t)) 
   {} //Pass args to data structure constr. if first arg is not type checkpoint

  template <typename T1, typename T2, typename ... Ts>
  explicit checkpoint(T1 && t1, T2 && t2, Ts &&... ts) 
   : data(std::forward<T1>(t1), std::forward<T2>(t2), std::forward<Ts>(ts)...)
  {} //Pass args to data structure constuctor

  T data;
  
  //Serialization Definition
  friend class hpx::serialization::access;
  template<typename Volume>
  void serialize(Volume& vol, const unsigned int version) {
   vol & data;
  };
 
  checkpoint& operator= (checkpoint const & c) 
  {
    data=c.data;
  }
  checkpoint& operator= (checkpoint && c) 
  {
    data=std::move(c.data);
  }
   
  void write() 
  {
    static_assert(can_write<T>::value,
      "No write function (or can_write trait) is implemented for this type.");
      data.write();
  }

};

//Store function
template <typename C, typename ...T>
void save_checkpoint (checkpoint<C>& c, T&& ...t) {
 {
  //Create serialization archive from checkpoint data member
  hpx::serialization::output_archive ar(c.data);
  
  //Serialize data
  int const sequencer[]= {  //Trick to expand the variable pack
   (ar << t, 0)...};          //Takes advantage of the comma operator
 }
}

//Store function - With futures! 
template <typename C, typename ...T>
void save_checkpoint_future (checkpoint<C>& c, T&& ...t) {
  {
    hpx::future<int> f=hpx::make_ready_future(5);
    hpx::future<void> f2 = f.then([](hpx::future<int> i){
                    std::cout<<"I am functioning!"<<std::endl;
                  });
    f2.get();
/*    hpx::future<checkpoint<>> f =  hpx::dataflow(
                 hpx::launch::async, 
                 [](checkpoint<C>& c, T&& ...t){
                    //Create serialization archive from checkpoint data member
                    hpx::serialization::output_archive ar(c.data);
                    //Serialize data
                    int const sequencer[]= {  //Trick to expand the variable pack
                    (ar << t, 0)...}; }, //Takes advantage of the comma operator
                    std::cout<<"I am functioning!"<<std::endl; 
                    return hpx::make_ready_future(c);
                 },
                 std::forward<T>(t)...);
    f.get();
*/
  }
}

//Resurrect Function
template <typename C, typename ...T>
void restore_checkpoint (checkpoint<C> const& c, T& ...t) {
 {
  //Create seriaalization archive
  hpx::serialization::input_archive ar(c.data, 
      hpx::traits::serialization_access_data<C>::size(c.data));//Get the size of the container
  
  //De-serialize data
  int const sequencer[]= {  //Trick to exand the variable pack
   (ar >> t, 0)...};        //Takes advantage of the comma operator
 } 
}


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

#endif

