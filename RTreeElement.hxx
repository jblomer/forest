#ifndef RTREEELEMENT_H_
#define RTREEELEMENT_H_

#include <cassert>
#include <cstddef>
#include <cstring>
#include <memory>
#include <string>
#include <utility>

#include "RColumnModel.hxx"

namespace Toy {

class RTreeElementBase {
protected:
   // We can indicate that on disk format == in memory format
   bool fIsMovable;
   void *fRawContent;
   unsigned fSize;

   RColumnType fColumnType;

   virtual void DoSerialize(void *destination) const { assert(false); }
   virtual void DoDeserialize(void *source) const { assert(false); }
public:
   RTreeElementBase()
     : fIsMovable(false)
     , fRawContent(nullptr)
     , fSize(0)
     , fColumnType(RColumnType::kByte) { }
   virtual ~RTreeElementBase() { }

   RColumnType GetColumnType() const { return fColumnType; }

   void Serialize(void *destination) const {
     if (!fIsMovable) {
       DoSerialize(destination);
       return;
     }
     //if (fColumnType == RTreeColumnType::kFloat)
     //  printf("WRITE VALUE %f\n", *((float *)fRawContent));
     //else
     //  printf("WRITE VALUE %lu\n", *((std::uint64_t *)fRawContent));
     std::memcpy(destination, fRawContent, fSize);
   }

   void Deserialize(void *source) {
     if (!fIsMovable) {
       DoDeserialize(source);
       return;
     }
     std::memcpy(fRawContent, source, fSize);
   }

   std::size_t GetSize() const {
     return fSize;
   }

   void* GetRawContent() { return fRawContent; }
};


using RTreeOffset = std::uint64_t;


template <typename T>
class RTreeElement : public RTreeElementBase {
   T* fValue;

   void Initialize();

public:
   template<typename... ArgsT>
   explicit RTreeElement(T *value) : fValue(value) {
     Initialize();
     fColumnType = MapType();
   }

   static RColumnType MapType();
   static std::string GetMemoryType();
};

}  // namespace Toy

#endif  // RTREEELEMENT_H_
