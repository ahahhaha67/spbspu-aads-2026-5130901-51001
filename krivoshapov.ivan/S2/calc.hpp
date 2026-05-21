#ifndef KRIVOSHAPOV_CALC_HPP
#define KRIVOSHAPOV_CALC_HPP

#include <cstddef>
#include <string>
#include <stdexcept>
#include "stack.hpp"
#include "queue.hpp"

namespace krivoshapov
{
  long long evaluate(const std::string &line);

  bool is_blank(const std::string &line);
}

#endif
