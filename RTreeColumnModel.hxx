#ifndef RTREECOLUMNTYPE_H_
#define RTREECOLUMNTYPE_H_

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace Toy {

enum class RTreeColumnType {
  kRoot,
  kFloat,
  kInt,
  kCollection,
  kBitvector,
  kBlob
};

class RTreeColumnModel {
  std::string fName;
  std::string fBranchProvenance;
  RTreeColumnType fType;
  bool fIsSorted;
  RTreeColumnModel* fParent;
  std::vector<RTreeColumnModel* /*TODO: uniqueptr*/> fChildren;

public:
  RTreeColumnModel()
    : fName()
    , fBranchProvenance()
    , fType(RTreeColumnType::kRoot)
    , fIsSorted(false)
    , fParent(nullptr)
    , fChildren()
  { }

  RTreeColumnModel(std::string_view name,
                   std::string_view provenance,
                   RTreeColumnType type,
                   bool is_sorted)
    : fName(name)
    , fBranchProvenance(provenance)
    , fType(type)
    , fIsSorted(is_sorted)
    , fParent(nullptr)
    , fChildren()
  { }

  RTreeColumnType GetType() { return fType; }

  void Attach(RTreeColumnModel *model) {
    model->fParent = this;
    fChildren.push_back(model);
  }
};

}

#endif // RTREECOLUMNTYPE_H_
