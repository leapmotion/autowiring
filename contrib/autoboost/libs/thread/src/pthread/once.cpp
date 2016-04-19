// Copyright (C) 2007 Anthony Williams
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <autoboost/thread/detail/config.hpp>
#ifdef AUTOBOOST_THREAD_ONCE_ATOMIC
#include "./once_atomic.cpp"
#else
#define __STDC_CONSTANT_MACROS
#include <autoboost/thread/pthread/pthread_mutex_scoped_lock.hpp>
#include <autoboost/thread/once.hpp>
#include <autoboost/assert.hpp>
#include <autoboost/throw_exception.hpp>
#include <pthread.h>
#include <stdlib.h>
#include <memory>
#include <string.h> // memcmp.
namespace autoboost
{
    namespace thread_detail
    {
        AUTOBOOST_THREAD_DECL uintmax_atomic_t once_global_epoch=AUTOBOOST_THREAD_DETAIL_UINTMAX_ATOMIC_MAX_C;
        AUTOBOOST_THREAD_DECL pthread_mutex_t once_epoch_mutex=PTHREAD_MUTEX_INITIALIZER;
        AUTOBOOST_THREAD_DECL pthread_cond_t once_epoch_cv = PTHREAD_COND_INITIALIZER;

        namespace
        {
            pthread_key_t epoch_tss_key;
            pthread_once_t epoch_tss_key_flag=PTHREAD_ONCE_INIT;

            extern "C"
            {
                static void delete_epoch_tss_data(void* data)
                {
                    free(data);
                }

                static void create_epoch_tss_key()
                {
                    AUTOBOOST_VERIFY(!pthread_key_create(&epoch_tss_key,delete_epoch_tss_data));
                }
            }

            const pthread_once_t pthread_once_init_value=PTHREAD_ONCE_INIT;
            struct AUTOBOOST_THREAD_DECL delete_epoch_tss_key_on_dlclose_t
            {
                delete_epoch_tss_key_on_dlclose_t()
                {
                }
                ~delete_epoch_tss_key_on_dlclose_t()
                {
                    if(memcmp(&epoch_tss_key_flag, &pthread_once_init_value, sizeof(pthread_once_t)))
                    {
                        pthread_key_delete(epoch_tss_key);
                    }
                }
            };
            delete_epoch_tss_key_on_dlclose_t delete_epoch_tss_key_on_dlclose;
        }

        uintmax_atomic_t& get_once_per_thread_epoch()
        {
            AUTOBOOST_VERIFY(!pthread_once(&epoch_tss_key_flag,create_epoch_tss_key));
            void* data=pthread_getspecific(epoch_tss_key);
            if(!data)
            {
                data=malloc(sizeof(thread_detail::uintmax_atomic_t));
                if(!data) AUTOBOOST_THROW_EXCEPTION(std::bad_alloc());
                AUTOBOOST_VERIFY(!pthread_setspecific(epoch_tss_key,data));
                *static_cast<thread_detail::uintmax_atomic_t*>(data)=AUTOBOOST_THREAD_DETAIL_UINTMAX_ATOMIC_MAX_C;
            }
            return *static_cast<thread_detail::uintmax_atomic_t*>(data);
        }
    }

}
#endif //
