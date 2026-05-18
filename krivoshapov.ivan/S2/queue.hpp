#ifndef KRIVOSHAPOV_QUEUE_HPP
#define KRIVOSHAPOV_QUEUE_HPP

#include <cstddef>
#include <utility>
#include <stdexcept>

namespace krivoshapov
{
  template <typename T>
  class Queue
  {
  public:
    Queue() : data_(nullptr),
              head_(0),
              count_(0),
              cap_(0)
    {
    }

    ~Queue()
    {
      deallocate(data_);
    }

    void push(const T &value)
    {
      ensure();
      data_[(head_ + count_) % cap_] = value;
      ++count_;
    }

    void push(T &&value)
    {
      ensure();
      data_[(head_ + count_) % cap_] = std::move(value);
      ++count_;
    }

    void pop()
    {
      if (count_ == 0)
      {
        throw std::out_of_range("pop from empty queue");
      }
      data_[head_] = T();
      head_ = (head_ + 1) % cap_;
      --count_;
    }

    T &front()
    {
      if (count_ == 0)
      {
        throw std::out_of_range("front from empty queue");
      }
      return data_[head_];
    }

    const T &front() const
    {
      if (count_ == 0)
      {
        throw std::out_of_range("front from empty queue");
      }
      return data_[head_];
    }

  private:
    T *data_;
    size_t head_;
    size_t count_;
    size_t cap_;

    static T *allocate(size_t n) { return new T[n](); }
    static void deallocate(T *p) noexcept { delete[] p; }

    void ensure()
    {
      if (count_ < cap_)
      {
        return;
      }
      size_t ncap = (cap_ == 0) ? 8 : cap_ * 2;
      T *buf = allocate(ncap);
      try
      {
        for (size_t i = 0; i < count_; ++i)
        {
          buf[i] = std::move_if_noexcept(data_[(head_ + i) % cap_]);
        }
      }
      catch (...)
      {
        deallocate(buf);
        throw;
      }
      deallocate(data_);
      data_ = buf;
      head_ = 0;
      cap_ = ncap;
    }
  };
}

#endif
