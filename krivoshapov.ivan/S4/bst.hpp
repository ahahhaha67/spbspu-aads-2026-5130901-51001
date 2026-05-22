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

      BSTNode() : key_(), value_(), left_(nullptr), right_(nullptr), parent_(nullptr)
      {
      }

      BSTNode(const Key &k, const Value &v,
              BSTNode *l, BSTNode *r, BSTNode *p) : key_(k), value_(v), left_(l), right_(r), parent_(p)
      {
      }
    };
  }

  template <class Key, class Value>
  class BSTConstIterator;

  template <class Key, class Value>
  class BSTIterator
  {
    template <class K, class V, class C>
    friend class BSTree;
    friend class BSTConstIterator<Key, Value>;

  public:
    using Node = detail::BSTNode<Key, Value>;

    const Key &key() const { return node_->key_; }
    Value &value() { return node_->value_; }
    const Value &value() const { return node_->value_; }

    BSTIterator &operator++()
    {
      if (node_->right_ != fake_leaf_)
      {
        node_ = node_->right_;
        while (node_->left_ != fake_leaf_)
        {
          node_ = node_->left_;
        }
      }
      else
      {
        while (node_->parent_ != nullptr && node_ == node_->parent_->right_)
        {
          node_ = node_->parent_;
        }
        node_ = node_->parent_;
      }
      return *this;
    }

    BSTIterator operator++(int)
    {
      BSTIterator tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const BSTIterator &rhs) const { return node_ == rhs.node_; }
    bool operator!=(const BSTIterator &rhs) const { return node_ != rhs.node_; }

  private:
    Node *node_;
    Node *fake_root_;
    Node *fake_leaf_;

    BSTIterator(Node *n, Node *fr, Node *fl) : node_(n), fake_root_(fr), fake_leaf_(fl)
    {
    }
  };
}

#endif
