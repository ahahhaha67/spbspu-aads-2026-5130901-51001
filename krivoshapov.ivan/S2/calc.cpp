#include "Calc.hpp"

namespace krivoshapov
{
  namespace
  {
    bool is_op(const std::string &t)
    {
      return t == "+" || t == "-" || t == "*" || t == "/" || t == "%" || t == "|";
    }

    // * / % > + - > | (побитовое ИЛИ — самый низкий, как в C)
    int prio(const std::string &op)
    {
      if (op == "*" || op == "/" || op == "%")
      {
        return 3;
      }
      if (op == "+" || op == "-")
      {
        return 2;
      }
      return 1;
    }
  }

  bool is_blank(const std::string &line)
  {
    for (size_t i = 0; i < line.size(); ++i)
    {
      if (line[i] != ' ' && line[i] != '\t' && line[i] != '\r')
      {
        return false;
      }
    }
    return true;
  }

  size_t evaluate(const std::string & /*line*/)
  {
    return 0;
  }
}
