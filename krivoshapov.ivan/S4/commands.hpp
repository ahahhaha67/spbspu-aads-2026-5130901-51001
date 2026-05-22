#ifndef KRIVOSHAPOV_COMMANDS_HPP
#define KRIVOSHAPOV_COMMANDS_HPP

#include <functional>
#include <istream>
#include <ostream>
#include <string>
#include "bst.hpp"
#include "hash_table.hpp"
#include "hash_utils.hpp"
#include "io_utils.hpp"

namespace krivoshapov
{
  struct IntLess
  {
    bool operator()(int a, int b) const { return a < b; }
  };

  using Dict = BSTree<int, std::string, IntLess>;
  using DatasetStore = HashTable<std::string, Dict, StringHash, StringEqual>;

  namespace detail
  {
    inline void ensureStore(DatasetStore &s)
    {
      if (s.size() * 10 >= s.slotCount() * 7)
      {
        s.rehash(s.slotCount() * 2);
      }
    }
  } // namespace detail

  inline void loadDatasets(std::istream &in, DatasetStore &store)
  {
    std::string line;
    while (std::getline(in, line))
    {
      Array<std::string> tokens = split(line);
      if (tokens.size() == 0)
      {
        continue;
      }
      Dict dict;
      size_t i = 1;
      while (i + 1 < tokens.size())
      {
        int key = 0;
        if (parseInt(tokens[i], key))
        {
          dict.push(key, tokens[i + 1]);
        }
        i += 2;
      }
      detail::ensureStore(store);
      store.add(tokens[0], dict);
    }
  }
}

#endif
