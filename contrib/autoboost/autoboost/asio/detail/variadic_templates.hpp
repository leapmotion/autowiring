//
// detail/variadic_templates.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_VARIADIC_TEMPLATES_HPP
#define AUTOBOOST_ASIO_DETAIL_VARIADIC_TEMPLATES_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if !defined(AUTOBOOST_ASIO_HAS_VARIADIC_TEMPLATES)

# define AUTOBOOST_ASIO_VARIADIC_TPARAMS(n) AUTOBOOST_ASIO_VARIADIC_TPARAMS_##n

# define AUTOBOOST_ASIO_VARIADIC_TPARAMS_1 \
  typename T1
# define AUTOBOOST_ASIO_VARIADIC_TPARAMS_2 \
  typename T1, typename T2
# define AUTOBOOST_ASIO_VARIADIC_TPARAMS_3 \
  typename T1, typename T2, typename T3
# define AUTOBOOST_ASIO_VARIADIC_TPARAMS_4 \
  typename T1, typename T2, typename T3, typename T4
# define AUTOBOOST_ASIO_VARIADIC_TPARAMS_5 \
  typename T1, typename T2, typename T3, typename T4, typename T5

# define AUTOBOOST_ASIO_VARIADIC_TARGS(n) AUTOBOOST_ASIO_VARIADIC_TARGS_##n

# define AUTOBOOST_ASIO_VARIADIC_TARGS_1 x1
# define AUTOBOOST_ASIO_VARIADIC_TARGS_2 x1, x2
# define AUTOBOOST_ASIO_VARIADIC_TARGS_3 x1, x2, x3
# define AUTOBOOST_ASIO_VARIADIC_TARGS_4 x1, x2, x3, x4
# define AUTOBOOST_ASIO_VARIADIC_TARGS_5 x1, x2, x3, x4, x5

# define AUTOBOOST_ASIO_VARIADIC_PARAMS(n) AUTOBOOST_ASIO_VARIADIC_PARAMS_##n

# define AUTOBOOST_ASIO_VARIADIC_PARAMS_1 T1 x1
# define AUTOBOOST_ASIO_VARIADIC_PARAMS_2 T1 x1, T2 x2
# define AUTOBOOST_ASIO_VARIADIC_PARAMS_3 T1 x1, T2 x2, T3 x3
# define AUTOBOOST_ASIO_VARIADIC_PARAMS_4 T1 x1, T2 x2, T3 x3, T4 x4
# define AUTOBOOST_ASIO_VARIADIC_PARAMS_5 T1 x1, T2 x2, T3 x3, T4 x4, T5 x5

# define AUTOBOOST_ASIO_VARIADIC_ARGS(n) AUTOBOOST_ASIO_VARIADIC_ARGS_##n

# define AUTOBOOST_ASIO_VARIADIC_ARGS_1 x1
# define AUTOBOOST_ASIO_VARIADIC_ARGS_2 x1, x2
# define AUTOBOOST_ASIO_VARIADIC_ARGS_3 x1, x2, x3
# define AUTOBOOST_ASIO_VARIADIC_ARGS_4 x1, x2, x3, x4
# define AUTOBOOST_ASIO_VARIADIC_ARGS_5 x1, x2, x3, x4, x5

# define AUTOBOOST_ASIO_VARIADIC_GENERATE(m) m(1) m(2) m(3) m(4) m(5)

#endif // !defined(AUTOBOOST_ASIO_HAS_VARIADIC_TEMPLATES)

#endif // AUTOBOOST_ASIO_DETAIL_VARIADIC_TEMPLATES_HPP
