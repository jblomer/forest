#ifndef RTREEENTRY_H_
#define RTREEENTRY_H_

#include <memory>
#include <vector>
#include <utility>

#include "RCargo.hxx"

namespace Toy {

class RTreeModel;

class RTreeEntry {
   friend class RTree;
   friend class RTreeModel;

   RTreeModel *fModel;
   CargoCollection fCargo;

   bool HasCompatibleModelId(RTreeModel *model);

   CargoCollection& GetCargoRefs() { return fCargo; }

public:
   RTreeEntry(RTreeModel *model) : fModel(model) { }

   template <typename T, typename... ArgsT>
   std::shared_ptr<T> AddCargo(ArgsT&&... args) {
     auto cargo = std::make_shared<RCargo<T>>(std::forward<ArgsT>(args)...);
     auto value_ptr = cargo->Get();
     fCargo.emplace_back(std::move(cargo));
     return value_ptr;
   }

   template <typename T, typename... ArgsT>
   void AddCargoCaptured(ArgsT&&... args) {
     auto cargo =
       std::make_shared<RCargoCaptured<T>>(std::forward<ArgsT>(args)...);
     fCargo.emplace_back(std::move(cargo));
   }

   void AddCargoSubtree(std::shared_ptr<RCargoSubtree> cargo) {
     fCargo.emplace_back(cargo);
   }

   bool IsCompatibleWith(RTreeModel *model) {
     return (model == fModel) || HasCompatibleModelId(model);
   }
};

}  // namespace Toy

#endif  // TTREEENTYR_H_