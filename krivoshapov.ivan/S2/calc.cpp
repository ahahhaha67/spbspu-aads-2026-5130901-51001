#include "calc.hpp"
#include <climits>

namespace krivoshapov
{
  namespace
  {
    bool is_op(const std::string &t)
    {
      return t == "+" || t == "-" || t == "*" || t == "/" || t == "%" || t == "|";
    }

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

    long long to_number(const std::string &t)
    {
      if (t.empty())
      {
        throw std::invalid_argument("operand is not a number");
      }
      long long value = 0;
      for (size_t i = 0; i < t.size(); ++i)
      {
        char c = t[i];
        if (c < '0' || c > '9')
        {
          throw std::invalid_argument("operand is not a number");
        }
        long long d = c - '0';
        if (value > (LLONG_MAX - d) / 10)
        {
          throw std::invalid_argument("number overflow");
        }
        value = value * 10 + d;
      }
      return value;
    }

    long long apply(const std::string &op, long long a, long long b)
    {
      if (op == "+")
      {
        if (b > 0 && a > LLONG_MAX - b)
        {
          throw std::invalid_argument("addition overflow");
        }
        if (b < 0 && a < LLONG_MIN - b)
        {
          throw std::invalid_argument("addition overflow");
        }
        return a + b;
      }
      if (op == "-")
      {
        if (b < 0 && a > LLONG_MAX + b)
        {
          throw std::invalid_argument("subtraction overflow");
        }
        if (b > 0 && a < LLONG_MIN + b)
        {
          throw std::invalid_argument("subtraction underflow");
        }
        return a - b;
      }
      if (op == "*")
      {
        if (a == 0 || b == 0)
        {
          return 0;
        }
        if (a == LLONG_MIN || b == LLONG_MIN)
        {
          if (a == LLONG_MIN && b == 1)
          {
            return a;
          }
          if (b == LLONG_MIN && a == 1)
          {
            return b;
          }
          throw std::invalid_argument("multiplication overflow");
        }
        bool neg = (a < 0) ^ (b < 0);
        long long abs_a = a < 0 ? -a : a;
        long long abs_b = b < 0 ? -b : b;
        if (abs_a > LLONG_MAX / abs_b)
        {
          throw std::invalid_argument("multiplication overflow");
        }
        long long abs_r = abs_a * abs_b;
        return neg ? -abs_r : abs_r;
      }
      if (op == "/")
      {
        if (b == 0)
        {
          throw std::invalid_argument("division by zero");
        }
        if (a == LLONG_MIN && b == -1)
        {
          throw std::invalid_argument("division overflow");
        }
        return a / b;
      }
      if (op == "%")
      {
        if (b == 0)
        {
          throw std::invalid_argument("modulo by zero");
        }
        if (a == LLONG_MIN && b == -1)
        {
          return 0;
        }
        long long r = a % b;
        if (r != 0 && (r < 0) != (b < 0))
        {
          r += b;
        }
        return r;
      }
      return a | b;
    }

    Queue<std::string> to_postfix(const std::string &line)
    {
      Queue<std::string> output;
      Stack<std::string> ops;
      bool expect_operand = true;

      size_t i = 0;
      while (i < line.size())
      {
        while (i < line.size() && (line[i] == ' ' || line[i] == '\t' || line[i] == '\r'))
        {
          ++i;
        }
        if (i >= line.size())
        {
          break;
        }
        size_t j = i;
        while (j < line.size() && line[j] != ' ' && line[j] != '\t' && line[j] != '\r')
        {
          ++j;
        }
        std::string tok = line.substr(i, j - i);
        i = j;

        if (tok == "(")
        {
          if (!expect_operand)
          {
            throw std::invalid_argument("invalid expression");
          }
          ops.push(tok);
        }
        else if (tok == ")")
        {
          if (expect_operand)
          {
            throw std::invalid_argument("invalid expression");
          }
          expect_operand = false;
          bool found = false;
          while (!ops.empty())
          {
            if (ops.top() == "(")
            {
              found = true;
              ops.pop();
              break;
            }
            output.push(ops.top());
            ops.pop();
          }
          if (!found)
          {
            throw std::invalid_argument("mismatched parentheses");
          }
        }
        else if (is_op(tok))
        {
          if (expect_operand)
          {
            throw std::invalid_argument("invalid expression");
          }
          expect_operand = true;
          while (!ops.empty() && ops.top() != "(" && prio(ops.top()) >= prio(tok))
          {
            output.push(ops.top());
            ops.pop();
          }
          ops.push(tok);
        }
        else
        {
          if (!expect_operand)
          {
            throw std::invalid_argument("invalid expression");
          }
          expect_operand = false;
          to_number(tok);
          output.push(tok);
        }
      }

      if (expect_operand)
      {
        throw std::invalid_argument("invalid expression");
      }

      while (!ops.empty())
      {
        if (ops.top() == "(")
        {
          throw std::invalid_argument("mismatched parentheses");
        }
        output.push(ops.top());
        ops.pop();
      }

      return output;
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

  long long evaluate(const std::string &line)
  {
    Queue<std::string> postfix = to_postfix(line);

    Stack<long long> calc;
    while (!postfix.empty())
    {
      std::string tok = postfix.front();
      postfix.pop();
      if (is_op(tok))
      {
        if (calc.size() < 2)
        {
          throw std::invalid_argument("invalid expression");
        }
        long long b = calc.top();
        calc.pop();
        long long a = calc.top();
        calc.pop();
        calc.push(apply(tok, a, b));
      }
      else
      {
        calc.push(to_number(tok));
      }
    }

    if (calc.size() != 1)
    {
      throw std::invalid_argument("invalid expression");
    }
    return calc.top();
  }
}
