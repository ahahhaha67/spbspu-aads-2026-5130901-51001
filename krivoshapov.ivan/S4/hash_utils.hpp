#ifndef KRIVOSHAPOV_HASH_UTILS_HPP
#define KRIVOSHAPOV_HASH_UTILS_HPP

#include <cstddef>
#include <string>

namespace krivoshapov
{
  struct StringHash
  {
    size_t operator()(const std::string &s) const
    {
      size_t h = 5381;
      for (size_t i = 0; i < s.size(); ++i)
      {
        h = h * 33 + static_cast<size_t>(static_cast<unsigned char>(s[i]));
      }
      return h;
    }
  };

  struct StringEqual
  {
    bool operator()(const std::string &a, const std::string &b) const
    {
      return a == b;
    }
  };
}

#endif
