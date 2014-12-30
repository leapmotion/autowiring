
// Copyright (C) 2005 Igor Chesnokov, mailto:ichesnokov@gmail.com (VC 6.5,VC 7.1 + counter code)
// Copyright (C) 2005-2007 Peder Holt (VC 7.0 + framework)
// Copyright (C) 2006 Steven Watanabe (VC 8.0)

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_MSVC_TYPEOF_IMPL_HPP_INCLUDED
# define AUTOBOOST_TYPEOF_MSVC_TYPEOF_IMPL_HPP_INCLUDED

# include <autoboost/config.hpp>
# include <autoboost/detail/workaround.hpp>
# include <autoboost/mpl/int.hpp>
# include <autoboost/type_traits/is_function.hpp>
# include <autoboost/utility/enable_if.hpp>

# if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC,>=1310)
#  include <typeinfo>
# endif

namespace autoboost
{
    namespace type_of
    {

        //Compile time constant code
# if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC,>=1300) && defined(_MSC_EXTENSIONS)
        template<int N> struct the_counter;

        template<typename T,int N = 5/*for similarity*/>
        struct encode_counter
        {
            __if_exists(the_counter<N + 256>)
            {
                AUTOBOOST_STATIC_CONSTANT(unsigned,count=(encode_counter<T,N + 257>::count));
            }
            __if_not_exists(the_counter<N + 256>)
            {
                __if_exists(the_counter<N + 64>)
                {
                    AUTOBOOST_STATIC_CONSTANT(unsigned,count=(encode_counter<T,N + 65>::count));
                }
                __if_not_exists(the_counter<N + 64>)
                {
                    __if_exists(the_counter<N + 16>)
                    {
                        AUTOBOOST_STATIC_CONSTANT(unsigned,count=(encode_counter<T,N + 17>::count));
                    }
                    __if_not_exists(the_counter<N + 16>)
                    {
                        __if_exists(the_counter<N + 4>)
                        {
                            AUTOBOOST_STATIC_CONSTANT(unsigned,count=(encode_counter<T,N + 5>::count));
                        }
                        __if_not_exists(the_counter<N + 4>)
                        {
                            __if_exists(the_counter<N>)
                            {
                                AUTOBOOST_STATIC_CONSTANT(unsigned,count=(encode_counter<T,N + 1>::count));
                            }
                            __if_not_exists(the_counter<N>)
                            {
                                AUTOBOOST_STATIC_CONSTANT(unsigned,count=N);
                                typedef the_counter<N> type;
                            }
                        }
                    }
                }
            }
        };

# define AUTOBOOST_TYPEOF_INDEX(T) (encode_counter<T>::count)
# define AUTOBOOST_TYPEOF_NEXT_INDEX(next)
# else
        template<int N> struct encode_counter : encode_counter<N - 1> {};
        template<> struct encode_counter<0> {};

        //Need to default to a larger value than 4, as due to MSVC's ETI errors. (sizeof(int)==4)
        char (*encode_index(...))[5];

# define AUTOBOOST_TYPEOF_INDEX(T) (sizeof(*autoboost::type_of::encode_index((autoboost::type_of::encode_counter<1005>*)0)))
# define AUTOBOOST_TYPEOF_NEXT_INDEX(next) friend char (*encode_index(encode_counter<next>*))[next];
# endif

        //Typeof code

# if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC,==1300)
        template<typename ID>
        struct msvc_extract_type
        {
            template<bool>
            struct id2type_impl;

            typedef id2type_impl<true> id2type;
        };

        template<typename T, typename ID>
        struct msvc_register_type : msvc_extract_type<ID>
        {
            template<>
            struct id2type_impl<true>  //VC7.0 specific bugfeature
            {
                typedef T type;
            };
        };
#elif AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC,>=1400)
        struct msvc_extract_type_default_param {};

        template<typename ID, typename T = msvc_extract_type_default_param>
        struct msvc_extract_type;

        template<typename ID>
        struct msvc_extract_type<ID, msvc_extract_type_default_param> {
            template<bool>
            struct id2type_impl;

            typedef id2type_impl<true> id2type;
        };

        template<typename ID, typename T>
        struct msvc_extract_type : msvc_extract_type<ID,msvc_extract_type_default_param>
        {
            template<>
            struct id2type_impl<true>  //VC8.0 specific bugfeature
            {
                typedef T type;
            };
            template<bool>
            struct id2type_impl;

            typedef id2type_impl<true> id2type;
        };

        template<typename T, typename ID>
        struct msvc_register_type : msvc_extract_type<ID, T>
        {
        };
# else
        template<typename ID>
        struct msvc_extract_type
        {
            struct id2type;
        };

        template<typename T, typename ID>
        struct msvc_register_type : msvc_extract_type<ID>
        {
            typedef msvc_extract_type<ID> base_type;
            struct base_type::id2type // This uses nice VC6.5 and VC7.1 bugfeature
            {
                typedef T type;
            };
        };
# endif
// EAN: preprocess this block out on advice of Peder Holt
// to eliminate errors in type_traits/common_type.hpp
# if 0 //AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC,==1310)
        template<const std::type_info& ref_type_info>
        struct msvc_typeid_wrapper {
            typedef typename msvc_extract_type<msvc_typeid_wrapper>::id2type id2type;
            typedef typename id2type::type wrapped_type;
            typedef typename wrapped_type::type type;
        };
        //This class is used for registering the type T. encode_type<T> is mapped against typeid(encode_type<T>).
        //msvc_typeid_wrapper<typeid(encode_type<T>)> will now have a type typedef that equals encode_type<T>.
        template<typename T>
        struct encode_type
        {
            typedef encode_type<T> input_type;
            //Invoke registration of encode_type<T>. typeid(encode_type<T>) is now mapped to encode_type<T>. Do not use registered_type for anything.
            //The reason for registering encode_type<T> rather than T, is that VC handles typeid(function reference) poorly. By adding another
            //level of indirection, we solve this problem.
            typedef typename msvc_register_type<input_type,msvc_typeid_wrapper<typeid(input_type)> >::id2type registered_type;
            typedef T type;
        };

        template<typename T> typename disable_if<
            typename is_function<T>::type,
            typename encode_type<T>::input_type>::type encode_start(T const&);

        template<typename T> typename enable_if<
            typename is_function<T>::type,
            typename encode_type<T>::input_type>::type encode_start(T&);

        template<typename Organizer, typename T>
        msvc_register_type<T,Organizer> typeof_register_type(const T&);


# define AUTOBOOST_TYPEOF(expr) \
    autoboost::type_of::msvc_typeid_wrapper<typeid(autoboost::type_of::encode_start(expr))>::type

# define AUTOBOOST_TYPEOF_TPL(expr) typename AUTOBOOST_TYPEOF(expr)

# define AUTOBOOST_TYPEOF_NESTED_TYPEDEF_TPL(name,expr) \
struct name {\
    enum {_typeof_register_value=sizeof(typeid(autoboost::type_of::typeof_register_type<name>(expr)))};\
    typedef typename autoboost::type_of::msvc_extract_type<name>::id2type id2type;\
    typedef typename id2type::type type;\
};

# define AUTOBOOST_TYPEOF_NESTED_TYPEDEF(name,expr) \
struct name {\
    enum {_typeof_register_value=sizeof(typeid(autoboost::type_of::typeof_register_type<name>(expr)))};\
    typedef autoboost::type_of::msvc_extract_type<name>::id2type id2type;\
    typedef id2type::type type;\
};

# else
        template<int ID>
        struct msvc_typeid_wrapper {
            typedef typename msvc_extract_type<mpl::int_<ID> >::id2type id2type;
            typedef typename id2type::type type;
        };
        //Workaround for ETI-bug for VC6 and VC7
        template<>
        struct msvc_typeid_wrapper<1> {
            typedef msvc_typeid_wrapper<1> type;
        };
        //Workaround for ETI-bug for VC7.1
        template<>
        struct msvc_typeid_wrapper<4> {
            typedef msvc_typeid_wrapper<4> type;
        };

        //Tie it all together
        template<typename T>
        struct encode_type
        {
            //Get the next available compile time constants index
            AUTOBOOST_STATIC_CONSTANT(unsigned,value=AUTOBOOST_TYPEOF_INDEX(T));
            //Instantiate the template
            typedef typename msvc_register_type<T,mpl::int_<value> >::id2type type;
            //Set the next compile time constants index
            AUTOBOOST_STATIC_CONSTANT(unsigned,next=value+1);
            //Increment the compile time constant (only needed when extensions are not active
            AUTOBOOST_TYPEOF_NEXT_INDEX(next);
        };

        template<class T>
        struct sizer
        {
            typedef char(*type)[encode_type<T>::value];
        };
# if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC,>=1310)
        template<typename T> typename disable_if<
            typename is_function<T>::type,
            typename sizer<T>::type>::type encode_start(T const&);

        template<typename T> typename enable_if<
            typename is_function<T>::type,
            typename sizer<T>::type>::type encode_start(T&);
# else
        template<typename T>
            typename sizer<T>::type encode_start(T const&);
# endif
        template<typename Organizer, typename T>
        msvc_register_type<T,Organizer> typeof_register_type(const T&,Organizer* =0);

# define AUTOBOOST_TYPEOF(expr) \
    autoboost::type_of::msvc_typeid_wrapper<sizeof(*autoboost::type_of::encode_start(expr))>::type

# define AUTOBOOST_TYPEOF_TPL(expr) typename AUTOBOOST_TYPEOF(expr)

# define AUTOBOOST_TYPEOF_NESTED_TYPEDEF_TPL(name,expr) \
    struct name {\
        AUTOBOOST_STATIC_CONSTANT(int,_typeof_register_value=sizeof(autoboost::type_of::typeof_register_type<name>(expr)));\
        typedef typename autoboost::type_of::msvc_extract_type<name>::id2type id2type;\
        typedef typename id2type::type type;\
    };

# define AUTOBOOST_TYPEOF_NESTED_TYPEDEF(name,expr) \
    struct name {\
        AUTOBOOST_STATIC_CONSTANT(int,_typeof_register_value=sizeof(autoboost::type_of::typeof_register_type<name>(expr)));\
        typedef autoboost::type_of::msvc_extract_type<name>::id2type id2type;\
        typedef id2type::type type;\
    };

#endif
    }
}

#endif//AUTOBOOST_TYPEOF_MSVC_TYPEOF_IMPL_HPP_INCLUDED
