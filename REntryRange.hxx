#ifndef RENTRY_RANGE_H_
#define RENTRY_RANGE_H_

#include "iterator_tpl.h"

#include "REntryPointer.hxx"
#include "RTree.hxx"

namespace Toy {

struct REntryRange {
  explicit REntryRange(RTree *tree) : fTree(tree) { }
  RTree *fTree;

  struct EntryIterator {
    unsigned pos;
    inline void next(const REntryRange* ref) {
      pos++;
    }
    inline void begin(const REntryRange* ref) {
      pos = 0;
    }
    inline void end(const REntryRange* ref) {
      pos = ref->fTree->GetNentries();
    }
    inline REntryPointer get(REntryRange* ref) {
      return REntryPointer(pos);
    }
    inline const REntryPointer get(const REntryRange* ref)
    {
      return REntryPointer(pos);
    }
    inline bool cmp(const EntryIterator& s) const {
      return (pos != s.pos);
    }
  };
  SETUP_ITERATORS(REntryRange, REntryPointer, EntryIterator);
};  // REntryRange

}  // namespace Toy

#endif  // RENTRY_RANGE_H_
