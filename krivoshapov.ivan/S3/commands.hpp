#ifndef KRIVOSHAPOV_COMMANDS_HPP
#define KRIVOSHAPOV_COMMANDS_HPP

#include <istream>
#include <ostream>
#include <string>
#include <functional>
#include "graph.hpp"
#include "hash_table.hpp"
#include "io_utils.hpp"
#include "sha1.hpp"

namespace krivoshapov
{
  using GraphStore = HashTable<std::string, Graph, StringHash, StringEqual>;

  namespace detail
  {
    inline const char *invalidText()
    {
      return "<INVALID COMMAND>";
    }

    inline void ensureStore(GraphStore &s)
    {
      if (s.size() * 10 >= s.slotCount() * 7)
      {
        s.rehash(s.slotCount() * 2);
      }
    }

    inline void printInvalid(std::ostream &out)
    {
      out << invalidText() << "\n";
    }

    inline void printAdjacency(std::ostream &out, const Graph::Adjacency &adj)
    {
      if (adj.size() == 0)
      {
        out << "\n";
        return;
      }
      for (size_t i = 0; i < adj.size(); ++i)
      {
        out << adj[i].first;
        for (size_t j = 0; j < adj[i].second.size(); ++j)
        {
          out << ' ' << adj[i].second[j];
        }
        out << "\n";
      }
    }

    inline void cmdGraphs(GraphStore &s, const Array<std::string> &, std::ostream &out)
    {
      Array<std::string> names;
      for (auto it = s.begin(); it != s.end(); ++it)
      {
        names.push_back(it->first);
      }
      names.sort([](const std::string &a, const std::string &b)
                 { return a < b; });
      if (names.size() == 0)
      {
        out << "\n";
        return;
      }
      for (size_t i = 0; i < names.size(); ++i)
      {
        out << names[i] << "\n";
      }
    }

    inline void cmdVertexes(GraphStore &s, const Array<std::string> &t, std::ostream &out)
    {
      if (t.size() != 2 || !s.has(t[1]))
      {
        printInvalid(out);
        return;
      }
      Graph::NameList v = s.at(t[1]).vertices();
      if (v.size() == 0)
      {
        out << "\n";
        return;
      }
      for (size_t i = 0; i < v.size(); ++i)
      {
        out << v[i] << "\n";
      }
    }

    inline void cmdOutbound(GraphStore &s, const Array<std::string> &t, std::ostream &out)
    {
      if (t.size() != 3 || !s.has(t[1]) || !s.at(t[1]).hasVertex(t[2]))
      {
        printInvalid(out);
        return;
      }
      printAdjacency(out, s.at(t[1]).outbound(t[2]));
    }

    inline void cmdInbound(GraphStore &s, const Array<std::string> &t, std::ostream &out)
    {
      if (t.size() != 3 || !s.has(t[1]) || !s.at(t[1]).hasVertex(t[2]))
      {
        printInvalid(out);
        return;
      }
      printAdjacency(out, s.at(t[1]).inbound(t[2]));
    }

    inline void cmdBind(GraphStore &s, const Array<std::string> &t, std::ostream &out)
    {
      size_t w = 0;
      if (t.size() != 5 || !s.has(t[1]) || !parseSizeT(t[4], w))
      {
        printInvalid(out);
        return;
      }
      s.at(t[1]).addEdge(t[2], t[3], w);
    }

    inline void cmdCut(GraphStore &s, const Array<std::string> &t, std::ostream &out)
    {
      size_t w = 0;
      if (t.size() != 5 || !s.has(t[1]) || !parseSizeT(t[4], w)
          || !s.at(t[1]).cutEdge(t[2], t[3], w))
      {
        printInvalid(out);
        return;
      }
    }

    inline void cmdCreate(GraphStore &s, const Array<std::string> &t, std::ostream &out)
    {
      if (t.size() < 2 || s.has(t[1]))
      {
        printInvalid(out);
        return;
      }
      Graph g;
      for (size_t i = 3; i < t.size(); ++i)
      {
        g.addVertex(t[i]);
      }
      ensureStore(s);
      s.add(t[1], g);
    }

    inline void cmdMerge(GraphStore &s, const Array<std::string> &t, std::ostream &out)
    {
      if (t.size() != 4 || s.has(t[1]) || !s.has(t[2]) || !s.has(t[3]))
      {
        printInvalid(out);
        return;
      }
      Graph g = s.at(t[2]);
      g.absorb(s.at(t[3]));
      ensureStore(s);
      s.add(t[1], g);
    }

    inline void cmdExtract(GraphStore &s, const Array<std::string> &t, std::ostream &out)
    {
      size_t k = 0;
      if (t.size() < 4 || s.has(t[1]) || !s.has(t[2]) || !parseSizeT(t[3], k))
      {
        printInvalid(out);
        return;
      }
      Graph::NameList keep;
      for (size_t i = 4; i < t.size(); ++i)
      {
        keep.push_back(t[i]);
      }
      const Graph &src = s.at(t[2]);
      for (size_t i = 0; i < keep.size(); ++i)
      {
        if (!src.hasVertex(keep[i]))
        {
          printInvalid(out);
          return;
        }
      }
      Graph g = src.extract(keep);
      ensureStore(s);
      s.add(t[1], g);
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

  inline void runCommands(std::istream &in, std::ostream &out, GraphStore &store)
  {
    using Handler =
        std::function<void(GraphStore &, const Array<std::string> &, std::ostream &)>;
    HashTable<std::string, Handler, StringHash, StringEqual> table(16);
    table.add("graphs", detail::cmdGraphs);
    table.add("vertexes", detail::cmdVertexes);
    table.add("outbound", detail::cmdOutbound);
    table.add("inbound", detail::cmdInbound);
    table.add("bind", detail::cmdBind);
    table.add("cut", detail::cmdCut);
    table.add("create", detail::cmdCreate);
    table.add("merge", detail::cmdMerge);
    table.add("extract", detail::cmdExtract);

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
        out << detail::invalidText() << "\n";
        continue;
      }
      table.at(t[0])(store, t, out);
    }
  }
}

#endif
