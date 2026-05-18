#include <iostream>
#include <string>
#include "dynamic_array.hpp"
#include "hash_table.hpp"
#include "sha1.hpp"

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
    int v = h.drop("x");
    check(v == 1 && !h.has("x") && h.has("y"), "ht.drop.tomb");
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
