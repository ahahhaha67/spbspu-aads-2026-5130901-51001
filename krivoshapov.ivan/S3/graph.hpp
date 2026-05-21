#ifndef KRIVOSHAPOV_GRAPH_HPP
#define KRIVOSHAPOV_GRAPH_HPP

#include <string>
#include <utility>
#include "dynamic_array.hpp"
#include "hash_table.hpp"
#include "sha1.hpp"

namespace krivoshapov
{
  class Graph
  {
  public:
    using Weights = Array<size_t>;
    using NameList = Array<std::string>;
    using Adjacency = Array<std::pair<std::string, Weights>>;

    Graph() : vertices_(8), edges_(8) {}

    void addVertex(const std::string &name)
    {
      if (!vertices_.has(name))
      {
        growVertices();
        vertices_.add(name, 1);
      }
    }

    bool hasVertex(const std::string &name) const
    {
      return vertices_.has(name);
    }

    void addEdge(const std::string &a, const std::string &b, size_t w)
    {
      addVertex(a);
      addVertex(b);
      VertexPair key(a, b);
      if (edges_.has(key))
      {
        edges_.at(key).push_back(w);
      }
      else
      {
        growEdges();
        Weights ws;
        ws.push_back(w);
        edges_.add(key, ws);
      }
    }

    bool cutEdge(const std::string &a, const std::string &b, size_t w)
    {
      if (!vertices_.has(a) || !vertices_.has(b))
      {
        return false;
      }
      VertexPair key(a, b);
      if (!edges_.has(key))
      {
        return false;
      }
      Weights &ws = edges_.at(key);
      size_t found = ws.size();
      for (size_t i = 0; i < ws.size(); ++i)
      {
        if (ws[i] == w)
        {
          found = i;
          break;
        }
      }
      if (found == ws.size())
      {
        return false;
      }
      Weights nw;
      for (size_t i = 0; i < ws.size(); ++i)
      {
        if (i != found)
        {
          nw.push_back(ws[i]);
        }
      }
      if (nw.empty())
      {
        edges_.drop(key);
      }
      else
      {
        ws = nw;
      }
      return true;
    }

    NameList vertices() const
    {
      NameList out;
      for (auto it = vertices_.begin(); it != vertices_.end(); ++it)
      {
        out.push_back(it->first);
      }
      out.sort([](const std::string &a, const std::string &b)
               { return a < b; });
      return out;
    }

    Adjacency outbound(const std::string &v) const { return collect(v, true); }
    Adjacency inbound(const std::string &v) const { return collect(v, false); }

    void absorb(const Graph &other)
    {
      for (auto it = other.vertices_.begin(); it != other.vertices_.end(); ++it)
      {
        addVertex(it->first);
      }
      for (auto it = other.edges_.begin(); it != other.edges_.end(); ++it)
      {
        const Weights &ws = it->second;
        for (size_t i = 0; i < ws.size(); ++i)
        {
          addEdge(it->first.first, it->first.second, ws[i]);
        }
      }
    }

    Graph extract(const NameList &keep) const
    {
      Graph g;
      for (size_t i = 0; i < keep.size(); ++i)
      {
        g.addVertex(keep[i]);
      }
      for (auto it = edges_.begin(); it != edges_.end(); ++it)
      {
        bool a = false;
        bool b = false;
        for (size_t i = 0; i < keep.size(); ++i)
        {
          if (keep[i] == it->first.first)
          {
            a = true;
          }
          if (keep[i] == it->first.second)
          {
            b = true;
          }
        }
        if (a && b)
        {
          const Weights &ws = it->second;
          for (size_t i = 0; i < ws.size(); ++i)
          {
            g.addEdge(it->first.first, it->first.second, ws[i]);
          }
        }
      }
      return g;
    }

  private:
    using VertexSet = HashTable<std::string, char, StringHash, StringEqual>;
    using EdgeMap = HashTable<VertexPair, Weights, PairHash, PairEqual>;

    VertexSet vertices_;
    EdgeMap edges_;

    void growVertices()
    {
      if (vertices_.size() * 10 >= vertices_.slotCount() * 7)
      {
        vertices_.rehash(vertices_.slotCount() * 2);
      }
    }

    void growEdges()
    {
      if (edges_.size() * 10 >= edges_.slotCount() * 7)
      {
        edges_.rehash(edges_.slotCount() * 2);
      }
    }

    Adjacency collect(const std::string &v, bool out) const
    {
      Adjacency res;
      for (auto it = edges_.begin(); it != edges_.end(); ++it)
      {
        const std::string &src = it->first.first;
        const std::string &dst = it->first.second;
        const std::string &match = out ? src : dst;
        const std::string &other = out ? dst : src;
        if (match != v)
        {
          continue;
        }
        size_t at = res.size();
        for (size_t i = 0; i < res.size(); ++i)
        {
          if (res[i].first == other)
          {
            at = i;
            break;
          }
        }
        if (at == res.size())
        {
          res.push_back(std::make_pair(other, Weights()));
        }
        const Weights &ws = it->second;
        for (size_t i = 0; i < ws.size(); ++i)
        {
          res[at].second.push_back(ws[i]);
        }
      }
      res.sort([](const std::pair<std::string, Weights> &x,
                  const std::pair<std::string, Weights> &y)
               { return x.first < y.first; });
      for (size_t i = 0; i < res.size(); ++i)
      {
        res[i].second.sort([](const size_t &x, const size_t &y)
                           { return x < y; });
      }
      return res;
    }
  };
}

#endif
