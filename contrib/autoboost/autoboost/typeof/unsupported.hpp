// Copyright (C) 2010 Peder Holt
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_TYPEOF_UNSUPPORTED_HPP_INCLUDED
#define AUTOBOOST_TYPEOF_UNSUPPORTED_HPP_INCLUDED

namespace autoboost { namespace type_of {
    struct typeof_emulation_is_unsupported_on_this_compiler {};
}}

#define AUTOBOOST_TYPEOF(expr) autoboost::type_of::typeof_emulation_is_unsupported_on_this_compiler
#define AUTOBOOST_TYPEOF_TPL AUTOBOOST_TYPEOF

#define AUTOBOOST_TYPEOF_NESTED_TYPEDEF_TPL(name,expr) \
struct name {\
    typedef AUTOBOOST_TYPEOF_TPL(expr) type;\
};

#define AUTOBOOST_TYPEOF_NESTED_TYPEDEF(name,expr) \
struct name {\
    typedef AUTOBOOST_TYPEOF(expr) type;\
};


#define AUTOBOOST_TYPEOF_REGISTER_TYPE(x)
#define AUTOBOOST_TYPEOF_REGISTER_TEMPLATE(x, params)

#endif