#pragma once

#include <chainbase/undo_index.hpp>

#include <boost/core/demangle.hpp>
#include <boost/multi_index_container.hpp>

#include <iostream>
#include <set>
#include <sstream>
#include <typeindex>
#include <typeinfo>
#include <vector>

namespace chainbase
{
   using std::unique_ptr;
   using std::vector;

   template <typename T>
   using allocator = std::allocator<T>;

   template <typename T>
   using node_allocator = std::allocator<T>;

   /**
    *  Object ID type that includes the type of the object it references
    */
   template <typename T>
   class oid
   {
     public:
      oid(int64_t i = 0) : _id(i) {}

      oid& operator++()
      {
         ++_id;
         return *this;
      }

      friend bool operator<(const oid& a, const oid& b) { return a._id < b._id; }
      friend bool operator>(const oid& a, const oid& b) { return a._id > b._id; }
      friend bool operator<=(const oid& a, const oid& b) { return a._id <= b._id; }
      friend bool operator>=(const oid& a, const oid& b) { return a._id >= b._id; }
      friend bool operator==(const oid& a, const oid& b) { return a._id == b._id; }
      friend bool operator!=(const oid& a, const oid& b) { return a._id != b._id; }
      friend std::ostream& operator<<(std::ostream& s, const oid& id)
      {
         s << boost::core::demangle(typeid(oid<T>).name()) << '(' << id._id << ')';
         return s;
      }

      int64_t _id = 0;
   };

   template <typename T, typename S>
   void to_bin(const oid<T>& id, S& stream)
   {
      to_bin(id._id, stream);
   }

   template <typename T, typename S>
   void from_bin(oid<T>& id, S& stream)
   {
      from_bin(id._id, stream);
   }

   template <uint16_t TypeNumber, typename Derived>
   struct object
   {
      typedef oid<Derived> id_type;
      static const uint16_t type_id = TypeNumber;
   };

   /** this class is ment to be specified to enable lookup of index type by object type using
    * the SET_INDEX_TYPE macro.
    **/
   template <typename T>
   struct get_index_type
   {
   };

   /**
    *  This macro must be used at global scope and OBJECT_TYPE and INDEX_TYPE must be fully qualified
    */
#define CHAINBASE_SET_INDEX_TYPE(OBJECT_TYPE, INDEX_TYPE) \
   namespace chainbase                                    \
   {                                                      \
      template <>                                         \
      struct get_index_type<OBJECT_TYPE>                  \
      {                                                   \
         typedef INDEX_TYPE type;                         \
      };                                                  \
   }

#define CHAINBASE_DEFAULT_CONSTRUCTOR(OBJECT_TYPE)     \
   template <typename Constructor, typename Allocator> \
   OBJECT_TYPE(Constructor&& c, Allocator&&)           \
   {                                                   \
      c(*this);                                        \
   }

   template <typename MultiIndexType>
   using generic_index = multi_index_to_undo_index<MultiIndexType>;

   class abstract_session
   {
     public:
      virtual ~abstract_session(){};
      virtual void push() = 0;
      virtual void squash() = 0;
      virtual void undo() = 0;
   };

   template <typename SessionType>
   class session_impl : public abstract_session
   {
     public:
      session_impl(SessionType&& s) : _session(std::move(s)) {}

      virtual void push() override { _session.push(); }
      virtual void squash() override { _session.squash(); }
      virtual void undo() override { _session.undo(); }

     private:
      SessionType _session;
   };

   class abstract_index
   {
     public:
      abstract_index(void* i) : _idx_ptr(i) {}
      virtual ~abstract_index() {}
      virtual void set_revision(uint64_t revision) = 0;
      virtual unique_ptr<abstract_session> start_undo_session(bool enabled) = 0;

      virtual int64_t revision() const = 0;
      virtual void undo() const = 0;
      virtual void squash() const = 0;
      virtual void commit(int64_t revision) const = 0;
      virtual void undo_all() const = 0;
      virtual uint32_t type_id() const = 0;
      virtual uint64_t row_count() const = 0;
      virtual const std::string& type_name() const = 0;
      virtual std::pair<int64_t, int64_t> undo_stack_revision_range() const = 0;

      virtual void remove_object(int64_t id) = 0;

      void* get() const { return _idx_ptr; }

     private:
      void* _idx_ptr;
   };

   template <typename BaseIndex>
   class index_impl : public abstract_index
   {
     public:
      index_impl(BaseIndex& base) : abstract_index(&base), _base(base) {}

      virtual unique_ptr<abstract_session> start_undo_session(bool enabled) override
      {
         return std::make_unique<session_impl<typename BaseIndex::session>>(
             _base.start_undo_session(enabled));
      }

      virtual void set_revision(uint64_t revision) override { _base.set_revision(revision); }
      virtual int64_t revision() const override { return _base.revision(); }
      virtual void undo() const override { _base.undo(); }
      virtual void squash() const override { _base.squash(); }
      virtual void commit(int64_t revision) const override { _base.commit(revision); }
      virtual void undo_all() const override { _base.undo_all(); }
      virtual uint32_t type_id() const override { return BaseIndex::value_type::type_id; }
      virtual uint64_t row_count() const override { return _base.indices().size(); }
      virtual const std::string& type_name() const override { return BaseIndex_name; }
      virtual std::pair<int64_t, int64_t> undo_stack_revision_range() const override
      {
         return _base.undo_stack_revision_range();
      }

      virtual void remove_object(int64_t id) override { return _base.remove_object(id); }

     private:
      BaseIndex& _base;
      std::string BaseIndex_name =
          boost::core::demangle(typeid(typename BaseIndex::value_type).name());
   };

   template <typename IndexType>
   class index : public index_impl<IndexType>
   {
     public:
      index(IndexType& i) : index_impl<IndexType>(i) {}
   };

   class database
   {
     public:
      using database_index_row_count_multiset = std::multiset<std::pair<unsigned, std::string>>;

      database() = default;
      database(database&&) = default;
      database& operator=(database&&) = default;
      ~database()
      {
         _index_list.clear();
         _index_map.clear();
      }

      struct session
      {
        public:
         session(session&& s) = default;
         session(vector<std::unique_ptr<abstract_session>>&& s) : _index_sessions(std::move(s)) {}

         ~session() { undo(); }

         void push()
         {
            for (auto& i : _index_sessions)
               i->push();
            _index_sessions.clear();
         }

         void squash()
         {
            for (auto& i : _index_sessions)
               i->squash();
            _index_sessions.clear();
         }

         void undo()
         {
            for (auto& i : _index_sessions)
               i->undo();
            _index_sessions.clear();
         }

        private:
         friend class database;
         session() {}

         vector<std::unique_ptr<abstract_session>> _index_sessions;
      };

      session start_undo_session(bool enabled)
      {
         if (enabled)
         {
            vector<std::unique_ptr<abstract_session>> _sub_sessions;
            _sub_sessions.reserve(_index_list.size());
            for (auto& item : _index_list)
            {
               _sub_sessions.push_back(item->start_undo_session(enabled));
            }
            return session(std::move(_sub_sessions));
         }
         else
         {
            return session();
         }
      }

      int64_t revision() const
      {
         if (_index_list.size() == 0)
            return -1;
         return _index_list[0]->revision();
      }

      std::pair<int64_t, int64_t> undo_stack_revision_range() const
      {
         if (_index_list.size() == 0)
            return {-1, -1};
         return _index_list[0]->undo_stack_revision_range();
      }

      void undo()
      {
         for (auto& item : _index_list)
         {
            item->undo();
         }
      }

      void squash()
      {
         for (auto& item : _index_list)
         {
            item->squash();
         }
      }

      void commit(int64_t revision)
      {
         for (auto& item : _index_list)
         {
            item->commit(revision);
         }
      }

      void undo_all()
      {
         for (auto& item : _index_list)
         {
            item->undo_all();
         }
      }

      void set_revision(uint64_t revision)
      {
         for (auto i : _index_list)
            i->set_revision(revision);
      }

      template <typename index_type>
      void add_index(index_type& the_index)
      {
         const uint16_t type_id = index_type::value_type::type_id;
         typedef typename index_type::allocator_type index_alloc;

         std::string type_name =
             boost::core::demangle(typeid(typename index_type::value_type).name());

         if (!(_index_map.size() <= type_id || _index_map[type_id] == nullptr))
            eosio::check(false, "::type_id is already in use");

         if (type_id >= _index_map.size())
            _index_map.resize(type_id + 1);

         auto new_index = std::make_unique<index<index_type>>(the_index);
         _index_list.push_back(&*new_index);
         _index_map[type_id] = std::move(new_index);
      }

      template <typename MultiIndexType>
      const generic_index<MultiIndexType>& get_index() const
      {
         typedef generic_index<MultiIndexType> index_type;
         typedef index_type* index_type_ptr;
         assert(_index_map.size() > index_type::value_type::type_id);
         assert(_index_map[index_type::value_type::type_id]);
         return *index_type_ptr(_index_map[index_type::value_type::type_id]->get());
      }

      template <typename MultiIndexType, typename ByIndex>
      auto get_index() const -> decltype(
          ((generic_index<MultiIndexType>*)(nullptr))->indices().template get<ByIndex>())
      {
         typedef generic_index<MultiIndexType> index_type;
         typedef index_type* index_type_ptr;
         assert(_index_map.size() > index_type::value_type::type_id);
         assert(_index_map[index_type::value_type::type_id]);
         return index_type_ptr(_index_map[index_type::value_type::type_id]->get())
             ->indices()
             .template get<ByIndex>();
      }

      template <typename MultiIndexType>
      generic_index<MultiIndexType>& get_mutable_index()
      {
         typedef generic_index<MultiIndexType> index_type;
         typedef index_type* index_type_ptr;
         assert(_index_map.size() > index_type::value_type::type_id);
         assert(_index_map[index_type::value_type::type_id]);
         return *index_type_ptr(_index_map[index_type::value_type::type_id]->get());
      }

      template <typename ObjectType, typename IndexedByType, typename CompatibleKey>
      const ObjectType* find(CompatibleKey&& key) const
      {
         typedef typename get_index_type<ObjectType>::type index_type;
         const auto& idx = get_index<index_type>().indices().template get<IndexedByType>();
         auto itr = idx.find(std::forward<CompatibleKey>(key));
         if (itr == idx.end())
            return nullptr;
         return &*itr;
      }

      template <typename ObjectType>
      const ObjectType* find(oid<ObjectType> key = oid<ObjectType>()) const
      {
         typedef typename get_index_type<ObjectType>::type index_type;
         return get_index<index_type>().find(key);
      }

      template <typename ObjectType, typename IndexedByType, typename CompatibleKey>
      const ObjectType& get(CompatibleKey&& key) const
      {
         auto obj = find<ObjectType, IndexedByType>(std::forward<CompatibleKey>(key));
         if (!obj)
         {
            std::stringstream ss;
            ss << "unknown key (" << boost::core::demangle(typeid(key).name()) << "): " << key;
            eosio::check(false, ss.str());
         }
         return *obj;
      }

      template <typename ObjectType>
      const ObjectType& get(const oid<ObjectType>& key = oid<ObjectType>()) const
      {
         auto obj = find<ObjectType>(key);
         if (!obj)
         {
            std::stringstream ss;
            ss << "unknown key (" << boost::core::demangle(typeid(key).name()) << "): " << key._id;
            eosio::check(false, ss.str());
         }
         return *obj;
      }

      template <typename ObjectType, typename Modifier>
      void modify(const ObjectType& obj, Modifier&& m)
      {
         typedef typename get_index_type<ObjectType>::type index_type;
         get_mutable_index<index_type>().modify(obj, m);
      }

      template <typename ObjectType>
      void remove(const ObjectType& obj)
      {
         typedef typename get_index_type<ObjectType>::type index_type;
         return get_mutable_index<index_type>().remove(obj);
      }

      template <typename ObjectType, typename Constructor>
      const ObjectType& create(Constructor&& con)
      {
         typedef typename get_index_type<ObjectType>::type index_type;
         return get_mutable_index<index_type>().emplace(std::forward<Constructor>(con));
      }

      database_index_row_count_multiset row_count_per_index() const
      {
         database_index_row_count_multiset ret;
         for (const auto& ai_ptr : _index_map)
         {
            if (!ai_ptr)
               continue;
            ret.emplace(make_pair(ai_ptr->row_count(), ai_ptr->type_name()));
         }
         return ret;
      }

     private:
      /**
       * This is a sparse list of known indices kept to accelerate creation of undo sessions
       */
      vector<abstract_index*> _index_list;

      /**
       * This is a full map (size 2^16) of all possible index designed for constant time lookup
       */
      vector<unique_ptr<abstract_index>> _index_map;
   };

   template <typename Object, typename... Args>
   using shared_multi_index_container =
       boost::multi_index_container<Object, Args..., chainbase::node_allocator<Object>>;
}  // namespace chainbase
