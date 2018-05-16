#ifndef RTREEMEDIUM_H_
#define RTREEMEDIUM_H_

#include <filesystem>
#include <memory>

#include "RTreeColumn.hxx"

namespace Toy {

class RBasket;
class RTree;
class RTreeModel;
class RTreeRawSink;

class RTreeSink {
public:
   static std::unique_ptr<RTreeRawSink> MakeRawSink(
     const std::filesystem::path &path);

   virtual ~RTreeSink() { }

   virtual void Attach(RTree *tree) = 0;

   virtual void OnCreate() = 0;
   virtual void OnFullBasket(RBasket *basket, RTreeColumn *column) = 0;
};


class RTreeRawSink : public RTreeSink {
   RTree *fTree;
   std::filesystem::path fPath;
   int fd;

   void WriteFooter();

public:
   RTreeRawSink(const std::filesystem::path &path);
   virtual ~RTreeRawSink();

   virtual void Attach(RTree *tree) override { fTree = tree; }

   virtual void OnCreate() override;
   virtual void OnFullBasket(RBasket *basket, RTreeColumn *column) override;
};

}

#endif  // RTREEMEDIUM_H_
