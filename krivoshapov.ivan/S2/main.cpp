#include "calc.cpp"
#include <iostream>
#include <fstream>
#include <exception>

namespace krivoshapov
{
  namespace
  {
    std::string num_to_str(size_t v)
    {
      if (v == 0)
      {
        return "0";
      }
      std::string s;
      while (v != 0)
      {
        s += static_cast<char>('0' + static_cast<char>(v % 10));
        v /= 10;
      }
      std::string r;
      for (size_t i = s.size(); i > 0; --i)
      {
        r += s[i - 1];
      }
      return r;
    }

    int run(std::istream &in)
    {
      Stack<size_t> results;
      std::string line;
      while (std::getline(in, line))
      {
        if (is_blank(line))
        {
          continue;
        }
        results.push(evaluate(line));
      }
      std::string out;
      bool first = true;
      while (!results.empty())
      {
        if (!first)
        {
          out += " ";
        }
        out += num_to_str(results.top());
        results.pop();
        first = false;
      }
      std::cout << out << "\n";
      return 0;
    }
  }
}
