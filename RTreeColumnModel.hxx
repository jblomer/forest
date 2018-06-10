#ifndef RTREECOLUMNTYPE_H_
#define RTREECOLUMNTYPE_H_

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace Toy {

enum class RTreeColumnType {
  kUnknown,
  kFloat,
  kInt,
  kBit,
  kByte,
  kOffset,
};

class RTreeColumnModel {
  std::string fName;
  std::string fBranchProvenance;
  RTreeColumnType fType;
  bool fIsSorted;
  RTreeColumnModel* fParent;
  std::vector<RTreeColumnModel* /*TODO: uniqueptr*/> fChildren;
  std::size_t fElementSize;

public:
  RTreeColumnModel()
    : fName()
    , fBranchProvenance()
    , fType(RTreeColumnType::kUnknown)
    , fIsSorted(false)
    , fParent(nullptr)
    , fChildren()
    , fElementSize(0)
  {
  }

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
  {
    switch (fType) {
      case RTreeColumnType::kFloat:
        fElementSize = sizeof(float);
        break;
      case RTreeColumnType::kOffset:
        fElementSize = sizeof(unsigned);
        break;
      default:
        fElementSize = 0;
    }
  }

  RTreeColumnType GetType() const { return fType; }
  std::string GetName() const { return fName; }
  std::size_t GetElementSize() const { return fElementSize; }

  void Attach(RTreeColumnModel *model) {
    model->fParent = this;
    fChildren.push_back(model);
  }
};

}

std::ostream& operator<<(std::ostream& out, const Toy::RTreeColumnType value);

#endif // RTREECOLUMNTYPE_H_
