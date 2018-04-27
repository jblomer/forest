#ifndef TTREEMEDIUM_H_
#define TTREEMEDIUM_H_

#include <experimental/filesystem>
#include <memory>

namespace Toy {

class TTreeModel;

class RTreeRawSink;

class RTreeSink {
protected:
   virtual bool Create(TTreeModel *model) = 0;

public:
   static std::unique_ptr<RTreeRawSink> MakeRawSink(
     const std::experimental::filesystem::path &path);

   virtual ~RTreeSink() { }
};


class RTreeRawSink : public RTreeSink {
protected:
   RTreeRawSink(
     const TTreeModel *model,
     const std::experimental::filesystem::path &path);


public:

};

}

#endif  // TTREEMEDIUM_H_
