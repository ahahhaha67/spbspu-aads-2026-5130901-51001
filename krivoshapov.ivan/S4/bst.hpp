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
    BSTIterator(Node *n, Node *fr, Node *fl) : node_(n), fake_root_(fr), fake_leaf_(fl) {}
  };

  template <class Key, class Value>
  class BSTConstIterator
  {
    template <class K, class V, class C>
    friend class BSTree;

  public:
    using Node = detail::BSTNode<Key, Value>;
    BSTConstIterator(const BSTIterator<Key, Value> &it) : node_(it.node_), fake_root_(it.fake_root_), fake_leaf_(it.fake_leaf_) {}
    const Key &key() const { return node_->key_; }
    const Value &value() const { return node_->value_; }
    BSTConstIterator &operator++()
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
    BSTConstIterator operator++(int)
    {
      BSTConstIterator tmp = *this;
      ++(*this);
      return tmp;
    }
    bool operator==(const BSTConstIterator &rhs) const { return node_ == rhs.node_; }
    bool operator!=(const BSTConstIterator &rhs) const { return node_ != rhs.node_; }

  private:
    Node *node_;
    Node *fake_root_;
    Node *fake_leaf_;
    BSTConstIterator(Node *n, Node *fr, Node *fl) : node_(n), fake_root_(fr), fake_leaf_(fl) {}
  };

  template <class Key, class Value, class Compare>
  class BSTree
  {
  public:
    using Node = detail::BSTNode<Key, Value>;
    using iterator = BSTIterator<Key, Value>;
    using const_iterator = BSTConstIterator<Key, Value>;

    BSTree() : fake_leaf_(new Node()),
               fake_root_(new Node())
    {
      initSentinels();
    }

    ~BSTree()
    {
      if (fake_root_ != nullptr)
      {
        destroyTree();
      }
    }

  private:
    Node *fake_leaf_;
    Node *fake_root_;

    void initSentinels()
    {
      fake_leaf_->left_ = fake_leaf_;
      fake_leaf_->right_ = fake_leaf_;
      fake_leaf_->parent_ = nullptr;
      fake_root_->left_ = fake_leaf_;
      fake_root_->right_ = fake_leaf_;
      fake_root_->parent_ = nullptr;
    }

    void destroyTree()
    {
      clearSubtree(fake_root_->left_);
      delete fake_root_;
      delete fake_leaf_;
      fake_root_ = nullptr;
      fake_leaf_ = nullptr;
    }

    void clearSubtree(Node *n)
    {
      if (n == fake_leaf_)
      {
        return;
      }
      clearSubtree(n->left_);
      clearSubtree(n->right_);
      delete n;
    }
  };
  template <class Key, class Value, class Compare>
  class BSTree
  {
  public:
    using Node = detail::BSTNode<Key, Value>;
    using iterator = BSTIterator<Key, Value>;
    using const_iterator = BSTConstIterator<Key, Value>;

    BSTree() : fake_leaf_(new Node()), fake_root_(new Node()) { initSentinels(); }
    ~BSTree()
    {
      if (fake_root_ != nullptr)
      {
        destroyTree();
      }
    }

    void push(const Key &k, const Value &v)
    {
      Node *root = fake_root_->left_;
      if (root == fake_leaf_)
      {
        fake_root_->left_ = new Node(k, v, fake_leaf_, fake_leaf_, fake_root_);
        return;
      }
      Node *cur = root;
      for (;;)
      {
        if (Compare()(k, cur->key_))
        {
          if (cur->left_ == fake_leaf_)
          {
            cur->left_ = new Node(k, v, fake_leaf_, fake_leaf_, cur);
            return;
          }
          cur = cur->left_;
        }
        else if (Compare()(cur->key_, k))
        {
          if (cur->right_ == fake_leaf_)
          {
            cur->right_ = new Node(k, v, fake_leaf_, fake_leaf_, cur);
            return;
          }
          cur = cur->right_;
        }
        else
        {
          cur->value_ = v;
          return;
        }
      }
      Value &get(const Key &k)
      {
        Node *n = findNode(k);
        if (n == fake_leaf_)
        {
          throw std::out_of_range("key not found");
        }
        return n->value_;
      }

      const Value &get(const Key &k) const
      {
        Node *n = findNode(k);
        if (n == fake_leaf_)
        {
          throw std::out_of_range("key not found");
        }
        return n->value_;
      }

      bool has(const Key &k) const { return findNode(k) != fake_leaf_; }
    }
    Value drop(const Key &k)
    {
      Node *n = findNode(k);
      if (n == fake_leaf_)
      {
        throw std::out_of_range("key not found");
      }
      Value v = n->value_;
      removeNode(n);
      return v;
    }

  private:
    Node *fake_leaf_;
    Node *fake_root_;
    Node *findNode(const Key &k) const
    {
      Node *cur = fake_root_->left_;
      while (cur != fake_leaf_)
      {
        if (Compare()(k, cur->key_))
        {
          cur = cur->left_;
        }
        else if (Compare()(cur->key_, k))
        {
          cur = cur->right_;
        }
        else
        {
          return cur;
        }
      }
      return fake_leaf_;
    }
    void replaceChild(Node *parent, Node *oldChild, Node *newChild)
    {
      if (parent->left_ == oldChild)
      {
        parent->left_ = newChild;
      }
      else
      {
        parent->right_ = newChild;
      }
      if (newChild != fake_leaf_)
      {
        newChild->parent_ = parent;
      }
    }

    void removeNode(Node *n)
    {
      if (n->left_ != fake_leaf_ && n->right_ != fake_leaf_)
      {
        Node *s = n->right_;
        while (s->left_ != fake_leaf_)
        {
          s = s->left_;
        }
        n->key_ = s->key_;
        n->value_ = s->value_;
        removeNode(s);
        return;
      }
      Node *child = (n->left_ != fake_leaf_) ? n->left_ : n->right_;
      replaceChild(n->parent_, n, child);
      delete n;
    }
  };
}

#endif
