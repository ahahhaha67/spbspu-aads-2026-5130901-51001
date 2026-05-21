#include <iostream>
#include <string>
#include "dynamic_array.hpp"
#include "hash_table.hpp"
#include "sha1.hpp"
#include "graph.hpp"

namespace
{
  int failed = 0;
  int total = 0;

  void check(bool cond, const std::string &name)
  {
    ++total;
    if (!cond)
    {
      ++failed;
      std::cout << "FAIL: " << name << "\n";
    }
  }
}

int main()
{
  using namespace krivoshapov;

  {
    Array<int> a;
    a.push_back(3);
    a.push_back(1);
    a.push_back(2);
    a.sort([](const int &x, const int &y)
           { return x < y; });
    check(a.size() == 3 && a[0] == 1 && a[2] == 3, "array.sort");
    Array<int> b = a;
    check(b[1] == 2, "array.copy");
  }

  {
    unsigned char d[20];
    sha1(reinterpret_cast<const unsigned char *>("abc"), 3, d);
    check(d[0] == 0xA9 && d[19] == 0x9D, "sha1.abc");
  }

  {
    HashTable<std::string, int, StringHash, StringEqual> h(4);
    h.add("x", 1);
    h.add("y", 2);
    check(h.has("x") && h.at("y") == 2, "ht.add");
    h.add("x", 9);
    check(h.at("x") == 9, "ht.overwrite");
    int v = h.drop("x");
    check(v == 9 && !h.has("x") && h.has("y"), "ht.drop.tomb");
    h.add("x", 7);
    check(h.at("x") == 7, "ht.reuse.tomb");
    h.rehash(64);
    check(h.has("x") && h.has("y") && h.size() == 2, "ht.rehash");
  }

  {
    HashTable<std::string, int, StringHash, StringEqual> h(2);
    bool thrown = false;
    try
    {
      h.add("a", 1);
      h.add("b", 2);
      h.add("c", 3);
    }
    catch (const std::length_error &)
    {
      thrown = true;
    }
    check(thrown, "ht.full.throws");
  }

  {
    Graph g;
    g.addEdge("a", "b", 40);
    g.addEdge("c", "a", 30);
    g.addEdge("c", "b", 20);
    Graph::Adjacency o = g.outbound("c");
    check(o.size() == 2 && o[0].first == "a"
          && o[0].second[0] == 30 && o[1].first == "b",
          "graph.outbound");
    Graph::Adjacency in = g.inbound("b");
    check(in.size() == 2 && in[0].first == "a" && in[1].first == "c",
          "graph.inbound");
    check(g.cutEdge("c", "b", 20) && !g.cutEdge("c", "b", 99),
          "graph.cut");
    Graph::NameList vs = g.vertices();
    check(vs.size() == 3 && vs[0] == "a", "graph.vertices.sorted");
  }

  if (failed == 0)
  {
    std::cout << "ALL " << total << " TESTS PASSED\n";
  }
  else
  {
    std::cout << failed << "/" << total << " FAILED\n";
  }
  return failed == 0 ? 0 : 1;
}
