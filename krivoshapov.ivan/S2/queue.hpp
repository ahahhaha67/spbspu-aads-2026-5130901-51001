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

    Queue(const Queue &rhs) : data_(nullptr),
                              head_(0),
                              count_(0),
                              cap_(0)
    {
      if (rhs.count_ != 0)
      {
        T *buf = allocate(rhs.count_);
        try
        {
          for (size_t i = 0; i < rhs.count_; ++i)
          {
            buf[i] = rhs.data_[(rhs.head_ + i) % rhs.cap_];
          }
        }
        catch (...)
        {
          deallocate(buf);
          throw;
        }
        data_ = buf;
        head_ = 0;
        count_ = rhs.count_;
        cap_ = rhs.count_;
      }
    }

    Queue(Queue &&rhs) noexcept : data_(rhs.data_),
                                  head_(rhs.head_),
                                  count_(rhs.count_),
                                  cap_(rhs.cap_)
    {
      rhs.data_ = nullptr;
      rhs.head_ = 0;
      rhs.count_ = 0;
      rhs.cap_ = 0;
    }

    ~Queue()
    {
      deallocate(data_);
    }

    Queue &operator=(const Queue &rhs)
    {
      if (this != &rhs)
      {
        Queue tmp(rhs);
        swap(tmp);
      }
      return *this;
    }

    Queue &operator=(Queue &&rhs) noexcept
    {
      if (this != &rhs)
      {
        deallocate(data_);
        data_ = rhs.data_;
        head_ = rhs.head_;
        count_ = rhs.count_;
        cap_ = rhs.cap_;
        rhs.data_ = nullptr;
        rhs.head_ = 0;
        rhs.count_ = 0;
        rhs.cap_ = 0;
      }
      return *this;
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

    bool empty() const noexcept { return count_ == 0; }
    size_t size() const noexcept { return count_; }

    void clear() noexcept
    {
      deallocate(data_);
      data_ = nullptr;
      head_ = 0;
      count_ = 0;
      cap_ = 0;
    }

    void swap(Queue &rhs) noexcept
    {
      std::swap(data_, rhs.data_);
      std::swap(head_, rhs.head_);
      std::swap(count_, rhs.count_);
      std::swap(cap_, rhs.cap_);
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
