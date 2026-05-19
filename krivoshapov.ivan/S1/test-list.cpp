#include <iostream>
#include <string>
#include <limits>

#include "../common/list.hpp"

namespace krivoshapov {

static int passed = 0;
static int failed = 0;

static void check(bool cond, const char * description)
{
  if (cond) {
    ++passed;
  } else {
    ++failed;
    std::cerr << "FAIL: " << description << '\n';
  }
}

static void testEmptyList()
{
  List< int > lst;
  check(lst.empty(),                "empty list: empty() == true");
  check(lst.size() == 0,            "empty list: size() == 0");
  check(lst.begin() == lst.end(),   "empty list: begin() == end()");
  check(lst.cbegin() == lst.cend(), "empty list: cbegin() == cend()");
}

static void testPushBackAndIterate()
{
  List< int > lst;
  lst.pushBack(1);
  lst.pushBack(2);
  lst.pushBack(3);

  check(!lst.empty(),     "pushBack: not empty");
  check(lst.size() == 3,  "pushBack: size == 3");
  check(lst.front() == 1, "pushBack: front == 1");
  check(lst.back()  == 3, "pushBack: back == 3");

  auto it = lst.begin();
  check(*it == 1, "iterate: 1st == 1"); ++it;
  check(*it == 2, "iterate: 2nd == 2"); ++it;
  check(*it == 3, "iterate: 3rd == 3"); ++it;
  check(it == lst.end(), "iterate: reaches end");
}

static void testPushFrontAndIterate()
{
  List< int > lst;
  lst.pushFront(3);
  lst.pushFront(2);
  lst.pushFront(1);

  check(lst.front() == 1, "pushFront: front == 1");
  check(lst.back()  == 3, "pushFront: back == 3");
  check(lst.size()  == 3, "pushFront: size == 3");

  auto it = lst.begin();
  check(*it == 1, "pushFront iterate: 1st == 1"); ++it;
  check(*it == 2, "pushFront iterate: 2nd == 2"); ++it;
  check(*it == 3, "pushFront iterate: 3rd == 3");
}

static void testPopFront()
{
  List< int > lst;
  lst.pushBack(10);
  lst.pushBack(20);
  lst.pushBack(30);

  lst.popFront();
  check(lst.size()  == 2,  "popFront: size == 2");
  check(lst.front() == 20, "popFront: new front == 20");

  lst.popFront();
  lst.popFront();
  check(lst.empty(),              "popFront: empty after all pops");
  check(lst.begin() == lst.end(), "popFront: begin == end when empty");
}

static void testPopBack()
{
  List< int > lst;
  lst.pushBack(10);
  lst.pushBack(20);
  lst.pushBack(30);

  lst.popBack();
  check(lst.size() == 2,  "popBack: size == 2");
  check(lst.back() == 20, "popBack: new back == 20");

  lst.popBack();
  lst.popBack();
  check(lst.empty(), "popBack: empty after all pops");
}

static void testBidirectionalIteration()
{
  List< int > lst;
  lst.pushBack(1);
  lst.pushBack(2);
  lst.pushBack(3);

  auto it = lst.begin();
  ++it; ++it;
  check(*it == 3, "bidir: reached 3rd element");
  --it;
  check(*it == 2, "bidir: step back to 2");
  --it;
  check(*it == 1, "bidir: step back to 1");
  check(it == lst.begin(), "bidir: back at begin");
}

static void testPostfixOperators()
{
  List< int > lst;
  lst.pushBack(7);
  lst.pushBack(8);

  auto it   = lst.begin();
  auto prev = it++;
  check(*prev == 7, "postfix++: old value == 7");
  check(*it   == 8, "postfix++: new position == 8");

  auto prev2 = it--;
  check(*prev2 == 8, "postfix--: old value == 8");
  check(*it    == 7, "postfix--: new position == 7");
}

static void testConstIterator()
{
  List< int > lst;
  lst.pushBack(5);
  lst.pushBack(6);

  const List< int > & cref = lst;
  auto it = cref.cbegin();
  check(*it == 5, "const iter: 1st == 5"); ++it;
  check(*it == 6, "const iter: 2nd == 6"); ++it;
  check(it == cref.cend(), "const iter: reaches cend");

  LCIter< int > ci = lst.begin();
  check(*ci == 5, "LIter->LCIter: value == 5");
}

static void testCopyConstructor()
{
  List< int > a;
  a.pushBack(1);
  a.pushBack(2);
  a.pushBack(3);

  List< int > b(a);
  check(b.size()  == 3, "copy ctor: size == 3");
  check(b.front() == 1, "copy ctor: front == 1");
  check(b.back()  == 3, "copy ctor: back == 3");

  b.popFront();
  check(b.size() == 2, "copy ctor: b shrinks independently");
  check(a.size() == 3, "copy ctor: a unchanged");
}

static void testCopyAssignment()
{
  List< int > a;
  a.pushBack(10);
  a.pushBack(20);

  List< int > b;
  b.pushBack(99);
  b = a;

  check(b.size()  == 2,  "copy assign: size == 2");
  check(b.front() == 10, "copy assign: front == 10");
  check(b.back()  == 20, "copy assign: back == 20");

  b = b;
  check(b.size() == 2, "copy assign self: size unchanged");
}

static void testMoveConstructor()
{
  List< int > a;
  a.pushBack(1);
  a.pushBack(2);

  List< int > b(static_cast< List< int > && >(a));
  check(b.size()  == 2, "move ctor: b.size == 2");
  check(b.front() == 1, "move ctor: b.front == 1");
  check(a.empty(),      "move ctor: a empty after move");
}

static void testMoveAssignment()
{
  List< int > a;
  a.pushBack(3);
  a.pushBack(4);

  List< int > b;
  b = static_cast< List< int > && >(a);
  check(b.size()  == 2, "move assign: b.size == 2");
  check(b.front() == 3, "move assign: b.front == 3");
  check(a.empty(),      "move assign: a empty after move");
}

static void testInsertMiddle()
{
  List< int > lst;
  lst.pushBack(1);
  lst.pushBack(3);

  auto it = lst.begin();
  ++it;
  lst.insert(it, 2);

  check(lst.size() == 3, "insert middle: size == 3");
  auto cur = lst.begin();
  check(*cur == 1, "insert middle: 1st == 1"); ++cur;
  check(*cur == 2, "insert middle: 2nd == 2"); ++cur;
  check(*cur == 3, "insert middle: 3rd == 3");
}

static void testInsertAtBegin()
{
  List< int > lst;
  lst.pushBack(2);
  lst.insert(lst.begin(), 1);
  check(lst.front() == 1, "insert begin: front == 1");
  check(lst.size()  == 2, "insert begin: size == 2");
}

static void testInsertAtEnd()
{
  List< int > lst;
  lst.pushBack(1);
  lst.insert(lst.end(), 2);
  check(lst.back()  == 2, "insert end: back == 2");
  check(lst.size()  == 2, "insert end: size == 2");
}

static void testClear()
{
  List< int > lst;
  lst.pushBack(1);
  lst.pushBack(2);
  lst.clear();

  check(lst.empty(),              "clear: empty after clear");
  check(lst.size() == 0,          "clear: size == 0");
  check(lst.begin() == lst.end(), "clear: begin == end");

  lst.pushBack(42);
  check(lst.size()  == 1,  "clear: reuse after clear size");
  check(lst.front() == 42, "clear: reuse after clear value");
}

static void testSingleElement()
{
  List< int > lst;
  lst.pushBack(99);
  check(lst.size()  == 1,  "single: size == 1");
  check(lst.front() == 99, "single: front == 99");
  check(lst.back()  == 99, "single: back == 99");

  lst.popFront();
  check(lst.empty(), "single: empty after popFront");

  lst.pushBack(99);
  lst.popBack();
  check(lst.empty(), "single: empty after popBack");
}

static void testStringList()
{
  List< std::string > lst;
  lst.pushBack("hello");
  lst.pushBack("world");
  check(lst.front() == "hello", "string list: front == hello");
  check(lst.back()  == "world", "string list: back == world");
  check(lst.size()  == 2,       "string list: size == 2");
}

}

int main()
{
  krivoshapov::testEmptyList();
  krivoshapov::testPushBackAndIterate();
  krivoshapov::testPushFrontAndIterate();
  krivoshapov::testPopFront();
  krivoshapov::testPopBack();
  krivoshapov::testBidirectionalIteration();
  krivoshapov::testPostfixOperators();
  krivoshapov::testConstIterator();
  krivoshapov::testCopyConstructor();
  krivoshapov::testCopyAssignment();
  krivoshapov::testMoveConstructor();
  krivoshapov::testMoveAssignment();
  krivoshapov::testInsertMiddle();
  krivoshapov::testInsertAtBegin();
  krivoshapov::testInsertAtEnd();
  krivoshapov::testClear();
  krivoshapov::testSingleElement();
  krivoshapov::testStringList();

  const int total = krivoshapov::passed + krivoshapov::failed;
  if (krivoshapov::failed == 0) {
    std::cout << "All " << total << " tests passed.\n";
    return 0;
  }
  std::cout << krivoshapov::failed << " / " << total << " tests FAILED.\n";
  return 1;
}
