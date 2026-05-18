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
      Slot() : st(EMPTY), key(), val() {}
    };

  public:
    explicit HashTable(size_t slots) : slots_(nullptr), cap_(slots < 1 ? 1 : slots), size_(0), tomb_(0)
    {
      slots_ = new Slot[cap_];
    }

    HashTable() : HashTable(8) {}

    ~HashTable() { delete[] slots_; }

    size_t size() const noexcept { return size_; }
    size_t slotCount() const noexcept { return cap_; }

  private:
    Slot *slots_;
    size_t cap_;
    size_t size_;
    size_t tomb_;

    size_t findIndex(const Key &k) const
    {
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
