#ifndef RTREE_H_
#define RTREE_H_

#include <memory>
#include <string_view>
#include <vector>

#include "iterator_tpl.h"

#include "RTreeColumn.hxx"
#include "RTreeModel.hxx"

namespace Toy {

class RTreeEntry;
class RTreeSource;

class RTree {
   std::unique_ptr<RTreeSink> fSink;
   std::unique_ptr<RTreeSource> fSource;
   std::shared_ptr<RTreeModel> fModel;
   RTreeColumnCollection fColumns;

   unsigned fNentries;

public:
  struct REntryCollection {
    explicit REntryCollection(RTree *tree) : fTree(tree) { }
    RTree *fTree;

    int GetView(std::string_view name) {
      return 0;
    }

    struct EntryPointer {
      explicit EntryPointer(std::uint64_t entry_number)
        : fEntryNumber(entry_number)
      { }
      std::uint64_t fEntryNumber;
    };

    struct EntryIterator {
      unsigned pos;
      inline void next(const REntryCollection* ref) {
        pos++;
      }
      inline void begin(const REntryCollection* ref) {
        pos = 0;
      }
      inline void end(const REntryCollection* ref) {
        pos = ref->fTree->GetNentries();
      }
      inline EntryPointer get(REntryCollection* ref) {
        return EntryPointer(pos);
      }
      inline const EntryPointer get(const REntryCollection* ref)
      {
        return EntryPointer(pos);
      }
      inline bool cmp(const EntryIterator& s) const {
        return (pos != s.pos);
      }
    };
    SETUP_ITERATORS(REntryCollection, EntryPointer, EntryIterator);
  };  // REntryCollection

  RTree(std::shared_ptr<RTreeModel> model,
        std::unique_ptr<RTreeSink> sink);
  RTree(std::shared_ptr<RTreeModel> model,
        std::unique_ptr<RTreeSource> source);
  ~RTree();

  unsigned GetNentries() const { return fNentries; }
  REntryCollection GetEntryCollection() {
    return REntryCollection(this);
  }

  void Fill() { Fill(&(fModel->fDefaultEntry)); }
  void Fill(RTreeEntry *entry);
  void FillV(RTreeEntry **entry, unsigned size);
};

}  // namespace Toy

#endif  // RTREE_H_
