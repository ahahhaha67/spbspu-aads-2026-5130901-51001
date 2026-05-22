#ifndef KRIVOSHAPOV_DYNAMIC_ARRAY_HPP
#define KRIVOSHAPOV_DYNAMIC_ARRAY_HPP

#include <cstddef>
#include <utility>
#include <stdexcept>

namespace krivoshapov
{
  template <class T>
  class Array
  {
  public:
    Array() : data_(nullptr), size_(0), cap_(0) {}

    Array(const Array &rhs) : data_(nullptr), size_(0), cap_(0)
    {
      reserve(rhs.size_);
      for (size_t i = 0; i < rhs.size_; ++i)
      {
        new (data_ + i) T(rhs.data_[i]);
      }
      size_ = rhs.size_;
    }

    Array(Array &&rhs) noexcept : data_(rhs.data_), size_(rhs.size_), cap_(rhs.cap_)
    {
      rhs.data_ = nullptr;
      rhs.size_ = 0;
      rhs.cap_ = 0;
    }

    Array &operator=(const Array &rhs)
    {
      if (this != &rhs)
      {
        Array tmp(rhs);
        swap(tmp);
      }
      return *this;
    }

    Array &operator=(Array &&rhs) noexcept
    {
      if (this != &rhs)
      {
        clear_storage();
        data_ = rhs.data_;
        size_ = rhs.size_;
        cap_ = rhs.cap_;
        rhs.data_ = nullptr;
        rhs.size_ = 0;
        rhs.cap_ = 0;
      }
      return *this;
    }

    ~Array() { clear_storage(); }

    void push_back(const T &v)
    {
      ensure(size_ + 1);
      new (data_ + size_) T(v);
      ++size_;
    }

    void push_back(T &&v)
    {
      ensure(size_ + 1);
      new (data_ + size_) T(std::move(v));
      ++size_;
    }

    size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }
    T &operator[](size_t i) { return data_[i]; }
    const T &operator[](size_t i) const { return data_[i]; }

    void clear() noexcept
    {
      for (size_t i = 0; i < size_; ++i)
      {
        data_[i].~T();
      }
      size_ = 0;
    }

    template <class Less>
    void sort(Less less)
    {
      for (size_t i = 1; i < size_; ++i)
      {
        size_t j = i;
        while (j > 0 && less(data_[j], data_[j - 1]))
        {
          std::swap(data_[j], data_[j - 1]);
          --j;
        }
      }
    }

    void swap(Array &rhs) noexcept
    {
      std::swap(data_, rhs.data_);
      std::swap(size_, rhs.size_);
      std::swap(cap_, rhs.cap_);
    }

  private:
    T *data_;
    size_t size_;
    size_t cap_;

    void clear_storage() noexcept
    {
      clear();
      ::operator delete(data_);
      data_ = nullptr;
      cap_ = 0;
    }

    void reserve(size_t n)
    {
      if (n <= cap_)
      {
        return;
      }
      T *nd = static_cast<T *>(::operator new(n * sizeof(T)));
      size_t i = 0;
      try
      {
        for (; i < size_; ++i)
        {
          new (nd + i) T(std::move(data_[i]));
        }
      }
      catch (...)
      {
        for (size_t k = 0; k < i; ++k)
        {
          nd[k].~T();
        }
        ::operator delete(nd);
        throw;
      }
      for (size_t k = 0; k < size_; ++k)
      {
        data_[k].~T();
      }
      ::operator delete(data_);
      data_ = nd;
      cap_ = n;
    }

    void ensure(size_t n)
    {
      if (n <= cap_)
      {
        return;
      }
      size_t nc = cap_ == 0 ? 4 : cap_ * 2;
      if (nc < n)
      {
        nc = n;
      }
      reserve(nc);
    }
  };
}

#endif
