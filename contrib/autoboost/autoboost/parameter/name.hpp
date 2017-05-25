// Copyright Daniel Wallin 2006. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_PARAMETER_NAME_060806_HPP
# define AUTOBOOST_PARAMETER_NAME_060806_HPP

# include <autoboost/parameter/keyword.hpp>
# include <autoboost/parameter/value_type.hpp>
# include <autoboost/detail/workaround.hpp>
# include <autoboost/preprocessor/cat.hpp>
# include <autoboost/preprocessor/stringize.hpp>
# include <autoboost/preprocessor/control/iif.hpp>
# include <autoboost/preprocessor/tuple/eat.hpp>
# include <autoboost/preprocessor/tuple/elem.hpp>
# include <autoboost/mpl/placeholders.hpp>

# if !defined(AUTOBOOST_NO_SFINAE) \
  && !AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x592))

#  include <autoboost/utility/enable_if.hpp>
#  include <autoboost/mpl/lambda.hpp>

namespace autoboost { namespace parameter { namespace aux {

// Tag type passed to MPL lambda.
struct lambda_tag;

struct name_tag_base
{};

template <class Tag>
struct name_tag
{};

template <class T>
struct is_name_tag
  : mpl::false_
{};

}}} // namespace autoboost::parameter::aux

namespace autoboost { namespace mpl {

template <class T>
struct lambda<
    T
  , typename autoboost::enable_if<
        parameter::aux::is_name_tag<T>, parameter::aux::lambda_tag
    >::type
>
{
    typedef true_ is_le;
    typedef bind3< quote3<parameter::value_type>, arg<2>, T, void> result_;
    typedef result_ type;
};

}} // namespace autoboost::mpl

# endif

# if AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x564))
# include <autoboost/preprocessor/detail/split.hpp>
// From Paul Mensonides
#  define AUTOBOOST_PARAMETER_IS_BINARY(x) \
    AUTOBOOST_PP_SPLIT(1, AUTOBOOST_PARAMETER_IS_BINARY_C x AUTOBOOST_PP_COMMA() 0) \
    /**/
#  define AUTOBOOST_PARAMETER_IS_BINARY_C(x,y) \
    ~, 1 AUTOBOOST_PP_RPAREN() \
    AUTOBOOST_PP_TUPLE_EAT(2) AUTOBOOST_PP_LPAREN() ~ \
    /**/
# else
#  include <autoboost/preprocessor/detail/is_binary.hpp>
#  define AUTOBOOST_PARAMETER_IS_BINARY(x) AUTOBOOST_PP_IS_BINARY(x)
# endif

# define AUTOBOOST_PARAMETER_BASIC_NAME(tag_namespace, tag, name)       \
    namespace tag_namespace                                         \
    {                                                               \
      struct tag                                                    \
      {                                                             \
          static char const* keyword_name()                         \
          {                                                         \
              return AUTOBOOST_PP_STRINGIZE(tag);                       \
          }                                                         \
                                                                    \
          typedef autoboost::parameter::value_type<                     \
              autoboost::mpl::_2, tag, autoboost::parameter::void_          \
          > _;                                                      \
                                                                    \
          typedef autoboost::parameter::value_type<                     \
              autoboost::mpl::_2, tag, autoboost::parameter::void_          \
          > _1;                                                     \
      };                                                            \
    }                                                               \
    namespace                                                       \
    {                                                               \
       ::autoboost::parameter::keyword<tag_namespace::tag> const& name  \
       = ::autoboost::parameter::keyword<tag_namespace::tag>::instance; \
    }

# define AUTOBOOST_PARAMETER_COMPLEX_NAME_TUPLE1(tag,namespace)         \
    (tag, namespace), ~

# define AUTOBOOST_PARAMETER_COMPLEX_NAME_TUPLE(name)                   \
    AUTOBOOST_PP_TUPLE_ELEM(2, 0, (AUTOBOOST_PARAMETER_COMPLEX_NAME_TUPLE1 name))

# define AUTOBOOST_PARAMETER_COMPLEX_NAME_TAG(name)                     \
    AUTOBOOST_PP_TUPLE_ELEM(2, 0, AUTOBOOST_PARAMETER_COMPLEX_NAME_TUPLE(name))

# define AUTOBOOST_PARAMETER_COMPLEX_NAME_NAMESPACE(name)               \
    AUTOBOOST_PP_TUPLE_ELEM(2, 1, AUTOBOOST_PARAMETER_COMPLEX_NAME_TUPLE(name))

# define AUTOBOOST_PARAMETER_COMPLEX_NAME(name)                         \
    AUTOBOOST_PARAMETER_BASIC_NAME(                                     \
        AUTOBOOST_PARAMETER_COMPLEX_NAME_NAMESPACE(name)                \
      , AUTOBOOST_PP_TUPLE_EAT(2) name                                  \
      , AUTOBOOST_PARAMETER_COMPLEX_NAME_TAG(name)                      \
    )                                                               \
/**/

# define AUTOBOOST_PARAMETER_SIMPLE_NAME(name)                          \
    AUTOBOOST_PARAMETER_BASIC_NAME(tag, name, AUTOBOOST_PP_CAT(_, name))

# define AUTOBOOST_PARAMETER_NAME(name)                                 \
    AUTOBOOST_PP_IIF(                                                   \
        AUTOBOOST_PARAMETER_IS_BINARY(name)                             \
      , AUTOBOOST_PARAMETER_COMPLEX_NAME                                \
      , AUTOBOOST_PARAMETER_SIMPLE_NAME                                 \
    )(name)                                                         \
/**/


# define AUTOBOOST_PARAMETER_TEMPLATE_KEYWORD(name)                     \
    namespace tag                                                   \
    {                                                               \
      struct name;                                                  \
    }                                                               \
    template <class T>                                              \
    struct name                                                     \
      : autoboost::parameter::template_keyword<tag::name, T>            \
    {};                                                             \
/**/

#endif // AUTOBOOST_PARAMETER_NAME_060806_HPP

