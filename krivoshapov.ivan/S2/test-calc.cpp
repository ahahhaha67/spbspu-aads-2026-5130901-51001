#include "calc.hpp"
#include <cassert>

namespace
{
  void test_stack()
  {
    krivoshapov::Stack<int> s;
    assert(s.empty());
    assert(s.size() == 0);

    s.push(1);
    s.push(2);
    s.push(3);
    assert(s.size() == 3);
    assert(s.top() == 3);

    s.pop();
    assert(s.top() == 2);
    assert(s.size() == 2);

    krivoshapov::Stack<int> c(s);
    c.push(9);
    assert(c.size() == 3 && s.size() == 2);

    krivoshapov::Stack<int> m(std::move(c));
    assert(m.size() == 3 && c.empty());

    krivoshapov::Stack<int> a;
    a = s;
    assert(a.size() == s.size());

    krivoshapov::Stack<int> b;
    b = std::move(m);
    assert(b.size() == 3 && m.empty());

    b.clear();
    assert(b.empty());

    bool threw = false;
    try
    {
      krivoshapov::Stack<int> e;
      e.pop();
    }
    catch (const std::out_of_range &)
    {
      threw = true;
    }
    assert(threw);

    threw = false;
    try
    {
      krivoshapov::Stack<int> e;
      e.top();
    }
    catch (const std::out_of_range &)
    {
      threw = true;
    }
    assert(threw);
  }

  void test_queue()
  {
    krivoshapov::Queue<int> q;
    assert(q.empty());

    for (int i = 0; i < 20; ++i)
    {
      q.push(i);
    }
    assert(q.size() == 20);

    for (int i = 0; i < 10; ++i)
    {
      assert(q.front() == i);
      q.pop();
    }
    assert(q.front() == 10 && q.size() == 10);

    krivoshapov::Queue<int> c(q);
    assert(c.front() == 10 && c.size() == 10);
    c.pop();
    assert(q.front() == 10);

    krivoshapov::Queue<int> m(std::move(c));
    assert(m.front() == 11 && c.empty());

    krivoshapov::Queue<int> a;
    a = q;
    assert(a.size() == q.size());

    krivoshapov::Queue<int> b;
    b = std::move(m);
    assert(!b.empty() && m.empty());

    b.clear();
    assert(b.empty());

    bool threw = false;
    try
    {
      krivoshapov::Queue<int> e;
      e.pop();
    }
    catch (const std::out_of_range &)
    {
      threw = true;
    }
    assert(threw);

    threw = false;
    try
    {
      krivoshapov::Queue<int> e;
      e.front();
    }
    catch (const std::out_of_range &)
    {
      threw = true;
    }
    assert(threw);
  }

  void ok(const std::string &e, size_t v)
  {
    assert(krivoshapov::evaluate(e) == v);
  }

  void bad(const std::string &e)
  {
    bool threw = false;
    try
    {
      krivoshapov::evaluate(e);
    }
    catch (const std::invalid_argument &)
    {
      threw = true;
    }
    assert(threw);
  }

  void test_eval()
  {
    ok("1 + 3", 4);
    ok("4 * 7 - 3", 25);
    ok("( 10 / ( 2 + 3 ) % 4 )", 2);
    ok("5 | 2 + 1", 7);
    ok("1 + 2 | 4", 7);
    ok("6 | 1 * 2", 6);
    ok("( ( 7 | 8 ) + 1 ) * 2", 32);
    ok("42", 42);
    ok("0 | 0", 0);

    bad("3 - 4");
    bad("5 / 0");
    bad("5 % 0");
    bad("2 + x");
    bad("1 2 +");
    bad("1 + + 2");
    bad("( )");
    bad("1 +");
    bad("( 1 + 2");
    bad("1 + 2 )");

    assert(krivoshapov::is_blank(""));
    assert(krivoshapov::is_blank("   \t\r"));
    assert(!krivoshapov::is_blank(" 1 "));
    assert(!krivoshapov::is_blank("0"));
  }
}

int main()
{
  test_stack();
  test_queue();
  test_eval();
  return 0;
}
