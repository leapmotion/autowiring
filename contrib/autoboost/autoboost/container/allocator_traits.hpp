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

#if defined(_MSC_VER)
#  pragma once
#endif

#include <autoboost/container/detail/config_begin.hpp>
#include <autoboost/container/detail/workaround.hpp>
#include <autoboost/container/container_fwd.hpp>
#include <autoboost/intrusive/pointer_traits.hpp>
#include <autoboost/intrusive/detail/memory_util.hpp>
#include <autoboost/container/detail/memory_util.hpp>
#include <autoboost/container/detail/mpl.hpp>
#include <autoboost/container/detail/placement_new.hpp>
#include <autoboost/move/utility_core.hpp>

#if defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
#include <autoboost/container/detail/preprocessor.hpp>
#endif



namespace autoboost {
namespace container {
namespace allocator_traits_detail {

}

#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

namespace container_detail {

//workaround needed for C++03 compilers with no construct()
//supporting rvalue references
template<class A>
struct is_std_allocator
{  static const bool value = false; };

template<class T>
struct is_std_allocator< std::allocator<T> >
{  static const bool value = true; };

}  //namespace container_detail {

#endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

//! The class template allocator_traits supplies a uniform interface to all allocator types.
//! This class is a C++03-compatible implementation of std::allocator_traits
template <typename Alloc>
struct allocator_traits
{
   //allocator_type
   typedef Alloc allocator_type;
   //value_type
   typedef typename Alloc::value_type         value_type;

   #if defined(AUTOBOOST_CONTAINER_DOXYGEN_INVOKED)
      //! Alloc::pointer if such a type exists; otherwise, value_type*
      //!
      typedef unspecified pointer;
      //! Alloc::const_pointer if such a type exists ; otherwise, pointer_traits<pointer>::rebind<const
      //!
      typedef see_documentation const_pointer;
      //! Non-standard extension
      //! Alloc::reference if such a type exists; otherwise, value_type&
      typedef see_documentation reference;
      //! Non-standard extension
      //! Alloc::const_reference if such a type exists ; otherwise, const value_type&
      typedef see_documentation const_reference;
      //! Alloc::void_pointer if such a type exists ; otherwise, pointer_traits<pointer>::rebind<void>.
      //!
      typedef see_documentation void_pointer;
      //! Alloc::const_void_pointer if such a type exists ; otherwis e, pointer_traits<pointer>::rebind<const
      //!
      typedef see_documentation const_void_pointer;
      //! Alloc::difference_type if such a type exists ; otherwise, pointer_traits<pointer>::difference_type.
      //!
      typedef see_documentation difference_type;
      //! Alloc::size_type if such a type exists ; otherwise, make_unsigned<difference_type>::type
      //!
      typedef see_documentation size_type;
      //! Alloc::propagate_on_container_copy_assignment if such a type exists, otherwise an integral_constant
      //! type with internal constant static member <code>value</code> == false.
      typedef see_documentation propagate_on_container_copy_assignment;
      //! Alloc::propagate_on_container_move_assignment if such a type exists, otherwise an integral_constant
      //! type with internal constant static member <code>value</code> == false.
      typedef see_documentation propagate_on_container_move_assignment;
      //! Alloc::propagate_on_container_swap if such a type exists, otherwise an integral_constant
      //! type with internal constant static member <code>value</code> == false.
      typedef see_documentation propagate_on_container_swap;
      //! Defines an allocator: Alloc::rebind<T>::other if such a type exists; otherwise, Alloc<T, Args>
      //! if Alloc is a class template instantiation of the form Alloc<U, Args>, where Args is zero or
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
      //! <code>type</code> is an allocator related to Alloc deduced deduced by rules explained in <code>rebind_alloc</code>.
      template <class T>
      struct portable_rebind_alloc
      {  typedef see_documentation type;  };
   #else
      //pointer
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(autoboost::container::container_detail::, Alloc,
         pointer, value_type*)
            pointer;
      //const_pointer
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_EVAL_DEFAULT(autoboost::container::container_detail::, Alloc,
         const_pointer, typename autoboost::intrusive::pointer_traits<pointer>::template
            rebind_pointer<const value_type>)
               const_pointer;
      //reference
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(autoboost::container::container_detail::, Alloc,
         reference, typename container_detail::unvoid<value_type>::type&)
            reference;
      //const_reference
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(autoboost::container::container_detail::, Alloc,
         const_reference, const typename container_detail::unvoid<value_type>::type&)
               const_reference;
      //void_pointer
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_EVAL_DEFAULT(autoboost::container::container_detail::, Alloc,
         void_pointer, typename autoboost::intrusive::pointer_traits<pointer>::template
            rebind_pointer<void>)
               void_pointer;
      //const_void_pointer
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_EVAL_DEFAULT(autoboost::container::container_detail::, Alloc,
         const_void_pointer, typename autoboost::intrusive::pointer_traits<pointer>::template
            rebind_pointer<const void>)
               const_void_pointer;
      //difference_type
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(autoboost::container::container_detail::, Alloc,
         difference_type, std::ptrdiff_t)
            difference_type;
      //size_type
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(autoboost::container::container_detail::, Alloc,
         size_type, std::size_t)
            size_type;
      //propagate_on_container_copy_assignment
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(autoboost::container::container_detail::, Alloc,
         propagate_on_container_copy_assignment, container_detail::false_type)
            propagate_on_container_copy_assignment;
      //propagate_on_container_move_assignment
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(autoboost::container::container_detail::, Alloc,
         propagate_on_container_move_assignment, container_detail::false_type)
            propagate_on_container_move_assignment;
      //propagate_on_container_swap
      typedef AUTOBOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(autoboost::container::container_detail::, Alloc,
         propagate_on_container_swap, container_detail::false_type)
            propagate_on_container_swap;

      #if !defined(AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES)
         //C++11
         template <typename T> using rebind_alloc  = typename autoboost::intrusive::pointer_rebind<Alloc, T>::type;
         template <typename T> using rebind_traits = allocator_traits< rebind_alloc<T> >;
      #else    // #if !defined(AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES)
         //Some workaround for C++03 or C++11 compilers with no template aliases
         template <typename T>
         struct rebind_alloc : autoboost::intrusive::pointer_rebind<Alloc,T>::type
         {
            typedef typename autoboost::intrusive::pointer_rebind<Alloc,T>::type Base;
            #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
            template <typename... Args>
            rebind_alloc(AUTOBOOST_FWD_REF(Args)... args)
               : Base(autoboost::forward<Args>(args)...)
            {}
            #else    // #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
            #define AUTOBOOST_PP_LOCAL_MACRO(n)                                                        \
            AUTOBOOST_PP_EXPR_IF(n, template<) AUTOBOOST_PP_ENUM_PARAMS(n, class P) AUTOBOOST_PP_EXPR_IF(n, >) \
            rebind_alloc(AUTOBOOST_PP_ENUM(n, AUTOBOOST_CONTAINER_PP_PARAM_LIST, _))                       \
               : Base(AUTOBOOST_PP_ENUM(n, AUTOBOOST_CONTAINER_PP_PARAM_FORWARD, _))                       \
            {}                                                                                     \
            //
            #define AUTOBOOST_PP_LOCAL_LIMITS (0, AUTOBOOST_CONTAINER_MAX_CONSTRUCTOR_PARAMETERS)
            #include AUTOBOOST_PP_LOCAL_ITERATE()
            #endif   // #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
         };

         template <typename T>
         struct rebind_traits
            : allocator_traits<typename autoboost::intrusive::pointer_rebind<Alloc, T>::type>
         {};
      #endif   // #if !defined(AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES)
      template <class T>
      struct portable_rebind_alloc
      {  typedef typename autoboost::intrusive::pointer_rebind<Alloc, T>::type type;  };
   #endif   //AUTOBOOST_CONTAINER_DOXYGEN_INVOKED

   //! <b>Returns</b>: <code>a.allocate(n)</code>
   //!
   static pointer allocate(Alloc &a, size_type n)
   {  return a.allocate(n);  }

   //! <b>Returns</b>: <code>a.deallocate(p, n)</code>
   //!
   //! <b>Throws</b>: Nothing
   static void deallocate(Alloc &a, pointer p, size_type n)
   {  a.deallocate(p, n);  }

   //! <b>Effects</b>: calls <code>a.allocate(n, p)</code> if that call is well-formed;
   //! otherwise, invokes <code>a.allocate(n)</code>
   static pointer allocate(Alloc &a, size_type n, const_void_pointer p)
   {
      const bool value = autoboost::container::container_detail::
         has_member_function_callable_with_allocate
            <Alloc, const size_type, const const_void_pointer>::value;
      container_detail::bool_<value> flag;
      return allocator_traits::priv_allocate(flag, a, n, p);
   }

   //! <b>Effects</b>: calls <code>a.destroy(p)</code> if that call is well-formed;
   //! otherwise, invokes <code>p->~T()</code>.
   template<class T>
   static void destroy(Alloc &a, T*p) AUTOBOOST_CONTAINER_NOEXCEPT
   {
      typedef T* destroy_pointer;
      const bool value = autoboost::container::container_detail::
         has_member_function_callable_with_destroy
            <Alloc, const destroy_pointer>::value;
      container_detail::bool_<value> flag;
      allocator_traits::priv_destroy(flag, a, p);
   }

   //! <b>Returns</b>: <code>a.max_size()</code> if that expression is well-formed; otherwise,
   //! <code>numeric_limits<size_type>::max()</code>.
   static size_type max_size(const Alloc &a) AUTOBOOST_CONTAINER_NOEXCEPT
   {
      const bool value = autoboost::container::container_detail::
         has_member_function_callable_with_max_size
            <const Alloc>::value;
      container_detail::bool_<value> flag;
      return allocator_traits::priv_max_size(flag, a);
   }

   //! <b>Returns</b>: <code>a.select_on_container_copy_construction()</code> if that expression is well-formed;
   //! otherwise, a.
   static
   #if !defined(AUTOBOOST_CONTAINER_DOXYGEN_INVOKED)
   typename container_detail::if_c
      <  autoboost::container::container_detail::
                  has_member_function_callable_with_select_on_container_copy_construction
                     <const Alloc>::value
      , Alloc
      , const Alloc &
      >::type
   #else
   Alloc
   #endif
   select_on_container_copy_construction(const Alloc &a)
   {
      const bool value = autoboost::container::container_detail::
         has_member_function_callable_with_select_on_container_copy_construction
            <const Alloc>::value;
      container_detail::bool_<value> flag;
      return allocator_traits::priv_select_on_container_copy_construction(flag, a);
   }

   #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES) || defined(AUTOBOOST_CONTAINER_DOXYGEN_INVOKED)
      //! <b>Effects</b>: calls <code>a.construct(p, std::forward<Args>(args)...)</code> if that call is well-formed;
      //! otherwise, invokes <code>::new (static_cast<void*>(p)) T(std::forward<Args>(args)...)</code>
      template <class T, class ...Args>
      static void construct(Alloc & a, T* p, AUTOBOOST_FWD_REF(Args)... args)
      {
         container_detail::bool_<container_detail::is_std_allocator<Alloc>::value> flag;
         allocator_traits::priv_construct(flag, a, p, ::autoboost::forward<Args>(args)...);
      }
   #endif
   #ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
   #if !defined(AUTOBOOST_CONTAINER_DOXYGEN_INVOKED)
      private:
      static pointer priv_allocate(container_detail::true_type, Alloc &a, size_type n, const_void_pointer p)
      {  return a.allocate(n, p);  }

      static pointer priv_allocate(container_detail::false_type, Alloc &a, size_type n, const_void_pointer)
      {  return allocator_traits::allocate(a, n);  }

      template<class T>
      static void priv_destroy(container_detail::true_type, Alloc &a, T* p) AUTOBOOST_CONTAINER_NOEXCEPT
      {  a.destroy(p);  }

      template<class T>
      static void priv_destroy(container_detail::false_type, Alloc &, T* p) AUTOBOOST_CONTAINER_NOEXCEPT
      {  p->~T(); (void)p;  }

      static size_type priv_max_size(container_detail::true_type, const Alloc &a) AUTOBOOST_CONTAINER_NOEXCEPT
      {  return a.max_size();  }

      static size_type priv_max_size(container_detail::false_type, const Alloc &) AUTOBOOST_CONTAINER_NOEXCEPT
      {  return size_type(-1);  }

      static Alloc priv_select_on_container_copy_construction(container_detail::true_type, const Alloc &a)
      {  return a.select_on_container_copy_construction();  }

      static const Alloc &priv_select_on_container_copy_construction(container_detail::false_type, const Alloc &a) AUTOBOOST_CONTAINER_NOEXCEPT
      {  return a;  }

      #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
         template<class T, class ...Args>
         static void priv_construct(container_detail::false_type, Alloc &a, T *p, AUTOBOOST_FWD_REF(Args) ...args)
         {
            const bool value = autoboost::container::container_detail::
                  has_member_function_callable_with_construct
                     < Alloc, T*, Args... >::value;
            container_detail::bool_<value> flag;
            priv_construct_dispatch2(flag, a, p, ::autoboost::forward<Args>(args)...);
         }

         template<class T, class ...Args>
         static void priv_construct(container_detail::true_type, Alloc &a, T *p, AUTOBOOST_FWD_REF(Args) ...args)
         {
            priv_construct_dispatch2(container_detail::false_type(), a, p, ::autoboost::forward<Args>(args)...);
         }

         template<class T, class ...Args>
         static void priv_construct_dispatch2(container_detail::true_type, Alloc &a, T *p, AUTOBOOST_FWD_REF(Args) ...args)
         {  a.construct( p, ::autoboost::forward<Args>(args)...);  }

         template<class T, class ...Args>
         static void priv_construct_dispatch2(container_detail::false_type, Alloc &, T *p, AUTOBOOST_FWD_REF(Args) ...args)
         {  ::new((void*)p, autoboost_container_new_t()) T(::autoboost::forward<Args>(args)...); }
      #else // #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
         public:
         #define AUTOBOOST_PP_LOCAL_MACRO(n)                                                              \
         template<class T AUTOBOOST_PP_ENUM_TRAILING_PARAMS(n, class P) >                                 \
         static void construct(Alloc &a, T *p                                                         \
                              AUTOBOOST_PP_ENUM_TRAILING(n, AUTOBOOST_CONTAINER_PP_PARAM_LIST, _))            \
         {                                                                                            \
            container_detail::bool_                                                                   \
               <container_detail::is_std_allocator<Alloc>::value> flag;                               \
            allocator_traits::priv_construct(flag, a, p                                               \
               AUTOBOOST_PP_ENUM_TRAILING(n, AUTOBOOST_CONTAINER_PP_PARAM_FORWARD, _));                       \
         }                                                                                            \
         //
         #define AUTOBOOST_PP_LOCAL_LIMITS (0, AUTOBOOST_CONTAINER_MAX_CONSTRUCTOR_PARAMETERS)
         #include AUTOBOOST_PP_LOCAL_ITERATE()

         private:
         #define AUTOBOOST_PP_LOCAL_MACRO(n)                                                                    \
         template<class T  AUTOBOOST_PP_ENUM_TRAILING_PARAMS(n, class P) >                                      \
         static void priv_construct(container_detail::false_type, Alloc &a, T *p                            \
                        AUTOBOOST_PP_ENUM_TRAILING(n, AUTOBOOST_CONTAINER_PP_PARAM_LIST,_))                         \
         {                                                                                                  \
            const bool value =                                                                              \
               autoboost::container::container_detail::has_member_function_callable_with_construct              \
                     < Alloc, T* AUTOBOOST_PP_ENUM_TRAILING(n, AUTOBOOST_CONTAINER_PP_FWD_TYPE, _) >::value;        \
            container_detail::bool_<value> flag;                                                            \
            priv_construct_dispatch2(flag, a, p                                                             \
               AUTOBOOST_PP_ENUM_TRAILING(n, AUTOBOOST_CONTAINER_PP_PARAM_FORWARD, _) );                            \
         }                                                                                                  \
                                                                                                            \
         template<class T  AUTOBOOST_PP_ENUM_TRAILING_PARAMS(n, class P) >                                      \
         static void priv_construct(container_detail::true_type, Alloc &a, T *p                             \
                        AUTOBOOST_PP_ENUM_TRAILING(n, AUTOBOOST_CONTAINER_PP_PARAM_LIST,_))                         \
         {                                                                                                  \
            priv_construct_dispatch2(container_detail::false_type(), a, p                                   \
               AUTOBOOST_PP_ENUM_TRAILING(n, AUTOBOOST_CONTAINER_PP_PARAM_FORWARD, _) );                            \
         }                                                                                                  \
                                                                                                            \
         template<class T  AUTOBOOST_PP_ENUM_TRAILING_PARAMS(n, class P) >                                      \
         static void priv_construct_dispatch2(container_detail::true_type, Alloc &a, T *p                   \
                        AUTOBOOST_PP_ENUM_TRAILING(n, AUTOBOOST_CONTAINER_PP_PARAM_LIST,_))                         \
         {  a.construct( p AUTOBOOST_PP_ENUM_TRAILING(n, AUTOBOOST_CONTAINER_PP_PARAM_FORWARD, _) );  }             \
                                                                                                            \
         template<class T  AUTOBOOST_PP_ENUM_TRAILING_PARAMS(n, class P) >                                      \
         static void priv_construct_dispatch2(container_detail::false_type, Alloc &, T *p                   \
                        AUTOBOOST_PP_ENUM_TRAILING(n, AUTOBOOST_CONTAINER_PP_PARAM_LIST, _) )                       \
         {  ::new((void*)p, autoboost_container_new_t()) T(AUTOBOOST_PP_ENUM(n, AUTOBOOST_CONTAINER_PP_PARAM_FORWARD, _)); }\
         //
         #define AUTOBOOST_PP_LOCAL_LIMITS (0, AUTOBOOST_CONTAINER_MAX_CONSTRUCTOR_PARAMETERS)
         #include AUTOBOOST_PP_LOCAL_ITERATE()
      #endif   // #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

      template<class T>
      static void priv_construct_dispatch2(container_detail::false_type, Alloc &, T *p, ::autoboost::container::default_init_t)
      {  ::new((void*)p) T; }
   #endif   //#if defined(AUTOBOOST_CONTAINER_DOXYGEN_INVOKED)

   #endif   //#ifndef AUTOBOOST_CONTAINER_DOXYGEN_INVOKED
};

}  //namespace container {
}  //namespace autoboost {

#include <autoboost/container/detail/config_end.hpp>

#endif // ! defined(AUTOBOOST_CONTAINER_ALLOCATOR_ALLOCATOR_TRAITS_HPP)
