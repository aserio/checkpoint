// Copyright (c) 2017 Adrian Serio
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// This example tests the save_checkpoint and restore_checkpoint functions for their ability to
// checkpoint and application.
//

#include <hpx/hpx_main.hpp>
#include <checkpoint.hpp>
#include <hpxio_file.hpp>

// Main
int main()
{
    char character = 'd';
    char character2;
    int integer = 10;
    int integer2;
    float flt = 10.01;
    float flt2;
    bool boolean = true;
    bool boolean2;
    std::string str = "I am a string of characters";
    std::string str2;
    std::vector<char> vec(str.begin(), str.end());
    std::vector<char> vec2;
    // std::vector<char> archive;
    checkpoint archive;
    checkpoint archive2;

    //Test 1
    hpx::cout << "Test 1:" ;

    //Print out initial variables
    hpx::cout << "Initial Variables" << std::endl
              << " char= " << character << std::endl
              << " int= " << integer << std::endl
              << " float= " << flt << std::endl
              << " bool= " << boolean << std::endl
              << " string= " << str << std::endl
              << " vector<char>= ";
    for (int i = 0; i < vec.size(); i++)
    {
        if (vec.size() != i + 1)
            hpx::cout << vec[i];
        else
            hpx::cout << vec[i] << std::endl;
    }

    hpx::shared_future<checkpoint> f_archive = save_checkpoint(std::move(archive)
                                         , character
                                         , integer
                                         , flt
                                         , boolean
                                         , str
                                         , vec);
    restore_checkpoint(
        f_archive.get(), character2, integer2, flt2, boolean2, str2, vec2);

    //Print out resurected variables
    hpx::cout << "Resurrected Variables" << std::endl
              << " char= " << character2 << std::endl
              << " int= " << integer2 << std::endl
              << " float= " << flt2 << std::endl
              << " bool= " << boolean2 << std::endl
              << " string= " << str2 << std::endl
              << " vector<char>= ";
    for (int i = 0; i < vec2.size(); i++)
    {
        if (vec2.size() != i + 1)
            hpx::cout << vec2[i];
        else
            hpx::cout << vec2[i] << std::endl;
    }

    if (character == character2 && integer == integer2 && flt == flt2 &&
        boolean == boolean2 && str == str2 &&
        std::equal(vec.begin(), vec.end(), vec2.begin()))
    {
        hpx::cout << " I work!" << std::endl;
    }

    //Test 2
    hpx::cout << "Test 2:";

    archive2 = f_archive.get();
    restore_checkpoint(
        archive2, character2, integer2, flt2, boolean2, str2, vec2);

    if (character == character2 && integer == integer2 && flt == flt2 &&
        boolean == boolean2 && str == str2 &&
        std::equal(vec.begin(), vec.end(), vec2.begin()))
    {
        hpx::cout << " I work!" << std::endl;
    }

    //Test 3
    hpx::cout << "Test 3:";

    hpx::future<checkpoint> archive3=
                               save_checkpoint(checkpoint(), vec, integer);
    restore_checkpoint(archive3.get(), vec2, integer2);

    if (std::equal(vec.begin(), vec.end(), vec2.begin()) && integer == integer2)
    {
        hpx::cout << " I work!" << std::endl;
    }

    //Test 4
    hpx::cout << "Test 4: ";
    hpxio_file omg("omg.txt");
    std::vector<char> char_buff = {'O', 'M', 'G', '!', ' ', 'T', 'h', 'i', 's',
        ' ', 'i', 's', ' ', 'a', ' ', 't', 'e', 's', 't', '!'};
    omg.data = char_buff;
    omg.print();
    omg.write();

    //Test 5
    std::cout<<"Test 5:";
    std::vector<int> test_vec;
    for (int c = 0; c < 101; c++)
    {
        test_vec.push_back(c);
    }
    checkpoint archive4 = save_checkpoint(hpx::launch::sync
                                        , checkpoint()
                                        , test_vec);
    std::cout<<" I work!"<<std::endl;

    //Test 6
    std::cout << "Test 6:";
    std::vector<int> test_vec2;
    restore_checkpoint(archive4, test_vec2);
    if (std::equal(test_vec.begin(), test_vec.end(), test_vec2.begin()))
    {
        std::cout << " I work!" << std::endl;
    }

    //Test 7
    std::cout << "Test 7:";
    checkpoint archive5(archive2);
    hpx::future<std::vector<int>> test_vec2_future =
        hpx::make_ready_future(test_vec2);
    hpx::future<checkpoint> f_check =
        save_checkpoint(std::move(archive5), test_vec2_future);
    hpx::future<std::vector<int>> test_vec3_future;
    restore_checkpoint(f_check.get(), test_vec3_future);
    if (test_vec2 == test_vec3_future.get())
    {
        std::cout << " I work!" << std::endl;
    }

    //Test 8
    //test the operator= constructor
    std::cout << "Test 8:";
    checkpoint archive6;
    archive6 = std::move(archive5);
    if (archive6 == archive5)
    {
        std::cout << " I work!" << std::endl;
    }

    //Test 9
    //test save_checkpoint future semantics
    std::cout << "Test 9:";
    int test_int = 8;
    int test_int2;
    int variable = 10;
    hpx::future<int> test_f_int = hpx::make_ready_future(variable);
    hpx::future<int> test_f_int2;
    std::ofstream test_file_9("test_file_9.txt");
    hpx::future<checkpoint> f_chk_file = save_checkpoint(
        std::move(checkpoint()), test_int, test_f_int);
    test_file_9<<f_chk_file.get();
    test_file_9.close();
    
    checkpoint archive_9;
    std::ifstream test_file_9_1("test_file_9.txt");
    test_file_9_1>>archive_9;
    restore_checkpoint(std::move(archive_9), test_int2, test_f_int2);
    test_file_9_1.close();
    if (test_int == test_int2 && variable == test_f_int2.get())
    {
        std::cout << " I work!" << std::endl;
    }
/*
    //Test 10
    //more tests with hpxio_file
    std::cout << "Test 10:" << std::endl;
    double test_double = 2.25;
    double test_double2;
    checkpoint_ns::checkpoint<hpxio_file> test_file_10("test_file_10.txt");
    hpx::shared_future<checkpoint_ns::checkpoint<hpxio_file>> f_test_file_10 =
        save_checkpoint(std::move(test_file_10), test_double);
    std::cout << f_test_file_10.get().data.size_data() << std::endl;
    //f_test_file_10.get().data.write(); //Get this to work
                                         //futures don't like their mem. changed
    restore_checkpoint(f_test_file_10.get(), test_double2);
    std::cout << "Double: " << test_double << "=" << test_double2 << std::endl;
    std::cout<<"I still need some work to write my data to file."<<std::endl;

    //Test 11
    //separate checkpoint and file usecase
    std::cout << "Test 11:";
    hpxio_file test_file_11("test_file_11.txt");
    std::vector<float> vec11{1.02, 1.03, 1.04, 1.05};
    hpx::future<checkpoint> fut_11=save_checkpoint(checkpoint(),vec11);
    test_file_11.write(fut_11.get());
    
    std::vector<float> vec_11_1;
    hpxio_file test_file_11_1("test_file_11.txt");
    checkpoint archive11;
    std::copy(
        test_file_11_1.data.begin()
      , test_file_11_1.data.end()
      , archive11.begin()
        );
    restore_checkpoint(archive11, vec_11_1);

    if(vec11==vec_11_1)
    {
        std::cout<<" I work!"<<std::endl;
    }

    //Test 12
    //test .load()
    std::cout << "Test 12:";
    std::vector<float> vec12;
    checkpoint archive12;
    archive12.load("test_file_11.txt");
    restore_checkpoint(archive12, vec12);
    if(vec12==vec_11_1)
    {
        std::cout<<" I work!"<<std::endl;
    }
*/    
    //Test 13
    //test policies
    std::cout << "Test 13:";
    int a13=10, b13=20, c13=30;
        std::cout<<std::endl<<"Values: "<<a13<<", "<<b13<<", "<<c13<<std::endl;
//    hpx::future<checkpoint> f_13=save_checkpoint(hpx::launch::async, checkpoint(), a13, b13, c13);
    hpx::future<checkpoint> f_13=save_checkpoint(hpx::launch::async, a13, b13, c13);
    int a13_1, b13_1, c13_1;
    restore_checkpoint(f_13.get(), a13_1, b13_1, c13_1);
        std::cout<<"Values: "<<a13_1<<", "<<b13_1<<", "<<c13_1<<std::endl;
    checkpoint archive13 = save_checkpoint(hpx::launch::sync, checkpoint(), a13_1, b13_1, c13_1);
    int a13_2, b13_2, c13_2;
    restore_checkpoint(archive13, a13_2, b13_2, c13_2);
        std::cout<<"Values: "<<a13_2<<", "<<b13_2<<", "<<c13_2<<std::endl;
    if(a13==a13_2 && b13==b13_2 && c13==c13_2)
    {
        std::cout<<" I work!"<<std::endl;
    }

    //Test 14
    //separate checkpoint and file usecase
    std::cout << "Test 14:";
    std::ofstream test_file_14("checkpoint_test_file.txt");
    std::vector<float> vec14{1.02, 1.03, 1.04, 1.05};
    hpx::future<checkpoint> fut_14=save_checkpoint(vec14);
    checkpoint archive14 = fut_14.get();
    test_file_14<<archive14;
//    test_file_14.write(archive14.data.data(),archive14.size());
    test_file_14.close();
    {
        std::cout<<" I work!"<<" I am: "<<archive14.size()<<std::endl;
        std::ifstream ifs("checkpoint_test_file.txt");
            if(ifs)                               //Check fstream is open
            {
                ifs.seekg(0, ifs.end);
                int length = ifs.tellg();         //Get length of file
                std::cout<<"File Length: "<<length<<std::endl;
            }
    }

    //test operator>>
    std::vector<float> vec14_1;
    checkpoint archive14_1;
    std::ifstream test_file_14_1("checkpoint_test_file.txt");
    test_file_14_1>>archive14_1;
    restore_checkpoint(archive14_1, vec14_1);
    if(vec14==vec14_1)
    {
        std::cout<<" I work!"<<std::endl;
    }
    
    //Test 15
    // testing iterators
    // Old code below
    //std::copy(archive15.begin(), archive15.end(), std::ostream_iterator<char>(test_file_15));
    std::cout<<"Test 15:";
    std::ofstream test_file_15("test_file_15.txt");
    std::vector<int> vec15{1,2,3,4,5};
    hpx::future<checkpoint> fut_15=save_checkpoint(vec15);
    checkpoint archive15=fut_15.get();
    test_file_15<<archive15;
    test_file_15.flush();
    
    std::vector<int> vec15_1;
    checkpoint archive15_1;
    std::ifstream test_file_15_1("test_file_15.txt");
    test_file_15_1>>archive15_1;
    restore_checkpoint(archive15_1, vec15_1);
    if(vec15==vec15_1)
    {
        std::cout<<" I work!"<<std::endl;
    }

    return 0;
}

