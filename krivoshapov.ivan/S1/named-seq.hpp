#ifndef KRIVOSHAPOV_NAMED_SEQ_HPP
#define KRIVOSHAPOV_NAMED_SEQ_HPP

#include <string>
#include "../common/list.hpp"

namespace krivoshapov {

struct NamedSeq {
  std::string name;
  List< size_t > nums;

  explicit NamedSeq(const std::string & n):
    name(n),
    nums()
  {}
};

}

#endif
