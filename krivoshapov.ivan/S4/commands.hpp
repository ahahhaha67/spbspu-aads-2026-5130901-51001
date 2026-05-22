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

    inline void cmdPrint(DatasetStore &s,
                         const Array<std::string> &t,
                         std::ostream &out)
    {
      if (t.size() != 2 || !s.has(t[1]))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      const Dict &d = s.at(t[1]);
      if (d.empty())
      {
        out << "<EMPTY>\n";
        return;
      }
      out << t[1];
      for (auto it = d.cbegin(); it != d.cend(); ++it)
      {
        out << ' ' << it.key() << ' ' << it.value();
      }
      out << '\n';
    }

    inline void cmdComplement(DatasetStore &s,
                              const Array<std::string> &t,
                              std::ostream &out)
    {
      if (t.size() != 4 || s.has(t[1]) || !s.has(t[2]) || !s.has(t[3]))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      Dict d1 = s.at(t[2]), d2 = s.at(t[3]), result;
      for (auto it = d1.cbegin(); it != d1.cend(); ++it)
      {
        if (!d2.has(it.key()))
        {
          result.push(it.key(), it.value());
        }
      }
      ensureStore(s);
      s.add(t[1], result);
    }

    inline void cmdIntersect(DatasetStore &s,
                             const Array<std::string> &t,
                             std::ostream &out)
    {
      if (t.size() != 4 || s.has(t[1]) || !s.has(t[2]) || !s.has(t[3]))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      Dict d1 = s.at(t[2]), d2 = s.at(t[3]), result;
      for (auto it = d1.cbegin(); it != d1.cend(); ++it)
      {
        if (d2.has(it.key()))
        {
          result.push(it.key(), it.value());
        }
      }
      ensureStore(s);
      s.add(t[1], result);
    }

    inline void cmdUnion(DatasetStore &s,
                         const Array<std::string> &t,
                         std::ostream &out)
    {
      if (t.size() != 4 || s.has(t[1]) || !s.has(t[2]) || !s.has(t[3]))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      Dict d1 = s.at(t[2]), d2 = s.at(t[3]), result;
      for (auto it = d1.cbegin(); it != d1.cend(); ++it)
      {
        result.push(it.key(), it.value());
      }
      for (auto it = d2.cbegin(); it != d2.cend(); ++it)
      {
        if (!result.has(it.key()))
        {
          result.push(it.key(), it.value());
        }
      }
      ensureStore(s);
      s.add(t[1], result);
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

  inline void runCommands(std::istream &in,
                          std::ostream &out,
                          DatasetStore &store)
  {
    using Handler =
        std::function<void(DatasetStore &, const Array<std::string> &, std::ostream &)>;
    HashTable<std::string, Handler, StringHash, StringEqual> table(16);
    table.add("print", detail::cmdPrint);
    table.add("complement", detail::cmdComplement);
    table.add("intersect", detail::cmdIntersect);
    table.add("union", detail::cmdUnion);

    std::string line;
    while (std::getline(in, line))
    {
      Array<std::string> t = split(line);
      if (t.size() == 0)
      {
        continue;
      }
      if (!table.has(t[0]))
      {
        out << "<INVALID COMMAND>\n";
        continue;
      }
      table.at(t[0])(store, t, out);
    }
  }
}

#endif
