// Adrian Serio
// April 28, 2017
//
// This example test the checkpooint function.
// Checkpoint is designed to serialize an object and save it as a
// byte stream. Resurrect converts the byte stream back into the 
// object.

#include <hpx/hpx_main.hpp>
#include <hpx/include/serialization.hpp>
#include <file.hpp>

template <typename T>
std::vector<char>  checkpoint (T const& t) {
 std::vector<char> v;
 {
  hpx::serialization::output_archive ar(v);
  ar<<t;
 }
 return v;
}

template <typename T>
void resurrect (std::vector<char> v, T& t) {
 {
  hpx::serialization::input_archive ar(v, v.size());
  ar >> t;
 } 
}

int main() {
 
 std::string str="I am a string of characters";
 std::vector<char> vec(str.begin(),str.end());
 std::vector<char> vec2;
 std::vector<char> archive;

 for (int i=0; i<vec.size(); i++){
 archive=checkpoint(vec);
 resurrect(archive, vec2);
 }

 if (std::equal(vec.begin(), vec.end(), vec2.begin())){
  hpx::cout<<"I work!"<<std::endl;
 }

 for (int i=0; i<vec2.size(); i++) {
  if (vec2.size() != i+1 ) hpx::cout<<vec2[i];
  else hpx::cout<<vec2[i]<<std::endl;
 }
 
 size_t count;
 off_t offset=17;
 
 //Test 1
 File file("test.txt");
 count=file.data.size();
 hpx::cout<<"Size of buffer: "<<count<<std::endl;
 file.print();
 file.save("test2.txt");
 
 return 0;
}
