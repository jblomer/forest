#ifndef RCOLUMN_RANGE_H_
#define RCOLUMN_RANGE_H_

#include <cstdint>

#include "iterator_tpl.h"

#include "RColumnPointer.hxx"

namespace Toy {

struct RColumnRange {
  explicit RColumnRange(std::uint64_t max) : fMax(max) { }
  std::uint64_t fMax;

  struct ColumnIterator {
    std::uint64_t pos;
    inline void next(const RColumnRange* ref) {
      pos++;
    }
    inline void begin(const RColumnRange* ref) {
      pos = 0;
    }
    inline void end(const RColumnRange* ref) {
      pos = ref->fMax;
    }
    inline RColumnPointer get(RColumnRange* ref) {
      return RColumnPointer(pos);
    }
    inline const RColumnPointer get(const RColumnRange* ref)
    {
      return RColumnPointer(pos);
    }
    inline bool cmp(const ColumnIterator& s) const {
      return (pos != s.pos);
    }
  };
  SETUP_ITERATORS(RColumnRange, RColumnPointer, ColumnIterator);
};  // RColumnRange

}  // namespace Toy

#endif  // RENTRY_RANGE_H_
