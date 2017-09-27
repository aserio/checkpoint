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
// which you wish to store.
//
// Resurrect takes the checkpoint object and the container
// which to store the objects in the byte stream (in the same order
// as they were placed in).
//
// Checkpoint is the container object which must be passed to save_checkpoint 
// and restore_checkpoint.

#if !defined(CHECKPOINT_HPP_07262017)
#define CHECKPOINT_HPP_07262017

#include <type_traits>

#include <hpx/include/components.hpp>
#include <hpx/include/serialization.hpp>

#include <hpx/util/range.hpp>

#include <cstddef>
#include <fstream>
#include <iosfwd>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace checkpoint_ns
{

// Forward declarations
class checkpoint;
std::ostream& operator<<(std::ostream& ost, checkpoint const& ckp);
std::istream& operator>>(std::istream& ist, checkpoint& ckp);
namespace detail
{
    struct save_funct_obj;
}

    //Checkpoint Object
    class checkpoint
    {
        std::vector<char> data;

        friend std::ostream& operator<<(std::ostream& ost, checkpoint const& ckp);
        friend std::istream& operator>>(std::istream& ist, checkpoint& ckp);
        //Serialization Definition
        friend class hpx::serialization::access;
        template <typename Archive>
        void serialize(Archive& arch, const unsigned int version)
        {
            arch& data;
        };
        friend struct detail::save_funct_obj;
        template <typename T, typename... Ts>
        friend void restore_checkpoint(checkpoint const& c, T& t, Ts& ... ts);

    public:
        checkpoint() = default;
        checkpoint(checkpoint const& c)
          : data(c.data)
        {
        }
        checkpoint(checkpoint&& c)
         : data(std::move(c.data))
        {
        }
        ~checkpoint() = default;

        //Other Constructors
        checkpoint(char* stream, std::size_t count)
        {
            for (std::size_t i=0;i<count;i++)
            {
                data.push_back(*stream);
                stream++;
            }
        }

        checkpoint& operator=(checkpoint const& c)
        {
            if (&c != this)
            {
                data = c.data;
            }
            return *this;
        }
        checkpoint& operator=(checkpoint&& c)
        {
            if (&c != this)
            {
                data = std::move(c.data);
            }
            return *this;
        }

        bool operator==(checkpoint const& c) const
        {
            return data == c.data;
        }
        bool operator!=(checkpoint const& c) const
        {
            return !(data == c.data);
        }
        
        // Expose iterators to access data held by checkpoint
        using const_iterator = std::vector<char>::const_iterator;
        const_iterator begin() const
        {
            return data.begin();
        }
        const_iterator end() const
        {
            return data.end();
        }
/*
        void load(std::string file_name)
        {
            std::ifstream ifs(file_name);
            if(ifs)                               //Check fstream is open
            {
                ifs.seekg(0, ifs.end);
                int length = ifs.tellg();         //Get length of file
                ifs.seekg(0, ifs.beg);
                data.resize(length);
                ifs.read(data.data(), length);
            }
        }
*/
        size_t size() const
        {
            return data.size();
        }

    };

    //Stream Overloads
    std::ostream& operator<<(std::ostream& ost, checkpoint const& ckp)
    {
        // Write the size of the checkpoint to the file
        int64_t size = ckp.size();
        ost.write(reinterpret_cast<char const *>(&size), sizeof(int64_t));
        // Write the file to the stream
        ost.write(ckp.data.data(), ckp.size());
        return ost;
    }
    std::istream& operator>>(std::istream& ist, checkpoint& ckp)
    {
        // Read in the size of the next checkpoint
        int64_t length;
        ist.read(reinterpret_cast<char *>(&length), sizeof(int64_t));
        ckp.data.resize(length);
        // Read in the next checkpoint
        ist.read(ckp.data.data(), length);
        return ist;
    }

    //Function object for save_checkpoint
    namespace detail
    {
        struct save_funct_obj
     {
         template <typename... Ts>
         checkpoint operator()(checkpoint&& c, Ts&&... ts) const
         {
             //Create serialization archive from checkpoint data member
             hpx::serialization::output_archive ar(c.data);
             //Serialize data
             int const sequencer[] = { //Trick to expand the variable pack
                 (ar << ts, 0)...};    //Takes advantage of the comma operator
             (void)sequencer;           // Suppress unused param. warnings
             return c;
         }
     };
    }

    //Save_checkpoint function 
    template <typename T
            , typename... Ts
            , typename U = typename std::enable_if<
                    !hpx::traits::is_launch_policy<T>::value && 
                    !std::is_same<typename std::decay<T>::type,checkpoint>::value
                >::type
             >
    hpx::future<checkpoint> save_checkpoint(T&& t, Ts&&... ts)
    {
        {
            return hpx::dataflow(
                detail::save_funct_obj()
              , std::move(checkpoint())
              , std::forward<T>(t)
              , std::forward<Ts>(ts)...);
        }
    }
    
    //Save_checkpoint function - Take a pre-initialized checkpoint
    template <typename T, typename... Ts>
    hpx::future<checkpoint> save_checkpoint(checkpoint&& c
        , T&& t
        , Ts&&... ts)
    {
        {
            return hpx::dataflow(
                detail::save_funct_obj()
              , std::move(c)
              , std::forward<T>(t)
              , std::forward<Ts>(ts)...);
        }
    }
    
    
    //Store function - Policy overload
    template <typename T, typename... Ts>
    hpx::future<checkpoint> save_checkpoint(
          hpx::launch p
        , T&& t
        , Ts&&... ts)
    {
        {
            return hpx::dataflow(
                  p
                , detail::save_funct_obj()
                , std::move(checkpoint())
                , std::forward<T>(t)
                , std::forward<Ts>(ts)...);
        }
    }

    //Store function - Policy overload & pre-initialized checkpoint
    template <typename T, typename... Ts>
    hpx::future<checkpoint> save_checkpoint(
          hpx::launch p
        , checkpoint&& c
        , T&& t
        , Ts&&... ts)
    {
        {
            return hpx::dataflow(
                  p
                , detail::save_funct_obj()
                , std::move(c)
                , std::forward<T>(t)
                , std::forward<Ts>(ts)...);
        }
    }

    //Save_checkpoint function - With sync policy
    template <typename T
            , typename... Ts
            , typename U = typename std::enable_if<
                    !std::is_same<typename std::decay<T>::type,checkpoint>::value
                >::type
             >
    checkpoint save_checkpoint(
          hpx::launch::sync_policy sync_p
        , T&& t
        , Ts&&... ts)
    {
        {
            hpx::future<checkpoint> f_chk = 
                hpx::dataflow(
                      sync_p
                    , detail::save_funct_obj()
                    , std::move(checkpoint())
                    , std::forward<T>(t)
                    , std::forward<Ts>(ts)...);
            return f_chk.get();
        }
    }

    //Save_checkpoint function - With sync policy & pre-init. checkpoint
    template <typename T, typename... Ts>
    checkpoint save_checkpoint(
          hpx::launch::sync_policy sync_p
        , checkpoint&& c
        , T&& t
        , Ts&&... ts)
    {
        {
            hpx::future<checkpoint> f_chk = 
                hpx::dataflow(
                      sync_p
                    , detail::save_funct_obj()
                    , std::move(c)
                    , std::forward<T>(t)
                    , std::forward<Ts>(ts)...);
            return f_chk.get();
        }
    }

    //Resurrect Function
    template <typename T, typename... Ts>
    void restore_checkpoint(checkpoint const& c, T& t, Ts& ... ts)
    {
        {
            //Create seriaalization archive
            hpx::serialization::input_archive ar(c.data, c.size());
    
            //De-serialize data
            ar >> t;
            int const sequencer[] = {//Trick to exand the variable pack
               (ar >> ts, 0)...};    //Takes advantage of the comma operator
            (void)sequencer;           // Suppress unused param. warnings
        }
    }
}

using checkpoint_ns::checkpoint;
using checkpoint_ns::save_checkpoint;
using checkpoint_ns::restore_checkpoint;

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
