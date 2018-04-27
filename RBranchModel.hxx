#ifndef RBRANCHMODEL_H_
#define RBRANCHMODEL_H_

#include <cstddef>
#include <string>
#include <string_view>

namespace Toy {

enum class RBranchType {
  kFloat,
  kInt,
  kBlob
};

class RBranchModel {
   std::string fName;
   RBranchType fType;

public:
   enum class Type {
      kFloat,
      kInt,
      kBlob
   };

   RBranchModel(std::string_view name, RBranchType type)
     : fName(name), fType(type) { }
   std::string GetName() { return fName; }

   std::size_t GetElementSize();

   template <typename T>
   static RBranchType MapType();
};  // RBranchModel

}  // namespace Toy

#endif  // RBRANCHMODEL_H_