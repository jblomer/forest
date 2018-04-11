#ifndef TTREEMODEL_H_
#define TTREEMODEL_H_

#include <memory>
#include <string_view>

class TTreeModel {
public:
   /**
    * Convenience wrapper around MakeBranch->GetSprout
    */
   template <typename T, typename... ArgsT>
   std::shared_ptr<T> Branch(std::string_view name, ArgsT&&... args) {

   }
};

#endif  // TTREEMODEL_H_