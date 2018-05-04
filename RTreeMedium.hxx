#ifndef RTREEMEDIUM_H_
#define RTREEMEDIUM_H_

#include <experimental/filesystem>
#include <memory>

namespace Toy {

class RBasket;
class RTree;
class RTreeModel;
class RTreeRawSink;

class RTreeSink {
public:
   static std::unique_ptr<RTreeRawSink> MakeRawSink(
     const std::experimental::filesystem::path &path);

   virtual ~RTreeSink() { }

   virtual void Attach(RTree *tree) = 0;

   virtual void OnCreate() = 0;
   virtual void OnFullBasket(RBasket *basket) = 0;
};


class RTreeRawSink : public RTreeSink {
   RTree *fTree;
   std::experimental::filesystem::path fPath;
   int fd;

   void WriteFooter();

public:
   RTreeRawSink(const std::experimental::filesystem::path &path);
   virtual ~RTreeRawSink();

   virtual void Attach(RTree *tree) override { fTree = tree; }

   virtual void OnCreate() override;
   virtual void OnFullBasket(RBasket *basket) override;
};

}

#endif  // RTREEMEDIUM_H_
