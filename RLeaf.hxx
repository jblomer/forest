#ifndef RLEAF_H_
#define RLEAF_H_

#include <cassert>
#include <cstddef>
#include <memory>
#include <utility>

namespace Toy {

class RLeafBase {
protected:
   // We can indicate that on disk format == in memory format
   // Improve
   bool fIsMovable;
   void *fRawContent;

   bool fIsFixedSized;
   unsigned fSize;

   virtual void DoSerialize() { assert(false); }
   virtual std::size_t DoGetSize() { assert(false); }
public:
   RLeafBase()
     : fIsMovable(false)
     , fRawContent(nullptr)
     , fIsFixedSized(false)
     , fSize(0) { }
   virtual ~RLeafBase() { }

   void Serialize() {
     if (!fIsMovable) {
       DoSerialize();
       return;
     }
     // Memory copy
   }

   std::size_t GetSize() {
     if (!fIsFixedSized) {
        return DoGetSize();
     }
     return fSize;
   }
};


template <typename T>
class RLeaf : public RLeafBase {
   std::shared_ptr<T> fValue;

public:
   template<typename... ArgsT>
   RLeaf(ArgsT&&... args)
     : fValue(std::make_shared<T>(std::forward<ArgsT>(args)...)) { }

   std::shared_ptr<T> Get() { return fValue; }
};

}  // namespace Toy

#endif  // RLEAF_H_
