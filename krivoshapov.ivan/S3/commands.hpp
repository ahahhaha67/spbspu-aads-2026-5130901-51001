#ifndef KRIVOSHAPOV_COMMANDS_HPP
#define KRIVOSHAPOV_COMMANDS_HPP

#include <istream>
#include <ostream>
#include <string>
#include "graph.hpp"
#include "hash_table.hpp"
#include "io_utils.hpp"
#include "sha1.hpp"

namespace krivoshapov
{
  using GraphStore = HashTable<std::string, Graph, StringHash, StringEqual>;

  namespace detail
  {
    inline void ensureStore(GraphStore &s)
    {
      if (s.size() * 10 >= s.slotCount() * 7)
      {
        s.rehash(s.slotCount() * 2);
      }
    }
  }

  inline void loadGraphs(std::istream &in, GraphStore &store)
  {
    std::string line;
    while (std::getline(in, line))
    {
      Array<std::string> header = split(line);
      if (header.size() == 0)
      {
        continue;
      }
      size_t count = 0;
      if (header.size() < 2 || !parseSizeT(header[1], count))
      {
        continue;
      }
      Graph g;
      size_t read = 0;
      while (read < count && std::getline(in, line))
      {
        Array<std::string> e = split(line);
        if (e.size() == 0)
        {
          continue;
        }
        size_t w = 0;
        if (e.size() >= 3 && parseSizeT(e[2], w))
        {
          g.addEdge(e[0], e[1], w);
        }
        ++read;
      }
      detail::ensureStore(store);
      if (store.has(header[0]))
      {
        store.at(header[0]).absorb(g);
      }
      else
      {
        store.add(header[0], g);
      }
    }
  }
}

#endif
