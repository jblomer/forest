#ifndef TSPROUT_H_
#define TSPROUT_H_

#include <memory>
#include <utility>

namespace Toy {

class TSproutBase {
public:
   virtual ~TSproutBase() { }
};

/**
 * Memory representation of the Tip of a TTreeBranch
 * (should it become TTreeValue??)
 */
template <typename T>
class TSprout : public TSproutBase {
   std::shared_ptr<T> fValue;

public:
   template<typename... ArgsT>
   TSprout(ArgsT&&... args)
     : fValue(std::make_shared<T>(std::forward<ArgsT>(args)...)) { }

   std::shared_ptr<T> Get() { return fValue; }
};

}  // namespace Toy

#endif  // TSPROUT_H_
