//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBOOST_INTERPROCESS_DETAIL_SPIN_CONDITION_HPP
#define AUTOBOOST_INTERPROCESS_DETAIL_SPIN_CONDITION_HPP

#ifndef AUTOBOOST_CONFIG_HPP
#  include <autoboost/config.hpp>
#endif
#
#if defined(AUTOBOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <autoboost/interprocess/detail/config_begin.hpp>
#include <autoboost/interprocess/detail/workaround.hpp>
#include <autoboost/interprocess/sync/spin/mutex.hpp>
#include <autoboost/interprocess/detail/posix_time_types_wrk.hpp>
#include <autoboost/interprocess/detail/atomic.hpp>
#include <autoboost/interprocess/sync/scoped_lock.hpp>
#include <autoboost/interprocess/exceptions.hpp>
#include <autoboost/interprocess/detail/os_thread_functions.hpp>
#include <autoboost/interprocess/sync/spin/wait.hpp>
#include <autoboost/move/utility_core.hpp>
#include <autoboost/cstdint.hpp>

namespace autoboost {
namespace interprocess {
namespace ipcdetail {

class spin_condition
{
   spin_condition(const spin_condition &);
   spin_condition &operator=(const spin_condition &);
   public:
   spin_condition();
   ~spin_condition();

   void notify_one();
   void notify_all();

   template <typename L>
   bool timed_wait(L& lock, const autoboost::posix_time::ptime &abs_time)
   {
      if (!lock)
         throw lock_exception();
      //Handle infinity absolute time here to avoid complications in do_timed_wait
      if(abs_time == autoboost::posix_time::pos_infin){
         this->wait(lock);
         return true;
      }
      return this->do_timed_wait(abs_time, *lock.mutex());
   }

   template <typename L, typename Pr>
   bool timed_wait(L& lock, const autoboost::posix_time::ptime &abs_time, Pr pred)
   {
      if (!lock)
         throw lock_exception();
      //Handle infinity absolute time here to avoid complications in do_timed_wait
      if(abs_time == autoboost::posix_time::pos_infin){
         this->wait(lock, pred);
         return true;
      }
      while (!pred()){
         if (!this->do_timed_wait(abs_time, *lock.mutex()))
            return pred();
      }
      return true;
   }

   template <typename L>
   void wait(L& lock)
   {
      if (!lock)
         throw lock_exception();
      do_wait(*lock.mutex());
   }

   template <typename L, typename Pr>
   void wait(L& lock, Pr pred)
   {
      if (!lock)
         throw lock_exception();

      while (!pred())
         do_wait(*lock.mutex());
   }

   template<class InterprocessMutex>
   void do_wait(InterprocessMutex &mut);

   template<class InterprocessMutex>
   bool do_timed_wait(const autoboost::posix_time::ptime &abs_time, InterprocessMutex &mut);

   private:
   template<class InterprocessMutex>
   bool do_timed_wait(bool tout_enabled, const autoboost::posix_time::ptime &abs_time, InterprocessMutex &mut);

   enum { SLEEP = 0, NOTIFY_ONE, NOTIFY_ALL };
   spin_mutex  m_enter_mut;
   volatile autoboost::uint32_t    m_command;
   volatile autoboost::uint32_t    m_num_waiters;
   void notify(autoboost::uint32_t command);
};

inline spin_condition::spin_condition()
{
   //Note that this class is initialized to zero.
   //So zeroed memory can be interpreted as an initialized
   //condition variable
   m_command      = SLEEP;
   m_num_waiters  = 0;
}

inline spin_condition::~spin_condition()
{
   //Notify all waiting threads
   //to allow POSIX semantics on condition destruction
   this->notify_all();
}

inline void spin_condition::notify_one()
{
   this->notify(NOTIFY_ONE);
}

inline void spin_condition::notify_all()
{
   this->notify(NOTIFY_ALL);
}

inline void spin_condition::notify(autoboost::uint32_t command)
{
   //This mutex guarantees that no other thread can enter to the
   //do_timed_wait method logic, so that thread count will be
   //constant until the function writes a NOTIFY_ALL command.
   //It also guarantees that no other notification can be signaled
   //on this spin_condition before this one ends
   m_enter_mut.lock();

   //Return if there are no waiters
   if(!atomic_read32(&m_num_waiters)) {
      m_enter_mut.unlock();
      return;
   }

   //Notify that all threads should execute wait logic
   spin_wait swait;
   while(SLEEP != atomic_cas32(const_cast<autoboost::uint32_t*>(&m_command), command, SLEEP)){
      swait.yield();
   }
   //The enter mutex will rest locked until the last waiting thread unlocks it
}

template<class InterprocessMutex>
inline void spin_condition::do_wait(InterprocessMutex &mut)
{
   this->do_timed_wait(false, autoboost::posix_time::ptime(), mut);
}

template<class InterprocessMutex>
inline bool spin_condition::do_timed_wait
   (const autoboost::posix_time::ptime &abs_time, InterprocessMutex &mut)
{
   return this->do_timed_wait(true, abs_time, mut);
}

template<class InterprocessMutex>
inline bool spin_condition::do_timed_wait(bool tout_enabled,
                                     const autoboost::posix_time::ptime &abs_time,
                                     InterprocessMutex &mut)
{
   autoboost::posix_time::ptime now = microsec_clock::universal_time();

   if(tout_enabled){
      if(now >= abs_time) return false;
   }

   typedef autoboost::interprocess::scoped_lock<spin_mutex> InternalLock;
   //The enter mutex guarantees that while executing a notification,
   //no other thread can execute the do_timed_wait method.
   {
      //---------------------------------------------------------------
      InternalLock lock;
      if(tout_enabled){
         InternalLock dummy(m_enter_mut, abs_time);
         lock = autoboost::move(dummy);
      }
      else{
         InternalLock dummy(m_enter_mut);
         lock = autoboost::move(dummy);
      }

      if(!lock)
         return false;
      //---------------------------------------------------------------
      //We increment the waiting thread count protected so that it will be
      //always constant when another thread enters the notification logic.
      //The increment marks this thread as "waiting on spin_condition"
      atomic_inc32(const_cast<autoboost::uint32_t*>(&m_num_waiters));

      //We unlock the external mutex atomically with the increment
      mut.unlock();
   }

   //By default, we suppose that no timeout has happened
   bool timed_out  = false, unlock_enter_mut= false;

   //Loop until a notification indicates that the thread should
   //exit or timeout occurs
   while(1){
      //The thread sleeps/spins until a spin_condition commands a notification
      //Notification occurred, we will lock the checking mutex so that
      spin_wait swait;
      while(atomic_read32(&m_command) == SLEEP){
         swait.yield();

         //Check for timeout
         if(tout_enabled){
            now = microsec_clock::universal_time();

            if(now >= abs_time){
               //If we can lock the mutex it means that no notification
               //is being executed in this spin_condition variable
               timed_out = m_enter_mut.try_lock();

               //If locking fails, indicates that another thread is executing
               //notification, so we play the notification game
               if(!timed_out){
                  //There is an ongoing notification, we will try again later
                  continue;
               }
               //No notification in execution, since enter mutex is locked.
               //We will execute time-out logic, so we will decrement count,
               //release the enter mutex and return false.
               break;
            }
         }
      }

      //If a timeout occurred, the mutex will not execute checking logic
      if(tout_enabled && timed_out){
         //Decrement wait count
         atomic_dec32(const_cast<autoboost::uint32_t*>(&m_num_waiters));
         unlock_enter_mut = true;
         break;
      }
      else{
         autoboost::uint32_t result = atomic_cas32
                        (const_cast<autoboost::uint32_t*>(&m_command), SLEEP, NOTIFY_ONE);
         if(result == SLEEP){
            //Other thread has been notified and since it was a NOTIFY one
            //command, this thread must sleep again
            continue;
         }
         else if(result == NOTIFY_ONE){
            //If it was a NOTIFY_ONE command, only this thread should
            //exit. This thread has atomically marked command as sleep before
            //so no other thread will exit.
            //Decrement wait count.
            unlock_enter_mut = true;
            atomic_dec32(const_cast<autoboost::uint32_t*>(&m_num_waiters));
            break;
         }
         else{
            //If it is a NOTIFY_ALL command, all threads should return
            //from do_timed_wait function. Decrement wait count.
            unlock_enter_mut = 1 == atomic_dec32(const_cast<autoboost::uint32_t*>(&m_num_waiters));
            //Check if this is the last thread of notify_all waiters
            //Only the last thread will release the mutex
            if(unlock_enter_mut){
               atomic_cas32(const_cast<autoboost::uint32_t*>(&m_command), SLEEP, NOTIFY_ALL);
            }
            break;
         }
      }
   }

   //Unlock the enter mutex if it is a single notification, if this is
   //the last notified thread in a notify_all or a timeout has occurred
   if(unlock_enter_mut){
      m_enter_mut.unlock();
   }

   //Lock external again before returning from the method
   mut.lock();
   return !timed_out;
}

}  //namespace ipcdetail
}  //namespace interprocess
}  //namespace autoboost

#include <autoboost/interprocess/detail/config_end.hpp>

#endif   //AUTOBOOST_INTERPROCESS_DETAIL_SPIN_CONDITION_HPP
