#include <boost/multi_index/key.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <chainbase/chainbase.hpp>
#include <eosio/from_bin.hpp>
#include <eosio/operators.hpp>
#include <eosio/stream.hpp>
#include <eosio/to_bin.hpp>

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

struct test_object : public chainbase::object<0, test_object>
{
   CHAINBASE_DEFAULT_CONSTRUCTOR(test_object)
   id_type id;
   uint64_t value;
};
EOSIO_REFLECT(test_object, id, value);
EOSIO_COMPARE(test_object);

std::ostream& operator<<(std::ostream& os, const test_object& o)
{
   os << '{' << o.id._id << ',' << o.value << '}';
   return os;
}

namespace std
{
   std::ostream& operator<<(std::ostream& os, const std::pair<int64_t, int64_t>& p)
   {
      os << '{' << p.first << ',' << p.second << '}';
      return os;
   }
}  // namespace std

template <typename T>
void serialize_copy(const T& t, T& result)
{
   std::vector<char> buf;
   eosio::vector_stream out{buf};
   to_bin(t, out);
   eosio::input_stream in{buf};
   from_bin(result, in);
}

TEST_CASE("serialize")
{
   chainbase::undo_index<
       test_object, std::allocator<test_object>,
       boost::multi_index::ordered_unique<boost::multi_index::key<&test_object::id>>,
       boost::multi_index::ordered_unique<boost::multi_index::key<&test_object::value>>>
       test_idx;

   decltype(test_idx) copy;
   bool copied = false;
   auto check_contents = [&] {
      if (copied)
      {
         CHECK(test_idx.get<0>() == copy.get<0>());
         CHECK(test_idx.get<1>() == copy.get<1>());
         CHECK(test_idx.undo_stack_revision_range() == copy.undo_stack_revision_range());
      }
   };

   auto emplace = [&](uint64_t value) {
      test_idx.emplace([&](test_object& row) { row.value = value; });
      if (copied)
      {
         copy.emplace([&](test_object& row) { row.value = value; });
      }
   };

   auto modify = [&](uint64_t old, uint64_t new_) {
      auto& index = test_idx.get<1>();
      auto pos = index.find(old);
      CHECK(pos != index.end());
      test_idx.modify(*pos, [&](test_object& row) { row.value = new_; });
      if (copied)
      {
         copy.modify(*copy.get<1>().find(old), [&](test_object& row) { row.value = new_; });
      }
   };

   auto remove = [&](uint64_t value) {
      auto& index = test_idx.get<1>();
      auto pos = index.find(value);
      CHECK(pos != index.end());
      test_idx.remove(*pos);
      if (copied)
      {
         copy.remove(*copy.get<1>().find(value));
      }
   };

   auto start_undo_session = [&](bool enabled = true) {
      test_idx.start_undo_session(enabled).push();
      if (copied)
      {
         copy.start_undo_session(enabled).push();
      }
   };

   auto undo = [&]() {
      test_idx.undo();
      if (copied)
      {
         copy.undo();
      }
   };

   auto squash = [&]() {
      test_idx.squash();
      if (copied)
      {
         copy.squash();
      }
   };

   auto checkpoint = [&](const char* name) {
      SECTION(name)
      {
         serialize_copy(test_idx, copy);
         copied = true;
      }
      check_contents();
   };

   checkpoint("empty");
   emplace(5);
   checkpoint("one");
   emplace(4);
   checkpoint("multiple");
   start_undo_session();
   checkpoint("empty session");
   emplace(6);
   checkpoint("session with add");
   undo();
   checkpoint("after undo");
   start_undo_session();
   emplace(7);
   emplace(3);
   start_undo_session();
   emplace(8);
   emplace(2);
   squash();
   checkpoint("after squash");
   start_undo_session();
   emplace(42);
   emplace(9);
   start_undo_session();
   modify(42, 43);
   emplace(1);
   start_undo_session();
   remove(43);
   emplace(10);
   squash();
   checkpoint("multiple entries");
   undo();
   checkpoint("undo 1");
   undo();
   checkpoint("undo 2");
   undo();
   checkpoint("undo 3");
}
