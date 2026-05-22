#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include "commands.hpp"

namespace
{
  using Tree = krivoshapov::BSTree<int, std::string, krivoshapov::IntLess>;

  void testBasic()
  {
    Tree t;
    assert(t.empty());
    t.push(5, "five");
    t.push(3, "three");
    t.push(7, "seven");
    assert(!t.empty());
    assert(t.has(5) && t.has(3) && t.has(7) && !t.has(4));
    assert(t.get(5) == "five");
    t.push(5, "FIVE");
    assert(t.get(5) == "FIVE");
  }

  void testIterator()
  {
    Tree t;
    t.push(5, "");
    t.push(3, "");
    t.push(7, "");
    t.push(1, "");
    t.push(4, "");
    std::string keys;
    for (auto it = t.cbegin(); it != t.cend(); ++it)
    {
      keys += std::to_string(it.key()) + " ";
    }
    assert(keys == "1 3 4 5 7 ");
    Tree empty;
    assert(empty.begin() == empty.end());
  }

  void testDrop()
  {
    Tree t;
    t.push(5, "five");
    t.push(3, "three");
    t.push(7, "seven");
    assert(t.drop(3) == "three");
    assert(!t.has(3));
    t.push(6, "six");
    t.push(8, "eight");
    t.drop(7);
    assert(!t.has(7) && t.has(6) && t.has(8));
    bool threw = false;
    try
    {
      t.drop(99);
    }
    catch (const std::out_of_range &)
    {
      threw = true;
    }
    assert(threw);
  }

  void testCopy()
  {
    Tree t;
    t.push(5, "five");
    t.push(3, "three");
    Tree t2(t);
    assert(t2.has(5) && t2.has(3));
    t2.push(7, "seven");
    assert(!t.has(7));
    Tree t3;
    t3 = t;
    t3.push(9, "nine");
    assert(!t.has(9));
  }

  void testMove()
  {
    Tree t;
    t.push(1, "one");
    t.push(2, "two");
    Tree t2(std::move(t));
    assert(t2.has(1) && t2.has(2) && t.empty());
    Tree t3;
    t3 = std::move(t2);
    assert(t3.has(1) && t3.has(2) && t2.empty());
  }

  void testHeight()
  {
    Tree t;
    assert(t.height() == 0);
    t.push(5, "");
    assert(t.height() == 1);
    t.push(3, "");
    assert(t.height() == 2);
    t.push(7, "");
    assert(t.height() == 2);
    t.push(1, "");
    assert(t.height() == 3);
  }

  void testRotateLeft()
  {
    Tree t;
    t.push(5, "five");
    t.push(7, "seven");
    auto it = t.cbegin();
    ++it;
    assert(it.key() == 7);
    t.rotateLeft(it);
    std::string keys;
    for (auto i = t.cbegin(); i != t.cend(); ++i)
    {
      keys += std::to_string(i.key()) + " ";
    }
    assert(keys == "5 7 ");
    assert(t.height() == 2);
  }

  void testRotateRight()
  {
    Tree t;
    t.push(5, "five");
    t.push(3, "three");
    auto it = t.cbegin();
    assert(it.key() == 3);
    t.rotateRight(it);
    std::string keys;
    for (auto i = t.cbegin(); i != t.cend(); ++i)
    {
      keys += std::to_string(i.key()) + " ";
    }
    assert(keys == "3 5 ");
  }

  static krivoshapov::DatasetStore makeStore()
  {
    std::istringstream data(
        "first 1 name 2 surname\n"
        "second 4 mouse 1 name 2 keyboard\n");
    krivoshapov::DatasetStore store(16);
    krivoshapov::loadDatasets(data, store);
    return store;
  }

  void testPrint()
  {
    auto store = makeStore();
    std::istringstream cmds("print first\nprint second\n");
    std::ostringstream out;
    krivoshapov::runCommands(cmds, out, store);
    assert(out.str().find("first 1 name 2 surname") != std::string::npos);
    assert(out.str().find("second 1 name 2 keyboard 4 mouse") != std::string::npos);
  }

  void testComplement()
  {
    auto store = makeStore();
    std::istringstream cmds("complement third second first\nprint third\n");
    std::ostringstream out;
    krivoshapov::runCommands(cmds, out, store);
    assert(out.str().find("third 4 mouse") != std::string::npos);
  }

  void testIntersect()
  {
    auto store = makeStore();
    std::istringstream cmds("intersect fourth first second\nprint fourth\n");
    std::ostringstream out;
    krivoshapov::runCommands(cmds, out, store);
    assert(out.str().find("fourth 1 name 2 surname") != std::string::npos);
  }

  void testUnion()
  {
    auto store = makeStore();
    std::istringstream cmds("union fifth first second\nprint fifth\n");
    std::ostringstream out;
    krivoshapov::runCommands(cmds, out, store);
    assert(out.str().find("fifth 1 name 2 surname 4 mouse") != std::string::npos);
  }

  void testInvalid()
  {
    auto store = makeStore();
    std::istringstream cmds(
        "print nosuch\n"
        "complement first first second\n"
        "unknown arg\n");
    std::ostringstream out;
    krivoshapov::runCommands(cmds, out, store);
    assert(out.str().find("<INVALID COMMAND>") != std::string::npos);
  }
}

int main()
{
  testBasic();
  testIterator();
  testDrop();
  testCopy();
  testMove();
  testHeight();
  testRotateLeft();
  testRotateRight();
  testPrint();
  testComplement();
  testIntersect();
  testUnion();
  testInvalid();
  std::cout << "ALL TESTS PASSED\n";
  return 0;
}
