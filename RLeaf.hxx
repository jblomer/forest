#ifndef RLEAF_H_
#define RLEAF_H_

#include <cassert>
#include <memory>
#include <utility>

namespace Toy {

class RLeafBase {
protected:
   // We can indicate that on disk format == in memory format
   // Improve
   bool fMovable;
   void *fRawContent;
   unsigned fSize;

   virtual void DoSerialize() { assert(false); }
public:
   RLeafBase() : fMovable(false), fRawContent(nullptr), fSize(0) { }
   virtual ~RLeafBase() { }

   void Serialize() {
     if (/* EXPECT_TRUE */ fMovable) {

     } else {
        DoSerialize();
     }
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
