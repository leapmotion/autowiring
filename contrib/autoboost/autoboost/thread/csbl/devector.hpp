// Copyright (C) 2013 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// 2013/10 Vicente J. Botet Escriba
//   Creation.

#ifndef AUTOBOOST_CSBL_DEVECTOR_HPP
#define AUTOBOOST_CSBL_DEVECTOR_HPP

#include <autoboost/config.hpp>

#include <autoboost/thread/csbl/vector.hpp>
#include <autoboost/move/detail/move_helpers.hpp>

namespace autoboost
{
  namespace csbl
  {
    template <class T>
    class devector
    {
      typedef csbl::vector<T> vector_type;
      vector_type data_;
      std::size_t front_index_;

      AUTOBOOST_COPYABLE_AND_MOVABLE(devector)

      template <class U>
      void priv_push_back(AUTOBOOST_FWD_REF(U) x)
      { data_.push_back(autoboost::forward<U>(x)); }

    public:
      typedef typename vector_type::size_type size_type;
      typedef typename vector_type::reference reference;
      typedef typename vector_type::const_reference const_reference;


      devector() : front_index_(0) {}
      devector(devector const& x) AUTOBOOST_NOEXCEPT
         :  data_(x.data_),
            front_index_(x.front_index_)
      {}
      devector(AUTOBOOST_RV_REF(devector) x) AUTOBOOST_NOEXCEPT
         :  data_(autoboost::move(x.data_)),
            front_index_(x.front_index_)
      {}

      devector& operator=(AUTOBOOST_COPY_ASSIGN_REF(devector) x)
      {
         if (&x != this)
         {
           data_ = x.data_;
           front_index_ = x.front_index_;
         }
         return *this;
      }

      devector& operator=(AUTOBOOST_RV_REF(devector) x)
#if defined AUTOBOOST_THREAD_USES_AUTOBOOST_VECTOR
         AUTOBOOST_NOEXCEPT_IF(vector_type::allocator_traits_type::propagate_on_container_move_assignment::value)
#endif
      {
        data_ = autoboost::move(x.data_);
        front_index_ = x.front_index_;
        return *this;
      }

      bool empty() const AUTOBOOST_NOEXCEPT
      { return data_.size() == front_index_; }

      size_type size() const AUTOBOOST_NOEXCEPT
      { return data_.size() - front_index_; }

      reference         front() AUTOBOOST_NOEXCEPT
      { return data_[front_index_]; }

      const_reference         front() const AUTOBOOST_NOEXCEPT
      { return data_[front_index_]; }

      reference         back() AUTOBOOST_NOEXCEPT
      { return data_.back(); }

      const_reference         back() const AUTOBOOST_NOEXCEPT
      { return data_.back(); }

      AUTOBOOST_MOVE_CONVERSION_AWARE_CATCH(push_back, T, void, priv_push_back)

      void pop_front()
      {
        ++front_index_;
        if (empty()) {
          data_.clear();
          front_index_=0;
        }
       }

    };
  }
}
#endif // header
