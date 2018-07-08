#ifndef RCOLUMN_POINTER_H_
#define RCOLUMN_POINTER_H_

#include <cstdint>

namespace Toy {

class RColumnPointer {
private:
  std::uint64_t fIndex;

public:
  static const std::uint64_t kInvalidIndex = std::uint64_t(-1);
  explicit RColumnPointer(std::uint64_t index)
    : fIndex(index)
  { }

  bool operator ==(const RColumnPointer &other) const {
    return fIndex == other.fIndex;
  }

  bool operator !=(const RColumnPointer &other) const {
    return fIndex != other.fIndex;
  }

  std::uint64_t GetIndex() const { return fIndex; }
};  // RColumnPointer

}  // namespace Toy

#endif  // RCOLUMN_POINTER_H_
