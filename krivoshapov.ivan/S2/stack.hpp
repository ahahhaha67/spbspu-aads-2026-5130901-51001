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

    Stack(const Stack &rhs) : data_(nullptr),
                              size_(0),
                              cap_(0)
    {
      if (rhs.size_ != 0)
      {
        T *buf = allocate(rhs.cap_);
        try
        {
          for (size_t i = 0; i < rhs.size_; ++i)
          {
            buf[i] = rhs.data_[i];
          }
        }
        catch (...)
        {
          deallocate(buf);
          throw;
        }
        data_ = buf;
        size_ = rhs.size_;
        cap_ = rhs.cap_;
      }
    }

    ~Stack()
    {
      deallocate(data_);
    }

    Stack &operator=(const Stack &rhs)
    {
      if (this != &rhs)
      {
        Stack tmp(rhs);
        swap(tmp);
      }
      return *this;
    }

    void push(const T &value)
    {
      ensure();
      data_[size_] = value;
      ++size_;
    }

    void push(T &&value)
    {
      ensure();
      data_[size_] = std::move(value);
      ++size_;
    }

    void pop()
    {
      if (size_ == 0)
      {
        throw std::out_of_range("pop from empty stack");
      }
      --size_;
      data_[size_] = T();
    }

    T &top()
    {
      if (size_ == 0)
      {
        throw std::out_of_range("top from empty stack");
      }
      return data_[size_ - 1];
    }

    const T &top() const
    {
      if (size_ == 0)
      {
        throw std::out_of_range("top from empty stack");
      }
      return data_[size_ - 1];
    }

    bool empty() const noexcept { return size_ == 0; }
    size_t size() const noexcept { return size_; }

    void clear() noexcept
    {
      deallocate(data_);
      data_ = nullptr;
      size_ = 0;
      cap_ = 0;
    }

    void swap(Stack &rhs) noexcept
    {
      std::swap(data_, rhs.data_);
      std::swap(size_, rhs.size_);
      std::swap(cap_, rhs.cap_);
    }

  private:
    T *data_;
    size_t size_;
    size_t cap_;

    static T *allocate(size_t n) { return new T[n](); }
    static void deallocate(T *p) noexcept { delete[] p; }

    void ensure()
    {
      if (size_ < cap_)
      {
        return;
      }
      size_t ncap = (cap_ == 0) ? 8 : cap_ * 2;
      T *buf = allocate(ncap);
      try
      {
        for (size_t i = 0; i < size_; ++i)
        {
          buf[i] = std::move_if_noexcept(data_[i]);
        }
      }
      catch (...)
      {
        deallocate(buf);
        throw;
      }
      deallocate(data_);
      data_ = buf;
      cap_ = ncap;
    }
  };
}

#endif
