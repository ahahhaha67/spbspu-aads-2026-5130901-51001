#ifndef KRIVOSHAPOV_HASH_TABLE_HPP
#define KRIVOSHAPOV_HASH_TABLE_HPP

#include <cstddef>
#include <utility>
#include <stdexcept>

namespace krivoshapov
{
  template <class Key, class Value, class Hash, class Equal>
  class HashTable
  {
    enum State
    {
      EMPTY,
      OCCUPIED,
      TOMB
    };

    struct Slot
    {
      State st;
      Key key;
      Value val;
      Slot() : st(EMPTY), key(), val()
      {
      }
    };

  public:
    template <class Owner, class Ref>
    class IteratorBase
    {
      friend class HashTable;

    public:
      IteratorBase() : owner_(nullptr), idx_(0)
      {
      }

      Ref &operator*() const
      {
        return owner_->pairs_[idx_];
      }

      Ref *operator->() const
      {
        return std::addressof(owner_->pairs_[idx_]);
      }

      IteratorBase &operator++()
      {
        ++idx_;
        skip();
        return *this;
      }

      bool operator==(const IteratorBase &rhs) const
      {
        return owner_ == rhs.owner_ && idx_ == rhs.idx_;
      }

      bool operator!=(const IteratorBase &rhs) const
      {
        return !(*this == rhs);
      }

    private:
      Owner *owner_;
      size_t idx_;

      IteratorBase(Owner *o, size_t i) : owner_(o), idx_(i)
      {
        skip();
      }

      void skip()
      {
        while (idx_ < owner_->cap_ && owner_->slots_[idx_].st != OCCUPIED)
        {
          ++idx_;
        }
      }
    };

    using Pair = std::pair<const Key, Value>;
    using iterator = IteratorBase<HashTable, Pair>;
    using const_iterator = IteratorBase<const HashTable, const Pair>;

    HashTable() : HashTable(8)
    {
    }

    explicit HashTable(size_t slots)
        : slots_(nullptr), pairs_(nullptr),
          cap_(slots < 1 ? 1 : slots), size_(0), tomb_(0)
    {
      slots_ = new Slot[cap_];
      pairs_ = static_cast<Pair *>(::operator new(cap_ * sizeof(Pair)));
    }

    HashTable(const HashTable &rhs) : HashTable(rhs.cap_)
    {
      for (size_t i = 0; i < rhs.cap_; ++i)
      {
        if (rhs.slots_[i].st == OCCUPIED)
        {
          add(rhs.slots_[i].key, rhs.slots_[i].val);
        }
      }
    }

    HashTable(HashTable &&rhs) noexcept
        : slots_(rhs.slots_), pairs_(rhs.pairs_),
          cap_(rhs.cap_), size_(rhs.size_), tomb_(rhs.tomb_)
    {
      rhs.slots_ = nullptr;
      rhs.pairs_ = nullptr;
      rhs.cap_ = 0;
      rhs.size_ = 0;
      rhs.tomb_ = 0;
    }

    HashTable &operator=(const HashTable &rhs)
    {
      if (this != &rhs)
      {
        HashTable tmp(rhs);
        swap(tmp);
      }
      return *this;
    }

    HashTable &operator=(HashTable &&rhs) noexcept
    {
      if (this != &rhs)
      {
        destroy();
        slots_ = rhs.slots_;
        pairs_ = rhs.pairs_;
        cap_ = rhs.cap_;
        size_ = rhs.size_;
        tomb_ = rhs.tomb_;
        rhs.slots_ = nullptr;
        rhs.pairs_ = nullptr;
        rhs.cap_ = 0;
        rhs.size_ = 0;
        rhs.tomb_ = 0;
      }
      return *this;
    }

    ~HashTable()
    {
      destroy();
    }

    bool has(const Key &k) const
    {
      return findIndex(k) != cap_;
    }

    void add(const Key &k, const Value &v)
    {
      size_t fi = findIndex(k);
      if (fi != cap_)
      {
        slots_[fi].val = v;
        pairs_[fi].second = v;
        return;
      }
      size_t h = Hash()(k) % cap_;
      for (size_t step = 0; step < cap_; ++step)
      {
        size_t i = (h + step) % cap_;
        if (slots_[i].st != OCCUPIED)
        {
          bool wasTomb = slots_[i].st == TOMB;
          if (wasTomb)
          {
            pairs_[i].~Pair();
          }
          slots_[i].key = k;
          slots_[i].val = v;
          slots_[i].st = OCCUPIED;
          new (pairs_ + i) Pair(k, v);
          ++size_;
          if (wasTomb)
          {
            --tomb_;
          }
          return;
        }
      }
      throw std::length_error("hash table is full");
    }

    Value drop(const Key &k)
    {
      size_t i = findIndex(k);
      if (i == cap_)
      {
        throw std::out_of_range("key not found");
      }
      Value out = slots_[i].val;
      slots_[i].st = TOMB;
      --size_;
      ++tomb_;
      return out;
    }

    Value &at(const Key &k)
    {
      size_t i = findIndex(k);
      if (i == cap_)
      {
        throw std::out_of_range("key not found");
      }
      return slots_[i].val;
    }

    const Value &at(const Key &k) const
    {
      size_t i = findIndex(k);
      if (i == cap_)
      {
        throw std::out_of_range("key not found");
      }
      return slots_[i].val;
    }

    void rehash(size_t slots)
    {
      if (slots <= size_)
      {
        slots = size_ + 1;
      }
      HashTable tmp(slots);
      for (size_t i = 0; i < cap_; ++i)
      {
        if (slots_[i].st == OCCUPIED)
        {
          tmp.add(slots_[i].key, slots_[i].val);
        }
      }
      swap(tmp);
    }

    size_t size() const noexcept
    {
      return size_;
    }

    size_t slotCount() const noexcept
    {
      return cap_;
    }

    bool empty() const noexcept
    {
      return size_ == 0;
    }

    iterator begin()
    {
      return iterator(this, 0);
    }

    iterator end()
    {
      return iterator(this, cap_);
    }

    const_iterator begin() const
    {
      return const_iterator(this, 0);
    }

    const_iterator end() const
    {
      return const_iterator(this, cap_);
    }

    const_iterator cbegin() const
    {
      return begin();
    }

    const_iterator cend() const
    {
      return end();
    }

    void swap(HashTable &rhs) noexcept
    {
      std::swap(slots_, rhs.slots_);
      std::swap(pairs_, rhs.pairs_);
      std::swap(cap_, rhs.cap_);
      std::swap(size_, rhs.size_);
      std::swap(tomb_, rhs.tomb_);
    }

  private:
    Slot *slots_;
    Pair *pairs_;
    size_t cap_;
    size_t size_;
    size_t tomb_;

    void destroy() noexcept
    {
      if (pairs_)
      {
        for (size_t i = 0; i < cap_; ++i)
        {
          if (slots_[i].st != EMPTY)
          {
            pairs_[i].~Pair();
          }
        }
        ::operator delete(pairs_);
      }
      delete[] slots_;
      slots_ = nullptr;
      pairs_ = nullptr;
    }

    size_t findIndex(const Key &k) const
    {
      if (cap_ == 0)
      {
        return cap_;
      }
      size_t h = Hash()(k) % cap_;
      for (size_t step = 0; step < cap_; ++step)
      {
        size_t i = (h + step) % cap_;
        if (slots_[i].st == EMPTY)
        {
          return cap_;
        }
        if (slots_[i].st == OCCUPIED && Equal()(slots_[i].key, k))
        {
          return i;
        }
      }
      return cap_;
    }
  };
}

#endif
