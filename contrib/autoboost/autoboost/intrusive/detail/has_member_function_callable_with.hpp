//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2011-2014. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
//////////////////////////////////////////////////////////////////////////////

// sample.h

#if !defined(AUTOBOOST_PP_IS_ITERATING)

   #ifndef AUTOBOOST_INTRUSIVE_DETAIL_HAS_MEMBER_FUNCTION_CALLABLE_WITH_DETAILS_INCLUDED
   #define AUTOBOOST_INTRUSIVE_DETAIL_HAS_MEMBER_FUNCTION_CALLABLE_WITH_DETAILS_INCLUDED

      #include <autoboost/intrusive/detail/preprocessor.hpp>
      #include <autoboost/intrusive/detail/mpl.hpp>
      #include <autoboost/move/utility_core.hpp>


      //Mark that we don't support 0 arg calls due to compiler ICE in GCC 3.4/4.0/4.1 and
      //wrong SFINAE for GCC 4.2/4.3
      #if defined(__GNUC__) && !defined(__clang__) && ((__GNUC__*100 + __GNUC_MINOR__*10) >= 340) && ((__GNUC__*100 + __GNUC_MINOR__*10) <= 430)
      #define AUTOBOOST_INTRUSIVE_DETAIL_HAS_MEMBER_FUNCTION_CALLABLE_WITH_0_ARGS_UNSUPPORTED
      #elif defined(AUTOBOOST_INTEL) && (AUTOBOOST_INTEL < 1200 )
      #define AUTOBOOST_INTRUSIVE_DETAIL_HAS_MEMBER_FUNCTION_CALLABLE_WITH_0_ARGS_UNSUPPORTED
      #endif

      namespace autoboost_intrusive_has_member_function_callable_with {

      struct dont_care
      {
         dont_care(...);
      };

      template<class T>
      struct make_dontcare
      {
         typedef autoboost_intrusive_has_member_function_callable_with::dont_care type;
      };

      struct private_type
      {
         static private_type p;
         private_type const &operator,(int) const;
      };

      typedef char yes_type;            // sizeof(yes_type) == 1
      struct no_type{ char dummy[2]; }; // sizeof(no_type)  == 2

      template<typename T>
      no_type is_private_type(T const &);
      yes_type is_private_type(private_type const &);

      }  //boost_intrusive_has_member_function_callable_with

   #if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
   #define AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_SINGLE_ITERATION
   #endif

   #endif   //AUTOBOOST_INTRUSIVE_DETAIL_HAS_MEMBER_FUNCTION_CALLABLE_WITH_DETAILS_INCLUDED

#else //!AUTOBOOST_PP_IS_ITERATING

   #ifndef  AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME
   #error "AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME not defined!"
   #endif

   #ifndef  AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEGIN
   #error "AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEGIN not defined!"
   #endif

   #ifndef  AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END
   #error "AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END not defined!"
   #endif

   #if AUTOBOOST_PP_ITERATION_START() > AUTOBOOST_PP_ITERATION_FINISH()
   #error "AUTOBOOST_PP_ITERATION_START() must be <= AUTOBOOST_PP_ITERATION_FINISH()"
   #endif

   #if AUTOBOOST_PP_ITERATION() == AUTOBOOST_PP_ITERATION_START()

      AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEGIN

      template <typename Type>
      class AUTOBOOST_PP_CAT(has_member_function_named_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME)
      {
         struct BaseMixin
         {
            void AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME();
         };

         struct Base : public ::autoboost::intrusive::detail::remove_cv<Type>::type, public BaseMixin { Base(); };
         template <typename T, T t> class Helper{};

         template <typename U>
         static autoboost_intrusive_has_member_function_callable_with::no_type  deduce
            (U*, Helper<void (BaseMixin::*)(), &U::AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME>* = 0);
         static autoboost_intrusive_has_member_function_callable_with::yes_type deduce(...);

         public:
         static const bool value =
            sizeof(autoboost_intrusive_has_member_function_callable_with::yes_type) == sizeof(deduce((Base*)(0)));
      };

      #if defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

         template<typename Fun, bool HasFunc
                  AUTOBOOST_PP_ENUM_TRAILING(AUTOBOOST_PP_ITERATION_FINISH(), AUTOBOOST_INTRUSIVE_PP_TEMPLATE_PARAM_VOID_DEFAULT, _)>
         struct AUTOBOOST_PP_CAT(AUTOBOOST_PP_CAT(has_member_function_callable_with_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME), _impl);
         //!

         template<typename Fun AUTOBOOST_PP_ENUM_TRAILING_PARAMS(AUTOBOOST_PP_ITERATION_FINISH(), class P)>
         struct AUTOBOOST_PP_CAT(AUTOBOOST_PP_CAT(has_member_function_callable_with_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME), _impl)
            <Fun, false AUTOBOOST_PP_ENUM_TRAILING_PARAMS(AUTOBOOST_PP_ITERATION_FINISH(), P)>
         {
            static const bool value = false;
         };
         //!

      #else //!defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

         template<typename Fun, bool HasFunc, class ...Args>
         struct AUTOBOOST_PP_CAT(AUTOBOOST_PP_CAT(has_member_function_callable_with_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME),_impl);

         template<typename Fun, class ...Args>
         struct AUTOBOOST_PP_CAT(AUTOBOOST_PP_CAT(has_member_function_callable_with_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME),_impl)
            <Fun, false, Args...>
         {
            static const bool value = false;
         };

         #ifdef AUTOBOOST_NO_CXX11_DECLTYPE

         //Special case for 0 args
         template< class F
               , std::size_t N =
                     sizeof((autoboost::move_detail::declval<F>().
                        AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME (), 0))>
         struct AUTOBOOST_PP_CAT(zeroarg_checker_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME)
         {
            autoboost_intrusive_has_member_function_callable_with::yes_type dummy;
            AUTOBOOST_PP_CAT(zeroarg_checker_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME)(int);
         };

         //For buggy compilers like MSVC 7.1+ ((F*)0)->func() does not
         //SFINAE-out the zeroarg_checker_ instantiation but sizeof yields to 0.
         template<class F>
         struct AUTOBOOST_PP_CAT(zeroarg_checker_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME)<F, 0>
         {
            autoboost_intrusive_has_member_function_callable_with::no_type dummy;
            AUTOBOOST_PP_CAT(zeroarg_checker_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME)(int);
         };

         #endif   //#ifdef AUTOBOOST_NO_CXX11_DECLTYPE

         template<typename Fun>
         struct AUTOBOOST_PP_CAT(AUTOBOOST_PP_CAT(has_member_function_callable_with_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME),_impl)
            <Fun, true>
         {
            #ifndef AUTOBOOST_NO_CXX11_DECLTYPE
            template<class U, class V = decltype(autoboost::move_detail::declval<U>().AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME()) >
            static autoboost_intrusive_has_member_function_callable_with::yes_type Test(U*);
            #else
            template<class U>
               static AUTOBOOST_PP_CAT(zeroarg_checker_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME)
               <U> Test(AUTOBOOST_PP_CAT(zeroarg_checker_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME)<U>*);
            #endif
   
            template <class U>
            static autoboost_intrusive_has_member_function_callable_with::no_type Test(...);

            static const bool value = sizeof(Test< Fun >(0))
                                 == sizeof(autoboost_intrusive_has_member_function_callable_with::yes_type);
         };

         template<typename Fun, class ...Args>
         struct AUTOBOOST_PP_CAT( AUTOBOOST_PP_CAT(has_member_function_callable_with_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME), _impl)
            <Fun, true , Args...>
         {

            template<class ...DontCares>
            struct FunWrapTmpl : Fun
            {
               FunWrapTmpl();
               using Fun::AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME;

               autoboost_intrusive_has_member_function_callable_with::private_type
                  AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME
                     ( DontCares...)  const;
            };

            typedef FunWrapTmpl<typename autoboost_intrusive_has_member_function_callable_with::make_dontcare<Args>::type...> FunWrap;

            static bool const value = (sizeof(autoboost_intrusive_has_member_function_callable_with::no_type) ==
                                       sizeof(autoboost_intrusive_has_member_function_callable_with::is_private_type
                                                ( (::autoboost::move_detail::declval< FunWrap >().
                                          AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME
                                             ( ::autoboost::move_detail::declval<Args>()... ), 0) )
                                             )
                                       );
         };

         template<typename Fun, class ...Args>
         struct AUTOBOOST_PP_CAT( has_member_function_callable_with_
                            , AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME)
            : public AUTOBOOST_PP_CAT( AUTOBOOST_PP_CAT(has_member_function_callable_with_
                                 , AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME),_impl)
               < Fun
               , AUTOBOOST_PP_CAT( has_member_function_named_
                             , AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME )<Fun>::value
               , Args... >
         {};

      #endif   //defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

      AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END

   #endif   //AUTOBOOST_PP_ITERATION() == AUTOBOOST_PP_ITERATION_START()

   #if AUTOBOOST_PP_ITERATION() == 0

      AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEGIN

      #if defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

         #if !defined(_MSC_VER) || (_MSC_VER < 1600)

            #if defined(AUTOBOOST_INTRUSIVE_DETAIL_HAS_MEMBER_FUNCTION_CALLABLE_WITH_0_ARGS_UNSUPPORTED)

            template<typename Fun>
            struct AUTOBOOST_PP_CAT(AUTOBOOST_PP_CAT(has_member_function_callable_with_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME),_impl)
               <Fun, true AUTOBOOST_PP_ENUM_TRAILING(AUTOBOOST_PP_SUB(AUTOBOOST_PP_ITERATION_FINISH(), AUTOBOOST_PP_ITERATION()), AUTOBOOST_INTRUSIVE_PP_IDENTITY, void)>
            {
               //Mark that we don't support 0 arg calls due to compiler ICE in GCC 3.4/4.0/4.1 and
               //wrong SFINAE for GCC 4.2/4.3
               static const bool value = true;
            };

            #else //defined(AUTOBOOST_INTRUSIVE_DETAIL_HAS_MEMBER_FUNCTION_CALLABLE_WITH_0_ARGS_UNSUPPORTED)

            //Special case for 0 args
            template< class F
                  , std::size_t N =
                        sizeof((autoboost::move_detail::declval<F>().
                           AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME (), 0))>
            struct AUTOBOOST_PP_CAT(zeroarg_checker_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME)
            {
               autoboost_intrusive_has_member_function_callable_with::yes_type dummy;
               AUTOBOOST_PP_CAT(zeroarg_checker_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME)(int);
            };

            //For buggy compilers like MSVC 7.1+ ((F*)0)->func() does not
            //SFINAE-out the zeroarg_checker_ instantiation but sizeof yields to 0.
            template<class F>
            struct AUTOBOOST_PP_CAT(zeroarg_checker_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME)<F, 0>
            {
               autoboost_intrusive_has_member_function_callable_with::no_type dummy;
               AUTOBOOST_PP_CAT(zeroarg_checker_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME)(int);
            };

            template<typename Fun>
            struct AUTOBOOST_PP_CAT(AUTOBOOST_PP_CAT(has_member_function_callable_with_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME),_impl)
               <Fun, true AUTOBOOST_PP_ENUM_TRAILING(AUTOBOOST_PP_SUB(AUTOBOOST_PP_ITERATION_FINISH(), AUTOBOOST_PP_ITERATION()), AUTOBOOST_INTRUSIVE_PP_IDENTITY, void)>
            {
               template<class U>
               static AUTOBOOST_PP_CAT(zeroarg_checker_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME)<U>
                  Test(AUTOBOOST_PP_CAT(zeroarg_checker_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME)<U>*);

               template <class U>
               static autoboost_intrusive_has_member_function_callable_with::no_type Test(...);

               static const bool value = sizeof(Test< Fun >(0))
                                    == sizeof(autoboost_intrusive_has_member_function_callable_with::yes_type);
            };
            #endif   //defined(AUTOBOOST_INTRUSIVE_DETAIL_HAS_MEMBER_FUNCTION_CALLABLE_WITH_0_ARGS_UNSUPPORTED)

         #else //#if !defined(_MSC_VER) || (_MSC_VER < 1600)
            template<typename Fun>
            struct AUTOBOOST_PP_CAT(AUTOBOOST_PP_CAT(has_member_function_callable_with_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME),_impl)
               <Fun, true AUTOBOOST_PP_ENUM_TRAILING(AUTOBOOST_PP_SUB(AUTOBOOST_PP_ITERATION_FINISH(), AUTOBOOST_PP_ITERATION()), AUTOBOOST_INTRUSIVE_PP_IDENTITY, void)>
            {
               template<class U>
               static decltype( autoboost::move_detail::declval<U>().AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME()
                              , autoboost_intrusive_has_member_function_callable_with::yes_type())
                  Test(U*);

               template<class U>
               static autoboost_intrusive_has_member_function_callable_with::no_type Test(...);

               static const bool value = sizeof(Test<Fun>(0))
                                    == sizeof(autoboost_intrusive_has_member_function_callable_with::yes_type);
            };
         #endif   //#if !defined(_MSC_VER) || (_MSC_VER < 1600)

      #else   //#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

      #endif   //#if defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

      AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END

   #else   //AUTOBOOST_PP_ITERATION() == 0

      #if defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

         AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEGIN

         template<typename Fun AUTOBOOST_PP_ENUM_TRAILING_PARAMS(AUTOBOOST_PP_ITERATION(), class P)>
         struct AUTOBOOST_PP_CAT( AUTOBOOST_PP_CAT(has_member_function_callable_with_
                            , AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME),_impl)
            <Fun, true
            AUTOBOOST_PP_ENUM_TRAILING_PARAMS(AUTOBOOST_PP_ITERATION(), P)
            AUTOBOOST_PP_ENUM_TRAILING( AUTOBOOST_PP_SUB(AUTOBOOST_PP_ITERATION_FINISH(), AUTOBOOST_PP_ITERATION())
                                  , AUTOBOOST_INTRUSIVE_PP_IDENTITY
                                  , void)>
         {
            struct FunWrap : Fun
            {
               FunWrap();

               using Fun::AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME;
               autoboost_intrusive_has_member_function_callable_with::private_type
                  AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME
                     ( AUTOBOOST_PP_ENUM(AUTOBOOST_PP_ITERATION()
                     , AUTOBOOST_INTRUSIVE_PP_IDENTITY
                     , autoboost_intrusive_has_member_function_callable_with::dont_care))  const;
            };

            static bool const value =
            (sizeof(autoboost_intrusive_has_member_function_callable_with::no_type) ==
               sizeof(autoboost_intrusive_has_member_function_callable_with::is_private_type
                        (  (autoboost::move_detail::declval<FunWrap>().
                              AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME
                                 ( AUTOBOOST_PP_ENUM( AUTOBOOST_PP_ITERATION(), AUTOBOOST_INTRUSIVE_PP_DECLVAL, _) ), 0
                           )
                        )
                     )
            );
         };

         AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END
      #endif   //#if defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

   #endif   //AUTOBOOST_PP_ITERATION() == 0

   #if AUTOBOOST_PP_ITERATION() == AUTOBOOST_PP_ITERATION_FINISH()

      #if defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

         AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEGIN

         template<typename Fun
                  AUTOBOOST_PP_ENUM_TRAILING(AUTOBOOST_PP_ITERATION_FINISH(), AUTOBOOST_INTRUSIVE_PP_TEMPLATE_PARAM_VOID_DEFAULT, _)>
         struct AUTOBOOST_PP_CAT(has_member_function_callable_with_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME)
            : public AUTOBOOST_PP_CAT(AUTOBOOST_PP_CAT(has_member_function_callable_with_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME), _impl)
               <Fun, AUTOBOOST_PP_CAT(has_member_function_named_, AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME)<Fun>::value
               AUTOBOOST_PP_ENUM_TRAILING_PARAMS(AUTOBOOST_PP_ITERATION_FINISH(), P) >
         {};

         AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END

      #endif //#if defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

      #undef AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_FUNCNAME
      #undef AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_BEGIN
      #undef AUTOBOOST_INTRUSIVE_HAS_MEMBER_FUNCTION_CALLABLE_WITH_NS_END

   #endif   //#if AUTOBOOST_PP_ITERATION() == AUTOBOOST_PP_ITERATION_FINISH()

#endif   //!AUTOBOOST_PP_IS_ITERATING
