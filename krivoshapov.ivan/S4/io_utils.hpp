#ifndef KRIVOSHAPOV_IO_UTILS_HPP
#define KRIVOSHAPOV_IO_UTILS_HPP

#include <string>
#include <cstddef>
#include "dynamic_array.hpp"

namespace krivoshapov
{
  inline Array<std::string> split(const std::string &line)
  {
    Array<std::string> out;
    std::string cur;
    bool has = false;
    for (size_t i = 0; i < line.size(); ++i)
    {
      char c = line[i];
      if (c == ' ' || c == '\t' || c == '\r')
      {
        if (has)
        {
          out.push_back(cur);
          cur.clear();
          has = false;
        }
      }
      else
      {
        cur.push_back(c);
        has = true;
      }
    }
    if (has)
    {
      out.push_back(cur);
    }
    return out;
  }

  inline bool parseSizeT(const std::string &s, size_t &out)
  {
    if (s.empty())
    {
      return false;
    }
    size_t v = 0;
    for (size_t i = 0; i < s.size(); ++i)
    {
      char c = s[i];
      if (c < '0' || c > '9')
      {
        return false;
      }
      v = v * 10 + static_cast<size_t>(c - '0');
    }
    out = v;
    return true;
  }

  inline bool parseInt(const std::string &s, int &out)
  {
    if (s.empty())
    {
      return false;
    }
    bool neg = (s[0] == '-');
    size_t i = neg ? 1 : 0;
    if (i >= s.size())
    {
      return false;
    }
    long long v = 0;
    for (; i < s.size(); ++i)
    {
      char c = s[i];
      if (c < '0' || c > '9')
      {
        return false;
      }
      v = v * 10 + static_cast<long long>(c - '0');
      if (v > 2147483648LL)
      {
        return false;
      }
    }
    if (neg)
    {
      v = -v;
    }
    if (v < -2147483648LL || v > 2147483647LL)
    {
      return false;
    }
    out = static_cast<int>(v);
    return true;
  }
}

#endif
