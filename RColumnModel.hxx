#ifndef RCOLUMNMODEL_H_
#define RCOLUMNMODEL_H_

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace Toy {

enum class RColumnType {
  kUnknown,
  kFloat,
  kInt,
  kBit,
  kByte,
  kOffset,
};

class RColumnModel {
  std::string fName;
  std::string fBranchProvenance;
  RColumnType fType;
  bool fIsSorted;
  RColumnModel* fParent;
  std::vector<RColumnModel* /*TODO: uniqueptr*/> fChildren;
  std::size_t fElementSize;

public:
  RColumnModel()
    : fName()
    , fBranchProvenance()
    , fType(RColumnType::kUnknown)
    , fIsSorted(false)
    , fParent(nullptr)
    , fChildren()
    , fElementSize(0)
  {
  }

  RColumnModel(std::string_view name,
               std::string_view provenance,
               RColumnType type,
               bool is_sorted)
    : fName(name)
    , fBranchProvenance(provenance)
    , fType(type)
    , fIsSorted(is_sorted)
    , fParent(nullptr)
    , fChildren()
  {
    switch (fType) {
      case RColumnType::kFloat:
        fElementSize = sizeof(float);
        break;
      case RColumnType::kOffset:
        fElementSize = sizeof(std::uint64_t);
        break;
      default:
        fElementSize = 0;
    }
  }

  RColumnType GetType() const { return fType; }
  std::string GetName() const { return fName; }
  std::size_t GetElementSize() const { return fElementSize; }

  void Attach(RColumnModel *model) {
    model->fParent = this;
    fChildren.push_back(model);
  }
};

}

std::ostream& operator<<(std::ostream& out, const Toy::RColumnType value);

#endif // RCOLUMNMODEL_H_
