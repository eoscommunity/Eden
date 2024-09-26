#include <boost/multi_index/key.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/timer.hpp>
#include <chainbase/chainbase.hpp>
#include <eosio/from_bin.hpp>
#include <eosio/reflection.hpp>
#include <eosio/stream.hpp>
#include <eosio/to_bin.hpp>
#include <iostream>
#include <random>

struct test_object : public chainbase::object<0, test_object>
{
   CHAINBASE_DEFAULT_CONSTRUCTOR(test_object)
   id_type id;
   uint64_t value;
};
EOSIO_REFLECT(test_object, id, value)

constexpr int count = 10000000;

int main()
{
   chainbase::undo_index<
       test_object, std::allocator<test_object>,
       boost::multi_index::ordered_unique<boost::multi_index::key<&test_object::id>>,
       boost::multi_index::ordered_unique<boost::multi_index::key<&test_object::value>>>
       test_idx;
   std::default_random_engine gen;
   for (int i = 0; i < count; ++i)
   {
      std::uniform_int_distribution<uint64_t> dist;
      test_idx.emplace([&](test_object& row) { row.value = dist(gen); });
   }
   std::vector<char> buf;
   {
      std::cout << "save: " << std::flush;
      eosio::vector_stream out{buf};
      boost::timer timer;
      to_bin(test_idx, out);
      std::cout << timer.elapsed() << std::endl;
   }
   std::cout << "serialized size: " << buf.size() << std::endl;
   {
      std::cout << "load: " << std::flush;
      eosio::input_stream in{buf};
      decltype(test_idx) copy;
      boost::timer timer;
      from_bin(copy, in);
      std::cout << timer.elapsed() << std::endl;
   }
   {
      std::cout << "load vector: " << std::flush;
      eosio::input_stream in{buf};
      std::vector<std::pair<uint64_t, uint64_t>> copy;
      boost::timer timer;
      from_bin(copy, in);
      std::cout << timer.elapsed() << std::endl;
   }
}
