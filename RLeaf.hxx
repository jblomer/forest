#ifndef RLEAF_H_
#define RLEAF_H_

#include <cassert>
#include <memory>

#include "RTreeColumn.hxx"
#include "RTreeElement.hxx"


namespace Toy {

class RLeafBase {
   RTreeColumn* fPrincipalColumn;
   RTreeElementBase* fPrincipalElement;
   bool fIsSimple;
   virtual void DoWrite() { assert(false); }

protected:
   RLeafBase()
     : fPrincipalColumn(nullptr)
     , fPrincipalElement(nullptr)
     , fIsSimple(true)
   { }

   void Init(RTreeColumn* principal_column,
             RTreeElementBase* principal_element) {
     fPrincipalColumn = principal_column;
     fPrincipalElement = principal_element;
     assert(fPrincipalElement->GetColumnType() ==
            fPrincipalColumn->GetColumnType());
   }

public:
   virtual ~RLeafBase() { }

   void Write() {
     if (!fIsSimple) {
       DoWrite();
       return;
     }
     fPrincipalColumn->Append(*fPrincipalElement);
   }
};

template <typename T>
class RLeaf : public RLeafBase {
   std::shared_ptr<T> fValue;
public:
   template <typename... ArgsT>
   RLeaf(ArgsT&&... args) : RLeafBase() {
     fValue = std::make_shared<T>(std::forward<ArgsT>(args)...);
   }

   std::shared_ptr<T> Get() { return fValue; }
};

}  // namespace Toy


#endif // RLEAF_H_
