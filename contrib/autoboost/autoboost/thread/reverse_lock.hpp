// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2012 Vicente J. Botet Escriba

#ifndef AUTOBOOST_THREAD_REVERSE_LOCK_HPP
#define AUTOBOOST_THREAD_REVERSE_LOCK_HPP
#include <autoboost/thread/detail/config.hpp>
#include <autoboost/thread/detail/move.hpp>
#include <autoboost/thread/lockable_traits.hpp>
#include <autoboost/thread/lock_options.hpp>
#include <autoboost/thread/detail/delete.hpp>

namespace autoboost
{

    template<typename Lock>
    class reverse_lock
    {
    public:
        typedef typename Lock::mutex_type mutex_type;
        AUTOBOOST_THREAD_NO_COPYABLE(reverse_lock)

        explicit reverse_lock(Lock& m_)
        : m(m_), mtx(0)
        {
            if (m.owns_lock())
            {
              m.unlock();
            }
            mtx=m.release();
        }
        ~reverse_lock()
        {
          if (mtx) {
            mtx->lock();
            m = AUTOBOOST_THREAD_MAKE_RV_REF(Lock(*mtx, adopt_lock));
          }
        }

    private:
      Lock& m;
      mutex_type* mtx;
    };


#ifdef AUTOBOOST_THREAD_NO_AUTO_DETECT_MUTEX_TYPES
    template<typename T>
    struct is_mutex_type<reverse_lock<T> >
    {
        AUTOBOOST_STATIC_CONSTANT(bool, value = true);
    };

#endif


}

#endif // header
