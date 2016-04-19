// Copyright Daniel Wallin 2006. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//
// 2009.10.21 TDS remove depenency on autoboost::python::detail::referent_storage
//
#ifndef AUTOBOOST_PARAMETER_MAYBE_091021_HPP
# define AUTOBOOST_PARAMETER_MAYBE_091021_HPP

# include <autoboost/mpl/if.hpp>
# include <autoboost/mpl/identity.hpp>
# include <autoboost/type_traits/is_reference.hpp>
# include <autoboost/type_traits/add_reference.hpp>
# include <autoboost/optional.hpp>
# include <autoboost/aligned_storage.hpp>
# include <autoboost/type_traits/remove_cv.hpp>
# include <autoboost/type_traits/add_const.hpp>
# include <autoboost/parameter/aux_/is_maybe.hpp>

namespace autoboost { namespace parameter { namespace aux {

template <class T> struct referent_size;

template <class T>
struct referent_size<T&>
{
  AUTOBOOST_STATIC_CONSTANT(std::size_t, value = sizeof(T));
};

// A metafunction returning a POD type which can store U, where T ==
// U&. If T is not a reference type, returns a POD which can store T.
template <class T>
struct referent_storage
{
  typedef typename autoboost::aligned_storage<
    referent_size<T>::value
    >::type type;
};

template <class T>
struct maybe : maybe_base
{
    typedef typename add_reference<
# if AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x564))
        T const
# else
        typename add_const<T>::type
# endif
    >::type reference;

    typedef typename remove_cv<
        AUTOBOOST_DEDUCED_TYPENAME remove_reference<reference>::type
    >::type non_cv_value;

    explicit maybe(T value_)
      : value(value_)
      , constructed(false)
    {}

    maybe()
      : constructed(false)
    {}

    ~maybe()
    {
        if (constructed)
            this->destroy();
    }

    reference construct(reference value_) const
    {
        return value_;
    }

    template <class U>
    reference construct2(U const& value_) const
    {
        new (m_storage.address()) non_cv_value(value_);
        constructed = true;
        return *(non_cv_value*)m_storage.address();
    }

    template <class U>
    reference construct(U const& value_) const
    {
        return this->construct2(value_);
    }

    void destroy()
    {
        ((non_cv_value*)m_storage.address())->~non_cv_value();
    }

    typedef reference(maybe<T>::*safe_bool)() const;

    operator safe_bool() const
    {
        return value ? &maybe<T>::get : 0 ;
    }

    reference get() const
    {
        return value.get();
    }

private:
    autoboost::optional<T> value;
    mutable bool constructed;


    mutable typename referent_storage<
        reference
    >::type m_storage;
};

}}} // namespace autoboost::parameter::aux

#endif // AUTOBOOST_PARAMETER_MAYBE_060211_HPP

