#ifndef TBRANCH_H_
#define TBRANCH_H_

#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace Toy {

enum class TBranchType {
   kTypeFloat,
   kTypeInt,
   kTypeBlob,
};

/**
 * Static, type system of the tree
 */
class TBranch {
   std::string fName;
   TBranchType fType;

public:
   TBranch(std::string_view name, TBranchType type)
     : fName(name), fType(type) { }
   std::string GetName() { return fName; }

   template <typename T>
   static TBranchType MapType();
};

}  // namespace Toy


#endif  // TBRANCH_H_
