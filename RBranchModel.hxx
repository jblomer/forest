#ifndef RBRANCHMODEL_H_
#define RBRANCHMODEL_H_

#include <string>
#include <string_view>
#include <vector>

#include "RTreeColumnModel.hxx"

namespace Toy {

class RBranchModelBase {
  RBranchModelBase *fParent;
  std::vector<RBranchModelBase *> fChildren; /* TODO unique_ptr */
  bool fIsSimple;

protected:
   std::string fDescription;
   std::string fName;

   explicit RBranchModelBase(std::string_view name)
     : fParent(nullptr)
     , fIsSimple(false)
     , fName(name)
   { }

public:
   virtual ~RBranchModelBase() { }

   void Attach(RBranchModelBase *child) {
     child->fParent = this;
     fChildren.push_back(child);
   }

  virtual RTreeColumnModel* GenerateColumnModel() = 0;
};


class RBranchRoot {};

template <typename T>
class RBranchModel : public RBranchModelBase {
};

template <>
class RBranchModel<RBranchRoot> : public RBranchModelBase {
public:
  RBranchModel() : RBranchModelBase("") { }
  virtual RTreeColumnModel* GenerateColumnModel() override {
    return nullptr;
  }
};

template <>
class RBranchModel<float> : public RBranchModelBase {
public:
  explicit RBranchModel(std::string_view name) : RBranchModelBase(name) { }
  virtual RTreeColumnModel* GenerateColumnModel() override {
    RTreeColumnModel *model =
      new RTreeColumnModel(fName, fDescription, RTreeColumnType::kFloat, false);
    return model;
  }
};

}

#endif