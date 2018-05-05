#ifndef RTREEELEMENT_H_
#define RTREEELEMENT_H_

#include <cassert>
#include <cstddef>
#include <cstring>
#include <memory>
#include <string>
#include <utility>

#include "RTreeColumnModel.hxx"

namespace Toy {

class RTreeElementBase {
protected:
   // We can indicate that on disk format == in memory format
   bool fIsMovable;
   void *fRawContent;

   bool fIsFixedSize;
   unsigned fSize;

   RTreeColumnType fColumnType;

   virtual void DoSerialize(void *destination) const { assert(false); }
   virtual std::size_t DoGetSize() const { assert(false); }
public:
   RTreeElementBase()
     : fIsMovable(false)
     , fRawContent(nullptr)
     , fIsFixedSize(false)
     , fSize(0)
     , fColumnType(RTreeColumnType::kBlob) { }
   virtual ~RTreeElementBase() { }

   RTreeColumnType GetColumnType() const { return fColumnType; }

   void Serialize(void *destination) const {
     if (!fIsMovable) {
       DoSerialize(destination);
       return;
     }
     std::memcpy(destination, fRawContent, fSize);
   }

   std::size_t GetSize() const {
     if (!fIsFixedSize) {
        return DoGetSize();
     }
     return fSize;
   }
};


template <typename T>
class RTreeElement : public RTreeElementBase {
   T* fValue;

   void Initialize();

public:
   template<typename... ArgsT>
   RTreeElement(T *value) : fValue(value) {
     Initialize();
     fColumnType = MapType();
   }

   static RTreeColumnType MapType();
   static std::string GetMemoryType();
};

}  // namespace Toy

#endif  // RTREEELEMENT_H_
