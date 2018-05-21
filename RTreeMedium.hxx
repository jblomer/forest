#ifndef RTREEMEDIUM_H_
#define RTREEMEDIUM_H_

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

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
  static constexpr std::size_t kEpochSize = 1024*1024*10;

  using BasketHeads = std::vector<std::pair<uint64_t, uint64_t>>;

  struct RColumnHandle {
    RColumnHandle(std::uint32_t id, BasketHeads* heads)
      : fId(id)
      , fBasketHeads(heads)
    { }
    std::uint32_t fId;
    // TODO: make me unique
    std::shared_ptr<BasketHeads> fBasketHeads;
  };

  RTree *fTree;
  std::filesystem::path fPath;
  int fd;
  std::size_t fFilePos;
  std::unordered_map<void *, RColumnHandle> fGlobalIndex;
  std::unordered_map<void *, RColumnHandle> fEpochIndex;

  void WriteFooter();
  void WriteMiniFooter();
  void WritePadding(std::size_t padding);

public:
   RTreeRawSink(const std::filesystem::path &path);
   virtual ~RTreeRawSink();

   virtual void Attach(RTree *tree) override { fTree = tree; }

   virtual void OnCreate() override;
   virtual void OnFullBasket(RBasket *basket, RTreeColumn *column) override;
};

}

#endif  // RTREEMEDIUM_H_
