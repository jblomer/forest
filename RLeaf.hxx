#ifndef RLEAF_H_
#define RLEAF_H_

#include <cassert>
#include <cstddef>
#include <cstring>
#include <memory>
#include <string>
#include <utility>

#include "RBranchModel.hxx"

namespace Toy {

class RLeafBase {
protected:
   // We can indicate that on disk format == in memory format
   // Improve
   bool fIsMovable;
   void *fRawContent;

   bool fIsFixedSize;
   unsigned fSize;

   Toy::RBranchType fBranchType;

   virtual void DoSerialize(void *destination) { assert(false); }
   virtual std::size_t DoGetSize() { assert(false); }
public:
   RLeafBase()
     : fIsMovable(false)
     , fRawContent(nullptr)
     , fIsFixedSize(false)
     , fSize(0)
     , fBranchType(RBranchType::kBlob) { }
   virtual ~RLeafBase() { }

   RBranchType GetBranchType() { return fBranchType; }

   void Serialize(void *destination) {
     if (!fIsMovable) {
       DoSerialize(destination);
       return;
     }
     std::memcpy(destination, fRawContent, fSize);
   }

   std::size_t GetSize() {
     if (!fIsFixedSize) {
        return DoGetSize();
     }
     return fSize;
   }
};


template <typename T>
class RLeaf : public RLeafBase {
   std::shared_ptr<T> fValue;

   void Initialize();

public:
   template<typename... ArgsT>
   RLeaf(ArgsT&&... args)
     : fValue(std::make_shared<T>(std::forward<ArgsT>(args)...))
   {
     Initialize();
     fBranchType = MapType();
   }

   std::shared_ptr<T> Get() { return fValue; }

   static RBranchType MapType();
   static std::string GetMemoryType();
};

}  // namespace Toy

#endif  // RLEAF_H_
