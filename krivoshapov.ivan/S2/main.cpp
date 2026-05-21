#include "calc.hpp"
#include <iostream>
#include <fstream>
#include <exception>
#include <climits>

namespace krivoshapov
{
  namespace
  {
    std::string num_to_str(long long v)
    {
      if (v == 0)
      {
        return "0";
      }
      if (v == LLONG_MIN)
      {
        return "-9223372036854775808";
      }
      bool neg = v < 0;
      if (neg)
      {
        v = -v;
      }
      std::string s;
      while (v != 0)
      {
        s += static_cast<char>('0' + static_cast<char>(v % 10));
        v /= 10;
      }
      if (neg)
      {
        s += '-';
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
      Stack<long long> results;
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

int main(int argc, char **argv)
{
  try
  {
    if (argc > 1)
    {
      std::ifstream file(argv[1]);
      if (!file.is_open())
      {
        std::cerr << "cannot open file\n";
        return 1;
      }
      return krivoshapov::run(file);
    }
    return krivoshapov::run(std::cin);
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << "\n";
    return 1;
  }
}
