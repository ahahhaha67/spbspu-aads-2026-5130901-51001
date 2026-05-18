#ifndef KRIVOSHAPOV_STACK_HPP
#define KRIVOSHAPOV_STACK_HPP

#include <cstddef>
#include <utility>
#include <stdexcept>

namespace krivoshapov
{
  template <typename T>
  class Stack
  {
  public:
    Stack() : data_(nullptr),
              size_(0),
              cap_(0)
    {
    }

    ~Stack()
    {
      deallocate(data_);
    }

  private:
    T *data_;
    size_t size_;
    size_t cap_;

    static T *allocate(size_t n) { return new T[n](); }
    static void deallocate(T *p) noexcept { delete[] p; }
  };
}

#endif
