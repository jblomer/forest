#ifndef RCOLUMN_RANGE_H_
#define RCOLUMN_RANGE_H_

#include <cstdint>

#include "iterator_tpl.h"

#include "RColumnPointer.hxx"

namespace Toy {

class RColumnRange {
private:
  std::uint64_t fBegin;
  std::uint64_t fEnd;

public:
  explicit RColumnRange(std::uint64_t b, std::uint64_t e)
    : fBegin(b), fEnd(e) { }

  std::uint64_t GetBegin() const { return fBegin; }
  std::uint64_t GetEnd() const { return fEnd; }
  std::uint64_t GetSize() const { return fEnd - fBegin; }

  struct ColumnIterator {
    std::uint64_t pos;
    inline void next(const RColumnRange* ref) {
      pos++;
    }
    inline void begin(const RColumnRange* ref) {
      pos = ref->fBegin;
    }
    inline void end(const RColumnRange* ref) {
      pos = ref->fEnd;
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
