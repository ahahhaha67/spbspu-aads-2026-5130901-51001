#ifndef KRIVOSHAPOV_BST_HPP
#define KRIVOSHAPOV_BST_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>

namespace krivoshapov
{
  namespace detail
  {
    template <class Key, class Value>
    struct BSTNode
    {
      Key key_;
      Value value_;
      BSTNode *left_;
      BSTNode *right_;
      BSTNode *parent_;

      BSTNode() : key_(),
                  value_(),
                  left_(nullptr),
                  right_(nullptr),
                  parent_(nullptr)
      {
      }

      BSTNode(const Key &k, const Value &v,
              BSTNode *l, BSTNode *r, BSTNode *p) : key_(k),
                                                    value_(v),
                                                    left_(l),
                                                    right_(r),
                                                    parent_(p)
      {
      }
    };
  }
}

#endif
