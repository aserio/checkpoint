//  Copyright (c) 2017 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This example demonstrates how the HPX serialization archives could be used
// to directly store/load to/from a file.

#include <hpx/hpx.hpp>
#include <hpx/hpx_main.hpp>
#include <hpx/traits/serialization_access_data.hpp>

#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <checkpoint.hpp>

struct file_wrapper
{
    file_wrapper(std::string const& name, std::ios_base::openmode mode)
      : name_(name)
      , mode_(mode)
    {
        stream_.open(name_.c_str(), mode);
        if (!stream_.is_open())
            throw std::runtime_error("Couldn't open file: " + name);

        std::fstream::pos_type fsize = stream_.tellg();
        stream_.seekg(0, std::ios::end);
        data_.resize(stream_.tellg() - fsize);
        stream_.seekg(fsize, std::ios::beg);

        if (mode & std::ios_base::in)
        {
            stream_.read(data_.data(), data_.size());
        }
    }
    ~file_wrapper()
    {
        if (mode_ & std::ios_base::out)
        {
            stream_.write(data_.data(), data_.size());
        }
    }

    void read(void* address, std::size_t count, std::size_t current) const
    {
        std::memcpy(address, &data_[current], count);
    }

    void write(void const* address, std::size_t count, std::size_t current)
    {
        std::memcpy(&data_[current], address, count);
    }

    std::size_t size_data() const
    {
        return data_.size();
    }

    void resize(std::size_t count)
    {
        data_.resize(data_.size() + count);
    }

    char& operator[](int i)
    {
        return data_[i];
    }

    const char& operator[](int i) const
    {
        return data_[i];
    }

private:
    std::vector<char> data_;
    std::string name_;
    std::fstream stream_;
    std::ios_base::openmode mode_;
};

namespace hpx {
namespace traits {
    template <>
    struct serialization_access_data<file_wrapper>
        : default_serialization_access_data<file_wrapper>
    {
        static std::size_t size(file_wrapper const& cont)
        {
            return cont.size_data();
        }

        static void resize(file_wrapper& cont, std::size_t count)
        {
            return cont.resize(count);
        }

        static void write(file_wrapper& cont, std::size_t count,
            std::size_t current, void const* address)
        {
            cont.write(address, count, current);
        }

        // functions related to input operations
        static void read(file_wrapper const& cont, std::size_t count,
            std::size_t current, void* address)
        {
            cont.read(address, count, current);
        }
    };
}
}

int main(int argc, char* argv[])
{
    std::size_t size = 0;
    std::vector<double> os;
    std::vector<double> os2;
    {
        file_wrapper buffer("file_serialization_test.archive",
            std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
        hpx::serialization::output_archive oarchive(buffer);
        for (double c = -100.0; c < +100.0; c += 1.3)
        {
            os.push_back(c);
        }
        oarchive << os;
        size = oarchive.bytes_written();
    }

    {
        file_wrapper buffer("file_serialization_test.archive",
            std::ios_base::in | std::ios_base::binary);
        hpx::serialization::input_archive iarchive(buffer, size);
        std::vector<double> is;
        iarchive >> is;
        for (std::size_t i = 0; i < os.size(); ++i)
        {
            if (os[i] != is[i])
            {
                std::cerr << "Mismatch for element " << i << ":" << os[i]
                          << " != " << is[i] << "\n";
            }
        }
    }

    {
        //       checkpoint<> check;
        //       file_wrapper buffer2("buffer2_test.archive",
        //            std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
        //        checkpoint<file_wrapper> check(buffer2);
        checkpoint<file_wrapper> check("buffer2_test.archive",
            std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
        for (double c = -100.0; c < +100.0; c += 1.3)
        {
            os2.push_back(c);
        }
        save_checkpoint(check, os2);
    }

    {
        checkpoint<file_wrapper> check2(
            "buffer2_test.archive", std::ios_base::in | std::ios_base::binary);
        std::vector<double> is2;
        restore_checkpoint(check2, is2);
        for (std::size_t i = 0; i < os2.size(); ++i)
        {
            if (os2[i] != is2[i])
            {
                std::cerr << "Mismatch for element " << i << ":" << os2[i]
                          << " != " << is2[i] << "\n";
            }
        }
    }

    return 0;
}
