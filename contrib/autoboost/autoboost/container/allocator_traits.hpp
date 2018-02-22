//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Pablo Halpern 2009. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2011-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////
#ifndef AUTOBOOST_CONTAINER_ALLOCATOR_ALLOCATOR_TRAITS_HPP
#define AUTOBOOST_CONTAINER_ALLOCATOR_ALLOCATOR_TRAITS_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif

#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/container/detail/config_begin.hpp>
#include <autoboost/container/detail/workaround.hpp>

// container
#include <autoboost/container/container_fwd.hpp>
#include <autoboost/container/detail/mpl.hpp>
#include <autoboost/container/detail/type_traits.hpp>  //is_empty
#include <autoboost/container/detail/placement_new.hpp>
#ifndef AUTOBOOST_CONTAINER_DETAIL_STD_FWD_HPP
#include <autoboost/container/detail/std_fwd.hpp>
#endif
// intrusive
#include <autoboost/intrusive/pointer_traits.hpp>
#include <autoboost/intrusive/detail/mpl.hpp>
// move
#include <autoboost/move/utility_core.hpp>
// move/detail
#if defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
#include <autoboost/move/detail/fwd_macros.hpp>
#endif
// other autoboost
#include <autoboost/static_assert.hpp>

#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME allocate
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEG namespace autoboost { namespace container { namespace container_detail {
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END   }}}
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_MIN 2
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_MAX 2
#include <autoboost/intrusive/detail/has_member_function_callable_with.hpp>

#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME destroy
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEG namespace autoboost { namespace container { namespace container_detail {
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END   }}}
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_MIN 1
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_MAX 1
#include <autoboost/intrusive/detail/has_member_function_callable_with.hpp>

#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME construct
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEG namespace autoboost { namespace container { namespace container_detail {
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END   }}}
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_MIN 1
#define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_MAX 9
#include <autoboost/intrusive/detail/has_member_function_callable_with.hpp>

#endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

namespace autoboost {
namespace container {

#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

template<class Allocator>
class small_vector_allocator;

namespace allocator_traits_detail {

AUTOBOOST_INTRUSIVE_HAS_STATIC_MEMBER_FUNC_SIGNATURE(has_max_size, max_size)
AUTOBOOST_INTRUSIVE_HAS_STATIC_MEMBER_FUNC_SIGNATURE(has_select_on_container_copy_construction, select_on_container_copy_construction)

}  //namespace allocator_traits_detail {

namespace container_detail {

//workaround needed for C++03 compilers with no construct()
//supporting rvalue references
template<class Allocator>
struct is_std_allocator
{  static const bool value = false; };

template<class T>
struct is_std_allocator< std::allocator<T> >
{  static const bool value = true; };

template<class T>
struct is_std_allocator< small_vector_allocator< std::allocator<T> > >
{  static const bool value = true; };

template<class Allocator>
struct is_not_std_allocator
{  static const bool value = !is_std_allocator<Allocator>::value; };

AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(pointer)
AUTOBOOST_INTRUSIVE_INSTANTIATE_EVAL_DEFAULT_TYPE_TMPLT(const_pointer)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(reference)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(const_reference)
AUTOBOOST_INTRUSIVE_INSTANTIATE_EVAL_DEFAULT_TYPE_TMPLT(void_pointer)
AUTOBOOST_INTRUSIVE_INSTANTIATE_EVAL_DEFAULT_TYPE_TMPLT(const_void_pointer)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(size_type)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(propagate_on_container_copy_assignment)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(propagate_on_container_move_assignment)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(propagate_on_container_swap)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(is_always_equal)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(difference_type)
AUTOBOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(is_partially_propagable)

}  //namespace container_detail {

#endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

//! The class template allocator_traits supplies a uniform interface to all allocator types.
//! This class is a C++03-compatible implementation of std::allocator_traits
template <typename Allocator>
struct allocator_traits
{
   //allocator_type
   typedef Allocator allocator_type;
   //value_type
   typedef typename allocator_type::value_type value_type;

   #if defined(AUTOBOOST_CONTAINER_DOXYGEN_INVOKED)
      //! Allocator::pointer if such a type exists; otherwise, value_type*
      //!
      typedef unspecified pointer;
      //! Allocator::const_pointer if such a type exists ; otherwise, pointer_traits<pointer>::rebind<const
      //!
      typedef see_documentation const_pointer;
      //! Non-standard extension
      //! Allocator::reference if such a type exists; otherwise, value_type&
      typedef see_documentation reference;
      //! Non-standard extension
      //! Allocator::const_reference if such a type exists ; otherwise, const value_type&
      typedef see_documentation const_reference;
      //! Allocator::void_pointer if such a type exists ; otherwise, pointer_traits<pointer>::rebind<void>.
      //!
      typedef see_documentation void_pointer;
      //! Allocator::const_void_pointer if such a type exists ; otherwis e, pointer_traits<pointer>::rebind<const
      //!
      typedef see_documentation const_void_pointer;
      //! Allocator::difference_type if such a type exists ; otherwise, pointer_traits<pointer>::difference_type.
      //!
      typedef see_documentation difference_type;
      //! Allocator::size_type if such a type exists ; otherwise, make_unsigned<difference_type>::type
      //!
      typedef see_documentation size_type;
      //! Allocator::propagate_on_container_copy_assignment if such a type exists, otherwise a type
      //! with an internal constant static boolean member <code>value</code> == false.
      typedef see_documentation propagate_on_container_copy_assignment;
      //! Allocator::propagate_on_container_move_assignment if such a type exists, otherwise a type
      //! with an internal constant static boolean member <code>value</code> == false.
      typedef see_documentation propagate_on_container_move_assignment;
      //! Allocator::propagate_on_container_swap if such a type exists, otherwise a type
      //! with an internal constant static boolean member <code>value</code> == false.
      typedef see_documentation propagate_on_container_swap;
      //! Allocator::is_always_equal if such a type exists, otherwise a type
      //! with an internal constant static boolean member <code>value</code> == is_empty<Allocator>::value
      typedef see_documentation is_always_equal;
      //! Allocator::is_partially_propagable if such a type exists, otherwise a type
      //! with an internal constant static boolean member <code>value</code> == false
      //! <b>Note</b>: Non-standard extension used to implement `small_vector_allocator`.
      typedef see_documentation is_partially_propagable;
      //! Defines an allocator: Allocator::rebind<T>::other if such a type exists; otherwise, Allocator<T, Args>
      //! if Allocator is a class template instantiation of the form Allocator<U, Args>, where Args is zero or
      //! more type arguments ; otherwise, the instantiation of rebind_alloc is ill-formed.
      //!
      //! In C++03 compilers <code>rebind_alloc</code> is a struct derived from an allocator
      //! deduced by previously detailed rules.
      template <class T> using rebind_alloc = see_documentation;

      //! In C++03 compilers <code>rebind_traits</code> is a struct derived from
      //! <code>allocator_traits<OtherAlloc></code>, where <code>OtherAlloc</code> is
      //! the allocator deduced by rules explained in <code>rebind_alloc</code>.
      template <class T> using rebind_traits = allocator_traits<rebind_alloc<T> >;

      //! Non-standard extension: Portable allocator rebind for C++03 and C++11 compilers.
      //! <code>type</code> is an allocator related to Allocator deduced deduced by rules explained in <code>rebind_alloc</code>.
      template <class T>
      struct portable_rebind_alloc
      {  typedef see_documentation type;  };
   #else
      //pointer
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(autoboost::container::container_detail::, Allocator,
         pointer, value_type*)
            pointer;
      //const_pointer
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_EVAL_DEFAULT(autoboost::container::container_detail::, Allocator,
         const_pointer, typename autoboost::intrusive::pointer_traits<pointer>::template
            rebind_pointer<const value_type>)
               const_pointer;
      //reference
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(autoboost::container::container_detail::, Allocator,
         reference, typename container_detail::unvoid_ref<value_type>::type)
            reference;
      //const_reference
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(autoboost::container::container_detail::, Allocator,
         const_reference, typename container_detail::unvoid_ref<const value_type>::type)
               const_reference;
      //void_pointer
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_EVAL_DEFAULT(autoboost::container::container_detail::, Allocator,
         void_pointer, typename autoboost::intrusive::pointer_traits<pointer>::template
            rebind_pointer<void>)
               void_pointer;
      //const_void_pointer
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_EVAL_DEFAULT(autoboost::container::container_detail::, Allocator,
         const_void_pointer, typename autoboost::intrusive::pointer_traits<pointer>::template
            rebind_pointer<const void>)
               const_void_pointer;
      //difference_type
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(autoboost::container::container_detail::, Allocator,
         difference_type, std::ptrdiff_t)
            difference_type;
      //size_type
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(autoboost::container::container_detail::, Allocator,
         size_type, std::size_t)
            size_type;
      //propagate_on_container_copy_assignment
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(autoboost::container::container_detail::, Allocator,
         propagate_on_container_copy_assignment, container_detail::false_type)
            propagate_on_container_copy_assignment;
      //propagate_on_container_move_assignment
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(autoboost::container::container_detail::, Allocator,
         propagate_on_container_move_assignment, container_detail::false_type)
            propagate_on_container_move_assignment;
      //propagate_on_container_swap
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(autoboost::container::container_detail::, Allocator,
         propagate_on_container_swap, container_detail::false_type)
            propagate_on_container_swap;
      //is_always_equal
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(autoboost::container::container_detail::, Allocator,
         is_always_equal, container_detail::is_empty<Allocator>)
            is_always_equal;
      //is_partially_propagable
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(autoboost::container::container_detail::, Allocator,
         is_partially_propagable, container_detail::false_type)
            is_partially_propagable;

      //rebind_alloc & rebind_traits
      #if !defined(AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES)
         //C++11
         template <typename T> using rebind_alloc  = typename autoboost::intrusive::pointer_rebind<Allocator, T>::type;
         template <typename T> using rebind_traits = allocator_traits< rebind_alloc<T> >;
      #else    // #if !defined(AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES)
         //Some workaround for C++03 or C++11 compilers with no template aliases
         template <typename T>
         struct rebind_alloc : autoboost::intrusive::pointer_rebind<Allocator,T>::type
         {
            typedef typename autoboost::intrusive::pointer_rebind<Allocator,T>::type Base;
            #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
               template <typename... Args>
               rebind_alloc(AUTOBOOST_FWD_REF(Args)... args) : Base(autoboost::forward<Args>(args)...) {}
            #else    // #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
               #define AUTOBOOST_CONTAINER_ALLOCATOR_TRAITS_REBIND_ALLOC(N) \
               AUTOBOOST_MOVE_TMPL_LT##N AUTOBOOST_MOVE_CLASS##N AUTOBOOST_MOVE_GT##N\
               explicit rebind_alloc(AUTOBOOST_MOVE_UREF##N) : Base(AUTOBOOST_MOVE_FWD##N){}\
               //
               AUTOBOOST_MOVE_ITERATE_0TO9(AUTOBOOST_CONTAINER_ALLOCATOR_TRAITS_REBIND_ALLOC)
               #undef AUTOBOOST_CONTAINER_ALLOCATOR_TRAITS_REBIND_ALLOC
            #endif   // #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
         };

         template <typename T>
         struct rebind_traits
            : allocator_traits<typename autoboost::intrusive::pointer_rebind<Allocator, T>::type>
         {};
      #endif   // #if !defined(AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES)

      //portable_rebind_alloc
      template <class T>
      struct portable_rebind_alloc
      {  typedef typename autoboost::intrusive::pointer_rebind<Allocator, T>::type type;  };
   #endif   //AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

   //! <b>Returns</b>: <code>a.allocate(n)</code>
   //!
   AUTOBOOST_CONTAINER_FORCEINLINE static pointer allocate(Allocator &a, size_type n)
   {  return a.allocate(n);  }

   //! <b>Returns</b>: <code>a.deallocate(p, n)</code>
   //!
   //! <b>Throws</b>: Nothing
   AUTOBOOST_CONTAINER_FORCEINLINE static void deallocate(Allocator &a, pointer p, size_type n)
   {  a.deallocate(p, n);  }

   //! <b>Effects</b>: calls <code>a.allocate(n, p)</code> if that call is well-formed;
   //! otherwise, invokes <code>a.allocate(n)</code>
   AUTOBOOST_CONTAINER_FORCEINLINE static pointer allocate(Allocator &a, size_type n, const_void_pointer p)
   {
      const bool value = autoboost::container::container_detail::
         has_member_function_callable_with_allocate
            <Allocator, const size_type, const const_void_pointer>::value;
      container_detail::bool_<value> flag;
      return allocator_traits::priv_allocate(flag, a, n, p);
   }

   //! <b>Effects</b>: calls <code>a.destroy(p)</code> if that call is well-formed;
   //! otherwise, invokes <code>p->~T()</code>.
   template<class T>
   AUTOBOOST_CONTAINER_FORCEINLINE static void destroy(Allocator &a, T*p) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {
      typedef T* destroy_pointer;
      const bool value = autoboost::container::container_detail::
         has_member_function_callable_with_destroy
            <Allocator, const destroy_pointer>::value;
      container_detail::bool_<value> flag;
      allocator_traits::priv_destroy(flag, a, p);
   }

   //! <b>Returns</b>: <code>a.max_size()</code> if that expression is well-formed; otherwise,
   //! <code>numeric_limits<size_type>::max()</code>.
   AUTOBOOST_CONTAINER_FORCEINLINE static size_type max_size(const Allocator &a) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {
      const bool value = allocator_traits_detail::has_max_size<Allocator, size_type (Allocator::*)() const>::value;
      container_detail::bool_<value> flag;
      return allocator_traits::priv_max_size(flag, a);
   }

   //! <b>Returns</b>: <code>a.select_on_container_copy_construction()</code> if that expression is well-formed;
   //! otherwise, a.
   AUTOBOOST_CONTAINER_FORCEINLINE static AUTOBOOST_CONTAINER_DOC1ST(Allocator,
      typename container_detail::if_c
         < allocator_traits_detail::has_select_on_container_copy_construction<Allocator AUTOBOOST_MOVE_I Allocator (Allocator::*)() const>::value
         AUTOBOOST_MOVE_I Allocator AUTOBOOST_MOVE_I const Allocator & >::type)
   select_on_container_copy_construction(const Allocator &a)
   {
      const bool value = allocator_traits_detail::has_select_on_container_copy_construction
         <Allocator, Allocator (Allocator::*)() const>::value;
      container_detail::bool_<value> flag;
      return allocator_traits::priv_select_on_container_copy_construction(flag, a);
   }

   #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES) || defined(AUTOBOOST_CONTAINER_DOXYGEN_INVOKED)
      //! <b>Effects</b>: calls <code>a.construct(p, std::forward<Args>(args)...)</code> if that call is well-formed;
      //! otherwise, invokes <code>::new (static_cast<void*>(p)) T(std::forward<Args>(args)...)</code>
      template <class T, class ...Args>
      AUTOBOOST_CONTAINER_FORCEINLINE static void construct(Allocator & a, T* p, AUTOBOOST_FWD_REF(Args)... args)
      {
         static const bool value = ::autoboost::move_detail::and_
            < container_detail::is_not_std_allocator<Allocator>
            , autoboost::container::container_detail::has_member_function_callable_with_construct
                  < Allocator, T*, Args... >
            >::value;
         container_detail::bool_<value> flag;
         allocator_traits::priv_construct(flag, a, p, ::autoboost::forward<Args>(args)...);
      }
   #endif

   //! <b>Returns</b>: <code>a.storage_is_unpropagable(p)</code> if is_partially_propagable::value is true; otherwise,
   //! <code>false</code>.
   AUTOBOOST_CONTAINER_FORCEINLINE static bool storage_is_unpropagable(const Allocator &a, pointer p) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {
      container_detail::bool_<is_partially_propagable::value> flag;
      return allocator_traits::priv_storage_is_unpropagable(flag, a, p);
   }

   //! <b>Returns</b>: <code>true</code> if <code>is_always_equal::value == true</code>, otherwise,
   //! <code>a == b</code>.
   AUTOBOOST_CONTAINER_FORCEINLINE static bool equal(const Allocator &a, const Allocator &b) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {
      container_detail::bool_<is_always_equal::value> flag;
      return allocator_traits::priv_equal(flag, a, b);
   }

   #if !defined(AUTOBOOST_CONTAINER_DOXYGEN_INVOKED)
   private:
   AUTOBOOST_CONTAINER_FORCEINLINE static pointer priv_allocate(container_detail::true_type, Allocator &a, size_type n, const_void_pointer p)
   {  return a.allocate(n, p);  }

   AUTOBOOST_CONTAINER_FORCEINLINE static pointer priv_allocate(container_detail::false_type, Allocator &a, size_type n, const_void_pointer)
   {  return a.allocate(n);  }

   template<class T>
   AUTOBOOST_CONTAINER_FORCEINLINE static void priv_destroy(container_detail::true_type, Allocator &a, T* p) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  a.destroy(p);  }

   template<class T>
   AUTOBOOST_CONTAINER_FORCEINLINE static void priv_destroy(container_detail::false_type, Allocator &, T* p) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  p->~T(); (void)p;  }

   AUTOBOOST_CONTAINER_FORCEINLINE static size_type priv_max_size(container_detail::true_type, const Allocator &a) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return a.max_size();  }

   AUTOBOOST_CONTAINER_FORCEINLINE static size_type priv_max_size(container_detail::false_type, const Allocator &) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return size_type(-1)/sizeof(value_type);  }

   AUTOBOOST_CONTAINER_FORCEINLINE static Allocator priv_select_on_container_copy_construction(container_detail::true_type, const Allocator &a)
   {  return a.select_on_container_copy_construction();  }

   AUTOBOOST_CONTAINER_FORCEINLINE static const Allocator &priv_select_on_container_copy_construction(container_detail::false_type, const Allocator &a) AUTOBOOST_NOEXCEPT_OR_NOTHROW
   {  return a;  }

   #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
      template<class T, class ...Args>
      AUTOBOOST_CONTAINER_FORCEINLINE static void priv_construct(container_detail::true_type, Allocator &a, T *p, AUTOBOOST_FWD_REF(Args) ...args)
      {  a.construct( p, ::autoboost::forward<Args>(args)...);  }

      template<class T, class ...Args>
      AUTOBOOST_CONTAINER_FORCEINLINE static void priv_construct(container_detail::false_type, Allocator &, T *p, AUTOBOOST_FWD_REF(Args) ...args)
      {  ::new((void*)p, autoboost_container_new_t()) T(::autoboost::forward<Args>(args)...); }
   #else // #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
      public:

      #define AUTOBOOST_CONTAINER_ALLOCATOR_TRAITS_CONSTRUCT_IMPL(N) \
      template<class T AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_CLASS##N >\
      AUTOBOOST_CONTAINER_FORCEINLINE static void construct(Allocator &a, T *p AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_UREF##N)\
      {\
         static const bool value = ::autoboost::move_detail::and_ \
            < container_detail::is_not_std_allocator<Allocator> \
            , autoboost::container::container_detail::has_member_function_callable_with_construct \
                  < Allocator, T* AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_FWD_T##N > \
            >::value; \
         container_detail::bool_<value> flag;\
         (priv_construct)(flag, a, p AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_FWD##N);\
      }\
      //
      AUTOBOOST_MOVE_ITERATE_0TO8(AUTOBOOST_CONTAINER_ALLOCATOR_TRAITS_CONSTRUCT_IMPL)
      #undef AUTOBOOST_CONTAINER_ALLOCATOR_TRAITS_CONSTRUCT_IMPL

      private:
      /////////////////////////////////
      // priv_construct
      /////////////////////////////////
      #define AUTOBOOST_CONTAINER_ALLOCATOR_TRAITS_PRIV_CONSTRUCT_IMPL(N) \
      template<class T AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_CLASS##N >\
      AUTOBOOST_CONTAINER_FORCEINLINE static void priv_construct(container_detail::true_type, Allocator &a, T *p AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_UREF##N)\
      {  a.construct( p AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_FWD##N );  }\
      \
      template<class T AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_CLASS##N >\
      AUTOBOOST_CONTAINER_FORCEINLINE static void priv_construct(container_detail::false_type, Allocator &, T *p AUTOBOOST_MOVE_I##N AUTOBOOST_MOVE_UREF##N)\
      {  ::new((void*)p, autoboost_container_new_t()) T(AUTOBOOST_MOVE_FWD##N); }\
      //
      AUTOBOOST_MOVE_ITERATE_0TO8(AUTOBOOST_CONTAINER_ALLOCATOR_TRAITS_PRIV_CONSTRUCT_IMPL)
      #undef AUTOBOOST_CONTAINER_ALLOCATOR_TRAITS_PRIV_CONSTRUCT_IMPL

   #endif   // #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

   template<class T>
   AUTOBOOST_CONTAINER_FORCEINLINE static void priv_construct(container_detail::false_type, Allocator &, T *p, const ::autoboost::container::default_init_t&)
   {  ::new((void*)p, autoboost_container_new_t()) T; }

   AUTOBOOST_CONTAINER_FORCEINLINE static bool priv_storage_is_unpropagable(container_detail::true_type, const Allocator &a, pointer p)
   {  return a.storage_is_unpropagable(p);  }

   AUTOBOOST_CONTAINER_FORCEINLINE static bool priv_storage_is_unpropagable(container_detail::false_type, const Allocator &, pointer)
   {  return false;  }

   AUTOBOOST_CONTAINER_FORCEINLINE static bool priv_equal(container_detail::true_type,  const Allocator &, const Allocator &)
   {  return true;  }

   AUTOBOOST_CONTAINER_FORCEINLINE static bool priv_equal(container_detail::false_type, const Allocator &a, const Allocator &b)
   {  return a == b;  }

   #endif   //#if defined(AUTOBOOST_CONTAINER_DOXYGEN_INVOKED)
};

}  //namespace container {
}  //namespace autoboost {

#include <autoboost/container/detail/config_end.hpp>

#endif // ! defined(AUTOBOOST_CONTAINER_ALLOCATOR_ALLOCATOR_TRAITS_HPP)
