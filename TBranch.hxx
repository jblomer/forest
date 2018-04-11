#ifndef TBRANCH_H_
#define TBRANCH_H_

#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace Toy {

template <typename T>
class TBranch {
   std::string fName;
   std::shared_ptr<T> fSprout;

public:
   explicit TBranch(std::string_view name) : fName(name) { }
   std::string GetName() { return fName; }

   template <typename... ArgsT>
   std::shared_ptr<T> MakeSprout(ArgsT&&... args) {
     fSprout = std::make_shared<T>(std::forward<ArgsT>(args)...);
     return fSprout;
   }

   std::shared_ptr<T> GetSprout() {
     return fSprout;
   }
};

}  // namespace Toy

#endif  // TBRANCH_H_
