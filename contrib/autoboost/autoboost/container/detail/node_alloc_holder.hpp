//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_CONTAINER_DETAIL_NODE_ALLOC_HPP_
#define AUTOBOOST_CONTAINER_DETAIL_NODE_ALLOC_HPP_

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif

#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/container/detail/config_begin.hpp>
#include <autoboost/container/detail/workaround.hpp>

// container
#include <autoboost/container/allocator_traits.hpp>
// container/detail
#include <autoboost/container/detail/addressof.hpp>
#include <autoboost/container/detail/alloc_helpers.hpp>
#include <autoboost/container/detail/allocator_version_traits.hpp>
#include <autoboost/container/detail/construct_in_place.hpp>
#include <autoboost/container/detail/destroyers.hpp>
#include <autoboost/container/detail/iterator_to_raw_pointer.hpp>
#include <autoboost/container/detail/mpl.hpp>
#include <autoboost/container/detail/placement_new.hpp>
#include <autoboost/container/detail/to_raw_pointer.hpp>
#include <autoboost/container/detail/type_traits.hpp>
#include <autoboost/container/detail/version_type.hpp>
// intrusive
#include <autoboost/intrusive/detail/mpl.hpp>
#include <autoboost/intrusive/options.hpp>
// move
#include <autoboost/move/utility_core.hpp>
#if defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
#include <autoboost/move/detail/fwd_macros.hpp>
#endif
// other
#include <autoboost/core/no_exceptions_support.hpp>


namespace autoboost {
namespace container {
namespace container_detail {

AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(value_compare)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(predicate_type)

template<class Allocator, class ICont>
struct node_alloc_holder
{
   //If the intrusive container is an associative container, obtain the predicate, which will
   //be of type node_compare<>. If not an associative container value_compare will be a "nat" type.
   typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(autoboost::container::container_detail::, ICont,
      value_compare, container_detail::nat)                       intrusive_value_compare;
   //In that case obtain the value predicate from the node predicate via predicate_type
   //if intrusive_value_compare is node_compare<>, nat otherwise
   typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(autoboost::container::container_detail::, intrusive_value_compare,
      predicate_type, container_detail::nat)                      value_compare;

   typedef allocator_traits<Allocator>                                    allocator_traits_type;
   typedef typename allocator_traits_type::value_type             value_type;
   typedef ICont                                                  intrusive_container;
   typedef typename ICont::value_type                             Node;
   typedef typename allocator_traits_type::template
      portable_rebind_alloc<Node>::type                           NodeAlloc;
   typedef allocator_traits<NodeAlloc>                            node_allocator_traits_type;
   typedef container_detail::allocator_version_traits<NodeAlloc>  node_allocator_version_traits_type;
   typedef Allocator                                                      ValAlloc;
   typedef typename node_allocator_traits_type::pointer           NodePtr;
   typedef container_detail::scoped_deallocator<NodeAlloc>        Deallocator;
   typedef typename node_allocator_traits_type::size_type         size_type;
   typedef typename node_allocator_traits_type::difference_type   difference_type;
   typedef container_detail::integral_constant<unsigned,
      autoboost::container::container_detail::
         version<NodeAlloc>::value>                   alloc_version;
   typedef typename ICont::iterator                   icont_iterator;
   typedef typename ICont::const_iterator             icont_citerator;
   typedef allocator_destroyer<NodeAlloc>             Destroyer;
   typedef allocator_traits<NodeAlloc>                NodeAllocTraits;
   typedef allocator_version_traits<NodeAlloc>        AllocVersionTraits;

   private:
   AUTOBOOST_COPYABLE_AND_MOVABLE(node_alloc_holder)

   public:

   //Constructors for sequence containers
   node_alloc_holder()
      : members_()
   {}

   explicit node_alloc_holder(const ValAlloc &a)
      : members_(a)
   {}

   explicit node_alloc_holder(const node_alloc_holder &x)
      : members_(NodeAllocTraits::select_on_container_copy_construction(x.node_alloc()))
   {}

   explicit node_alloc_holder(AUTOBOOST_RV_REF(node_alloc_holder) x)
      : members_(autoboost::move(x.node_alloc()))
   {  this->icont().swap(x.icont());  }

   //Constructors for associative containers
   explicit node_alloc_holder(const value_compare &c, const ValAlloc &a)
      : members_(a, c)
   {}

   explicit node_alloc_holder(const value_compare &c, const node_alloc_holder &x)
      : members_(NodeAllocTraits::select_on_container_copy_construction(x.node_alloc()), c)
   {}

   explicit node_alloc_holder(const value_compare &c)
      : members_(c)
   {}

   //helpers for move assignments
   explicit node_alloc_holder(AUTOBOOST_RV_REF(node_alloc_holder) x, const value_compare &c)
      : members_(autoboost::move(x.node_alloc()), c)
   {  this->icont().swap(x.icont());  }

   void copy_assign_alloc(const node_alloc_holder &x)
   {
      container_detail::bool_<allocator_traits_type::propagate_on_container_copy_assignment::value> flag;
      container_detail::assign_alloc( static_cast<NodeAlloc &>(this->members_)
                                    , static_cast<const NodeAlloc &>(x.members_), flag);
   }

   void move_assign_alloc( node_alloc_holder &x)
   {
      container_detail::bool_<allocator_traits_type::propagate_on_container_move_assignment::value> flag;
      container_detail::move_alloc( static_cast<NodeAlloc &>(this->members_)
                                  , static_cast<NodeAlloc &>(x.members_), flag);
   }

   ~node_alloc_holder()
   {  this->clear(alloc_version()); }

   size_type max_size() const
   {  return allocator_traits_type::max_size(this->node_alloc());  }

   NodePtr allocate_one()
   {  return AllocVersionTraits::allocate_one(this->node_alloc());   }

   void deallocate_one(const NodePtr &p)
   {  AllocVersionTraits::deallocate_one(this->node_alloc(), p);  }

   #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

   template<class ...Args>
   NodePtr create_node(Args &&...args)
   {
      NodePtr p = this->allocate_one();
      Deallocator node_deallocator(p, this->node_alloc());
      allocator_traits<NodeAlloc>::construct
         ( this->node_alloc()
         , container_detail::addressof(p->m_data), autoboost::forward<Args>(args)...);
      node_deallocator.release();
      //This does not throw
      typedef typename Node::hook_type hook_type;
      ::new(static_cast<hook_type*>(container_detail::to_raw_pointer(p)), autoboost_container_new_t()) hook_type;
      return (p);
   }

   #else //defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

   #define AUTOBOOST_CONTAINER_NODE_ALLOC_HOLDER_CONSTRUCT_IMPL(N) \
   AUTOBOOST_MOVE_TMPL_LT##N AUTOBOOST_MOVE_CLASS##N AUTOBOOST_MOVE_GT##N \
   NodePtr create_node(AUTOBOOST_MOVE_UREF##N)\
   {\
      NodePtr p = this->allocate_one();\
      Deallocator node_deallocator(p, this->node_alloc());\
      allocator_traits<NodeAlloc>::construct\
         ( this->node_alloc()\
         , container_detail::addressof(p->m_data)\
          AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_FWD##N);\
      node_deallocator.release();\
      typedef typename Node::hook_type hook_type;\
      ::new(static_cast<hook_type*>(container_detail::to_raw_pointer(p)), autoboost_container_new_t()) hook_type;\
      return (p);\
   }\
   //
   AUTOBOOST_MOVE_ITERATE_0TO9(AUTOBOOST_CONTAINER_NODE_ALLOC_HOLDER_CONSTRUCT_IMPL)
   #undef AUTOBOOST_CONTAINER_NODE_ALLOC_HOLDER_CONSTRUCT_IMPL

   #endif   // !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

   template<class It>
   NodePtr create_node_from_it(const It &it)
   {
      NodePtr p = this->allocate_one();
      Deallocator node_deallocator(p, this->node_alloc());
      ::autoboost::container::construct_in_place(this->node_alloc(), container_detail::addressof(p->m_data), it);
      node_deallocator.release();
      //This does not throw
      typedef typename Node::hook_type hook_type;
      ::new(static_cast<hook_type*>(container_detail::to_raw_pointer(p)), autoboost_container_new_t()) hook_type;
      return (p);
   }

   void destroy_node(const NodePtr &nodep)
   {
      allocator_traits<NodeAlloc>::destroy(this->node_alloc(), container_detail::to_raw_pointer(nodep));
      this->deallocate_one(nodep);
   }

   void swap(node_alloc_holder &x)
   {
      this->icont().swap(x.icont());
      container_detail::bool_<allocator_traits_type::propagate_on_container_swap::value> flag;
      container_detail::swap_alloc(this->node_alloc(), x.node_alloc(), flag);
   }

   template<class FwdIterator, class Inserter>
   void allocate_many_and_construct
      (FwdIterator beg, difference_type n, Inserter inserter)
   {
      if(n){
         typedef typename node_allocator_version_traits_type::multiallocation_chain multiallocation_chain;

         //Try to allocate memory in a single block
         typedef typename multiallocation_chain::iterator multialloc_iterator;
         multiallocation_chain mem;
         NodeAlloc &nalloc = this->node_alloc();
         node_allocator_version_traits_type::allocate_individual(nalloc, n, mem);
         multialloc_iterator itbeg(mem.begin()), itlast(mem.last());
         mem.clear();
         Node *p = 0;
         AUTOBOOST_TRY{
            Deallocator node_deallocator(NodePtr(), nalloc);
            container_detail::scoped_destructor<NodeAlloc> sdestructor(nalloc, 0);
            while(n--){
               p = container_detail::iterator_to_raw_pointer(itbeg);
               node_deallocator.set(p);
               ++itbeg;
               //This can throw
               autoboost::container::construct_in_place(nalloc, container_detail::addressof(p->m_data), beg);
               sdestructor.set(p);
               ++beg;
               //This does not throw
               typedef typename Node::hook_type hook_type;
               ::new(static_cast<hook_type*>(p), autoboost_container_new_t()) hook_type;
               //This can throw in some containers (predicate might throw).
               //(sdestructor will destruct the node and node_deallocator will deallocate it in case of exception)
               inserter(*p);
               sdestructor.set(0);
            }
            sdestructor.release();
            node_deallocator.release();
         }
         AUTOBOOST_CATCH(...){
            mem.incorporate_after(mem.last(), &*itbeg, &*itlast, n);
            node_allocator_version_traits_type::deallocate_individual(this->node_alloc(), mem);
            AUTOBOOST_RETHROW
         }
         AUTOBOOST_CATCH_END
      }
   }

   void clear(version_1)
   {  this->icont().clear_and_dispose(Destroyer(this->node_alloc()));   }

   void clear(version_2)
   {
      typename NodeAlloc::multiallocation_chain chain;
      allocator_destroyer_and_chain_builder<NodeAlloc> builder(this->node_alloc(), chain);
      this->icont().clear_and_dispose(builder);
      //AUTOBOOST_STATIC_ASSERT((::autoboost::has_move_emulation_enabled<typename NodeAlloc::multiallocation_chain>::value == true));
      if(!chain.empty())
         this->node_alloc().deallocate_individual(chain);
   }

   icont_iterator erase_range(const icont_iterator &first, const icont_iterator &last, version_1)
   {  return this->icont().erase_and_dispose(first, last, Destroyer(this->node_alloc())); }

   icont_iterator erase_range(const icont_iterator &first, const icont_iterator &last, version_2)
   {
      typedef typename NodeAlloc::multiallocation_chain multiallocation_chain;
      NodeAlloc & nalloc = this->node_alloc();
      multiallocation_chain chain;
      allocator_destroyer_and_chain_builder<NodeAlloc> chain_builder(nalloc, chain);
      icont_iterator ret_it = this->icont().erase_and_dispose(first, last, chain_builder);
      nalloc.deallocate_individual(chain);
      return ret_it;
   }

   template<class Key, class Comparator>
   size_type erase_key(const Key& k, const Comparator &comp, version_1)
   {  return this->icont().erase_and_dispose(k, comp, Destroyer(this->node_alloc())); }

   template<class Key, class Comparator>
   size_type erase_key(const Key& k, const Comparator &comp, version_2)
   {
      allocator_multialloc_chain_node_deallocator<NodeAlloc> chain_holder(this->node_alloc());
      return this->icont().erase_and_dispose(k, comp, chain_holder.get_chain_builder());
   }

   protected:
   struct cloner
   {
      explicit cloner(node_alloc_holder &holder)
         :  m_holder(holder)
      {}

      NodePtr operator()(const Node &other) const
      {  return m_holder.create_node(other.m_data);  }

      node_alloc_holder &m_holder;
   };

   struct move_cloner
   {
      move_cloner(node_alloc_holder &holder)
         :  m_holder(holder)
      {}

      NodePtr operator()(Node &other)
      {  //Use m_data instead of get_data to allow moving const key in [multi]map
         return m_holder.create_node(::autoboost::move(other.m_data));
      }

      node_alloc_holder &m_holder;
   };

   struct members_holder
      :  public NodeAlloc
   {
      private:
      members_holder(const members_holder&);
      members_holder & operator=(const members_holder&);

      public:
      members_holder()
         : NodeAlloc(), m_icont()
      {}

      template<class ConvertibleToAlloc>
      explicit members_holder(AUTOBOOST_FWD_REF(ConvertibleToAlloc) c2alloc)
         :  NodeAlloc(autoboost::forward<ConvertibleToAlloc>(c2alloc))
         , m_icont()
      {}

      template<class ConvertibleToAlloc>
      members_holder(AUTOBOOST_FWD_REF(ConvertibleToAlloc) c2alloc, const value_compare &c)
         :  NodeAlloc(autoboost::forward<ConvertibleToAlloc>(c2alloc))
         , m_icont(typename ICont::key_compare(c))
      {}

      explicit members_holder(const value_compare &c)
         : NodeAlloc()
         , m_icont(typename ICont::key_compare(c))
      {}

      //The intrusive container
      ICont m_icont;
   };

   ICont &non_const_icont() const
   {  return const_cast<ICont&>(this->members_.m_icont);   }

   ICont &icont()
   {  return this->members_.m_icont;   }

   const ICont &icont() const
   {  return this->members_.m_icont;   }

   NodeAlloc &node_alloc()
   {  return static_cast<NodeAlloc &>(this->members_);   }

   const NodeAlloc &node_alloc() const
   {  return static_cast<const NodeAlloc &>(this->members_);   }

   members_holder members_;
};

}  //namespace container_detail {
}  //namespace container {
}  //namespace autoboost {

#include <autoboost/container/detail/config_end.hpp>

#endif // AUTOBOOST_CONTAINER_DETAIL_NODE_ALLOC_HPP_
