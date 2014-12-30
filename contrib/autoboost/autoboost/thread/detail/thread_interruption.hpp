#ifndef AUTOBOOST_THREAD_DETAIL_THREAD_INTERRUPTION_HPP
#define AUTOBOOST_THREAD_DETAIL_THREAD_INTERRUPTION_HPP
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2007-9 Anthony Williams
// (C) Copyright 2012 Vicente J. Botet Escriba

#include <autoboost/thread/detail/config.hpp>
#include <autoboost/thread/detail/delete.hpp>

#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS

namespace autoboost
{
    namespace this_thread
    {
        class AUTOBOOST_THREAD_DECL disable_interruption
        {
          bool interruption_was_enabled;
          friend class restore_interruption;
        public:
            AUTOBOOST_THREAD_NO_COPYABLE(disable_interruption)
            disable_interruption() AUTOBOOST_NOEXCEPT;
            ~disable_interruption() AUTOBOOST_NOEXCEPT;
        };

        class AUTOBOOST_THREAD_DECL restore_interruption
        {
        public:
            AUTOBOOST_THREAD_NO_COPYABLE(restore_interruption)
            explicit restore_interruption(disable_interruption& d) AUTOBOOST_NOEXCEPT;
            ~restore_interruption() AUTOBOOST_NOEXCEPT;
        };
    }
}

#endif // AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
#endif // header
