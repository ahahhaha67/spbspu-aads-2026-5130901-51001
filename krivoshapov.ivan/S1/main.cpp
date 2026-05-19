#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

#include "../common/list.hpp"
#include "named-seq.hpp"

namespace krivoshapov {

static void readInput(List< NamedSeq > & seqs)
{
  std::string name;
  while (std::cin >> name && !std::cin.eof()) {
    NamedSeq seq(name);
    size_t num = 0;
    while (std::cin >> num) {
      seq.nums.pushBack(num);
    }
    seqs.pushBack(static_cast< NamedSeq && >(seq));
    std::cin.clear();
  }
}

static void printNames(const List< NamedSeq > & seqs)
{
  auto it = seqs.cbegin();
  std::cout << it->name;
  ++it;
  for (; it != seqs.cend(); ++it) {
    std::cout << ' ' << it->name;
  }
  std::cout << '\n';
}

static size_t checkedSum(size_t a, size_t b)
{
  if (std::numeric_limits< size_t >::max() - a < b) {
    throw std::overflow_error("overflow when calculating row sums");
  }
  return a + b;
}

static bool collectRow(List< LIter< size_t > > & iters,
    List< NamedSeq > & seqs,
    List< size_t > & rowVals)
{
  auto iterIt = iters.begin();
  auto seqIt = seqs.begin();
  while (seqIt != seqs.end()) {
    if (*iterIt != seqIt->nums.end()) {
      rowVals.pushBack(**iterIt);
      ++(*iterIt);
    }
    ++seqIt;
    ++iterIt;
  }
  return !rowVals.empty();
}

static void processZip(List< NamedSeq > & seqs, List< size_t > & rowSums)
{
  List< LIter< size_t > > iters;
  for (auto it = seqs.begin(); it != seqs.end(); ++it) {
    iters.pushBack(it->nums.begin());
  }

  List< size_t > rowVals;
  while (collectRow(iters, seqs, rowVals)) {
    auto it = rowVals.cbegin();
    std::cout << *it;
    ++it;
    for (; it != rowVals.cend(); ++it) {
      std::cout << ' ' << *it;
    }
    std::cout << '\n';

    size_t rowSum = 0;
    for (auto vit = rowVals.cbegin(); vit != rowVals.cend(); ++vit) {
      rowSum = checkedSum(rowSum, *vit);
    }
    rowSums.pushBack(rowSum);
    rowVals.clear();
  }
}

static void printSums(const List< size_t > & sums)
{
  auto it = sums.cbegin();
  std::cout << *it;
  ++it;
  for (; it != sums.cend(); ++it) {
    std::cout << ' ' << *it;
  }
  std::cout << '\n';
}

}

int main()
{
  krivoshapov::List< krivoshapov::NamedSeq > seqs;
  krivoshapov::readInput(seqs);

  if (seqs.empty()) {
    std::cout << 0 << '\n';
    return 0;
  }

  krivoshapov::printNames(seqs);

  krivoshapov::List< size_t > rowSums;
  try {
    krivoshapov::processZip(seqs, rowSums);
  } catch (const std::exception & e) {
    std::cerr << e.what() << '\n';
    return 1;
  }

  if (rowSums.empty()) {
    std::cout << 0 << '\n';
    return 0;
  }

  krivoshapov::printSums(rowSums);
  return 0;
}
