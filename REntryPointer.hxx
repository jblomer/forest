#ifndef RENTRY_POINTER_H_
#define RENTRY_POINTER_H_

#include <cstdint>

struct REntryPointer {
  static const std::uint64_t kInvalidEntry = std::uint64_t(-1);
  explicit REntryPointer(std::uint64_t entry_number)
    : fEntryNumber(entry_number)
  { }

  bool operator ==(const REntryPointer &other) const {
    return fEntryNumber == other.fEntryNumber;
  }

  bool operator !=(const REntryPointer &other) const {
    return fEntryNumber != other.fEntryNumber;
  }

  std::uint64_t fEntryNumber;
};

#endif  // RENTRY_POINTER_H
