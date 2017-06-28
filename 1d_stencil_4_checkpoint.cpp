//  Copyright (c) 2014 Hartmut Kaiser
//  Copyright (c) 2014 Patricia Grubel
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This is the fourth in a series of examples demonstrating the development of
// a fully distributed solver for a simple 1D heat distribution problem.
//
// This example builds on example three. It futurizes the code from that
// example. Compared to example two this code runs much more efficiently. It
// allows for changing the amount of work executed in one HPX thread which
// enables tuning the performance for the optimal grain size of the
// computation. This example is still fully local but demonstrates nice
// scalability on SMP machines.
//
//

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>

#include <hpx/include/parallel_algorithm.hpp>
#include <boost/range/irange.hpp>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "print_time_results.hpp"

#include <hpxio_file.hpp>
#include <checkpoint.hpp>

///////////////////////////////////////////////////////////////////////////////
// Command-line variables
bool header = true; // print csv heading
double k = 0.5;     // heat transfer coefficient
double dt = 1.;     // time step
double dx = 1.;     // grid spacing

inline std::size_t idx(std::size_t i, int dir, std::size_t size)
{
    if(i == 0 && dir == -1)
        return size-1;
    if(i == size-1 && dir == +1)
        return 0;

    HPX_ASSERT((i + dir) < size);

    return i + dir;
}

///////////////////////////////////////////////////////////////////////////////
// Our partition data type
struct partition_data
{
private:
 typedef hpx::serialization::serialize_buffer<double> buffer_type;
public:
    partition_data()
      : data_(),
        size_(0)
    {}

    partition_data(std::size_t size)
      : data_(new double[size], size, buffer_type::take), 
        size_(size)
    {}

    partition_data(std::size_t size, double initial_value)
      : data_(new double[size], size, buffer_type::take), 
        size_(size)
    {
        double base_value = double(initial_value * size);
        for (std::size_t i = 0; i != size; ++i)
            data_[i] = base_value + double(i);
    }
    
    double& operator[](std::size_t idx) { return data_[idx]; }
    double operator[](std::size_t idx) const { return data_[idx]; }

    std::size_t size() const { return size_; }

private:
    buffer_type data_;
    std::size_t size_;
    
    // Serialization Definitions
    friend class hpx::serialization::access;
    template<typename Volume>
    void serialize(Volume& vol, const unsigned int version) {
     vol & data_ & size_;
    }
};

std::ostream& operator<<(std::ostream& os, partition_data const& c)
{
    os << "{";
    for (std::size_t i = 0; i != c.size(); ++i)
    {
        if (i != 0)
            os << ", ";
        os << c[i];
    }
    os << "}";
    return os;
}

///////////////////////////////////////////////////////////////////////////////
// Checkpoint Function

void save(partition_data const& status, std::string file_name, std::size_t timestep, std::size_t index) {
  file_name=file_name+std::to_string(timestep)+"_part"+std::to_string(index);
  Checkpoint<hpxio_file> file_archive(file_name);
  store(file_archive, status);
  
/*  //Check to see if store is working
  Checkpoint<> do_i_work;
  partition_data test_part(status.size());
  resurrect(file_archive, test_part);
  for(int y=0; y < status.size(); y++) {
    if (status[y]==test_part[y]) { std::cout<<"I WORK!"<<std::endl; }
  }
*/  
  file_archive.data.write();
  file_archive.data.close();
}
///////////////////////////////////////////////////////////////////////////////
struct stepper
{
    // Our data for one time step
    typedef hpx::shared_future<partition_data> partition;
    typedef std::vector<partition> space;

    // Our operator
    static double heat(double left, double middle, double right)
    {
        return middle + (k*dt/(dx*dx)) * (left - 2*middle + right);
    }

    // The partitioned operator, it invokes the heat operator above on all
    // elements of a partition.
    static partition_data heat_part(partition_data const& left,
        partition_data const& middle, partition_data const& right)
    {
        std::size_t size = middle.size();
        partition_data next(size);

        next[0] = heat(left[size-1], middle[0], middle[1]);

        for(std::size_t i = 1; i != size-1; ++i)
        {
            next[i] = heat(middle[i-1], middle[i], middle[i+1]);
        }

        next[size-1] = heat(middle[size-2], middle[size-1], right[0]);

        return next;
    }

    // do all the work on 'np' partitions, 'nx' data points each, for 'nt'
    // time steps, limit depth of dependency tree to 'nd'
    hpx::future<space> do_work(std::size_t np, std::size_t nx, std::size_t nt,
        std::uint64_t nd)
    {
        using hpx::dataflow;
        using hpx::util::unwrapped;

        std::string file_name="1d.archive";

        // U[t][i] is the state of position i at time t.
        std::vector<space> U(2);
        for (space& s: U)
            s.resize(np);

        // Initial conditions: f(0, i) = i
        std::size_t b = 0;
        auto range = boost::irange(b, np);
        using hpx::parallel::execution::par;
        hpx::parallel::for_each(par, boost::begin(range), boost::end(range),
            [&U, nx](std::size_t i)
            {
                U[0][i] = hpx::make_ready_future(partition_data(nx, double(i)));
            }
        );

        // limit depth of dependency tree
        hpx::lcos::local::sliding_semaphore sem(nd);

        auto Op = unwrapped(&stepper::heat_part);

        // Actual time step loop
        for (std::size_t t = 0; t != nt; ++t)
        {
            space const& current = U[t % 2];
            space& next = U[(t + 1) % 2];

            for (std::size_t i = 0; i != np; ++i)
            {
                next[i] = dataflow(
                        hpx::launch::async, Op,
                        current[idx(i, -1, np)], current[i], current[idx(i, +1, np)]
                    );
                
                //Checkpoint
                if (t % 44 == 0 && t != 0 ) {
                 next[i]=next[i].then( [file_name,nt,i](partition && p) {
                                        partition_data value(p.get());
                                        save(value, file_name, nt, i);
                                        partition f_value=
                                               hpx::make_ready_future(value);
                                        return f_value;

                                       }
                                      );
             }
            }
            
            // every nd time steps, attach additional continuation which will
            // trigger the semaphore once computation has reached this point
            if ((t % nd) == 0)
            {
                next[0].then(
                    [&sem, t](partition &&)
                    {
                        // inform semaphore about new lower limit
                        sem.signal(t);
                    });
            }

            // suspend if the tree has become too deep, the continuation above
            // will resume this thread once the computation has caught up
            sem.wait(t);
        }

        // Return the solution at time-step 'nt'.
        return hpx::when_all(U[nt % 2]);
    }
};

///////////////////////////////////////////////////////////////////////////////
int hpx_main(boost::program_options::variables_map& vm)
{
    std::uint64_t np = vm["np"].as<std::uint64_t>();   // Number of partitions.
    std::uint64_t nx = vm["nx"].as<std::uint64_t>();   // Number of grid points.
    std::uint64_t nt = vm["nt"].as<std::uint64_t>();   // Number of steps.
    std::uint64_t nd = vm["nd"].as<std::uint64_t>();   // Max depth of dep tree.

    if (vm.count("no-header"))
        header = false;

    //hpxio_file test_file("test.txt");
    //test_file.print();
    
    // Create the stepper object
    stepper step;

    // Measure execution time.
    std::uint64_t t = hpx::util::high_resolution_clock::now();

    // Execute nt time steps on nx grid points and print the final solution.
    hpx::future<stepper::space> result = step.do_work(np, nx, nt, nd);

    stepper::space solution = result.get();
    hpx::wait_all(solution);

    std::uint64_t elapsed = hpx::util::high_resolution_clock::now() - t;

    // Print the final solution
    if (vm.count("results"))
    {
        for (std::size_t i = 0; i != np; ++i)
            std::cout << "U[" << i << "] = " << solution[i].get() << std::endl;
    }

    std::uint64_t const os_thread_count = hpx::get_os_thread_count();
    print_time_results(os_thread_count, elapsed, nx, np, nt, header);

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    using namespace boost::program_options;

    // Configure application-specific options.
    options_description desc_commandline;

    desc_commandline.add_options()
        ("results", "print generated results (default: false)")
        ("nx", value<std::uint64_t>()->default_value(10),
         "Local x dimension (of each partition)")
        ("nt", value<std::uint64_t>()->default_value(45),
         "Number of time steps")
        ("nd", value<std::uint64_t>()->default_value(10),
         "Number of time steps to allow the dependency tree to grow to")
        ("np", value<std::uint64_t>()->default_value(10),
         "Number of partitions")
        ("k", value<double>(&k)->default_value(0.5),
         "Heat transfer coefficient (default: 0.5)")
        ("dt", value<double>(&dt)->default_value(1.0),
         "Timestep unit (default: 1.0[s])")
        ("dx", value<double>(&dx)->default_value(1.0),
         "Local x dimension")
        ( "no-header", "do not print out the csv header row")
    ;

    // Initialize and run HPX
    return hpx::init(desc_commandline, argc, argv);
}
