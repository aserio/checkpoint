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
    checkpoint<> archive;
    checkpoint<> archive2;

    //Test 1
    hpx::cout << "Test 1:" << std::endl;

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

    save_checkpoint(archive, character, integer, flt, boolean, str, vec);
    restore_checkpoint(
        archive, character2, integer2, flt2, boolean2, str2, vec2);

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
        hpx::cout << "I work!" << std::endl;
    }

    //Test 2
    hpx::cout << "Test 2:" << std::endl;

    archive2 = archive;
    restore_checkpoint(
        archive, character2, integer2, flt2, boolean2, str2, vec2);

    if (character == character2 && integer == integer2 && flt == flt2 &&
        boolean == boolean2 && str == str2 &&
        std::equal(vec.begin(), vec.end(), vec2.begin()))
    {
        hpx::cout << "I work!" << std::endl;
    }

    //Test 3
    hpx::cout << "Test 3:" << std::endl;

    checkpoint<> archive3;
    save_checkpoint(archive3, vec, integer);
    restore_checkpoint(archive3, vec2, integer2);

    if (std::equal(vec.begin(), vec.end(), vec2.begin()) && integer == integer2)
    {
        hpx::cout << "I work!" << std::endl;
    }

    //Test 4
    hpx::cout << "Test 4:" << std::endl;
    hpxio_file omg("omg.txt");
    std::vector<char> char_buff = {'O', 'M', 'G', '!', ' ', 'T', 'h', 'i', 's',
        ' ', 'i', 's', ' ', 'a', ' ', 't', 'e', 's', 't', '!'};
    omg.data = char_buff;
    omg.print();
    omg.write();

    //Test 5
    // checkpoint<hpxio_file> archive4("test4.archive");
    checkpoint<> archive4;
    std::vector<int> test_vec;
    for (int c = 0; c < 101; c++)
    {
        test_vec.push_back(c);
    }
    save_checkpoint(archive4, test_vec);
    // archive4.data.write();

    //Test 6
    std::cout << "Test 6:" << std::endl;
    std::vector<int> test_vec2;
    restore_checkpoint(archive4, test_vec2);
    if (std::equal(test_vec.begin(), test_vec.end(), test_vec2.begin()))
    {
        std::cout << "I work!" << std::endl;
    }

    //Test 7
    std::cout << "Test 7:" << std::endl;
    checkpoint<> archive5(archive2);
    hpx::future<std::vector<int>> test_vec2_future =
        hpx::make_ready_future(test_vec2);
    hpx::future<checkpoint<>> f_check =
        save_checkpoint_future(std::move(archive5), test_vec2_future);
    hpx::future<std::vector<int>> test_vec3_future;
    restore_checkpoint(f_check.get(), test_vec3_future);
    if (test_vec2 == test_vec3_future.get())
    {
        std::cout << "I work!" << std::endl;
    }

    //Test 8
    //test the operator= constructor
    std::cout << "Test 8:" << std::endl;
    checkpoint<> archive6;
    archive6 = std::move(archive5);
    if (archive6.data == archive5.data)
    {
        std::cout << "I work!" << std::endl;
    }

    //Test 9
    //test save_checkpoint_future semantics
    std::cout << "Test 9:" << std::endl;
    int test_int = 8;
    int test_int2;
    int variable = 10;
    hpx::future<int> test_f_int = hpx::make_ready_future(variable);
    hpx::future<int> test_f_int2;
    hpx::future<checkpoint<hpxio_file>> f_chk_file = save_checkpoint_future(
        std::move(checkpoint<hpxio_file>("test_file_9.txt")), test_int, test_f_int);
    restore_checkpoint(f_chk_file.get(), test_int2, test_f_int2);
    if (test_int == test_int2 && variable == test_f_int2.get())
    {
        std::cout << "I work!" << std::endl;
    }

    //Test 10
    //more tests with hpxio_file
    std::cout << "Test 10:" << std::endl;
    double test_double = 2.25;
    double test_double2;
    checkpoint<hpxio_file> test_file_10("test_file_10.txt");
    hpx::shared_future<checkpoint<hpxio_file>> f_test_file_10 =
        save_checkpoint_future(std::move(test_file_10), test_double);
    std::cout << f_test_file_10.get().data.size_data() << std::endl;
    //f_test_file_10.get().data.write(); //Get this to work
                                         //futures don't like their mem. changed
    restore_checkpoint(f_test_file_10.get(), test_double2);
    std::cout << "Double: " << test_double << "=" << test_double2 << std::endl;
    std::cout<<"I still need some work to write my data to file."<<std::endl;

    //Test 11
    //separate checkpoint and file usecase
    std::cout << "Test 11:" << std::endl;
    hpxio_file test_file_11("test_file_11.txt");
    std::vector<float> vec11{1.02, 1.03, 1.04, 1.05};
    hpx::future<checkpoint<>> fut_11=save_checkpoint_future(checkpoint<>(),vec11);
    test_file_11.write(fut_11.get().data);
    
    std::vector<float> vec_11_1;
    hpxio_file test_file_11_1("test_file_11.txt");
    checkpoint<> archive11;
    archive11.data=test_file_11_1.data;
    restore_checkpoint(archive11, vec_11_1);

    if(vec11==vec_11_1)
    {
        std::cout<<"I work!"<<std::endl;
    }

    //Test 12
    //test .load()
    std::cout << "Test 12:" << std::endl;
    std::vector<float> vec12;
    checkpoint<> archive12;
    archive12.load("test_file_11.txt");
    restore_checkpoint(archive12, vec12);
    if(vec12==vec_11_1)
    {
        std::cout<<"I work!"<<std::endl;
    }
    


    return 0;
}
