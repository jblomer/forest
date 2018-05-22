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
class RTreeRawSource;

class RTreeSink {
public:
   static std::unique_ptr<RTreeRawSink> MakeRawSink(
     const std::filesystem::path &path);

   virtual ~RTreeSink() { }

   virtual void Attach(RTree *tree) = 0;

   virtual void OnCreate() = 0;
   virtual void OnAddColumn(RTreeColumn *column) = 0;
   virtual void OnFullBasket(RBasket *basket, RTreeColumn *column) = 0;
};


class RTreeRawSink : public RTreeSink {
  static constexpr std::size_t kEpochSize = 1024 * 1024 * 10;

  using BasketHeads = std::vector<std::pair<uint64_t, uint64_t>>;

  struct RColumnIndex {
    RColumnIndex(std::uint32_t id)
      : fId(id)
    { }
    std::uint32_t fId;
    BasketHeads fBasketHeads;
  };

  RTree *fTree;
  std::filesystem::path fPath;
  int fd;
  std::size_t fFilePos;
  std::unordered_map<RTreeColumn*, std::unique_ptr<RColumnIndex>> fGlobalIndex;
  std::unordered_map<RTreeColumn*, std::unique_ptr<RColumnIndex>> fEpochIndex;

  void Write(void *buf, std::size_t size);
  void WriteFooter();
  void WriteMiniFooter();
  void WritePadding(std::size_t padding);

public:
   RTreeRawSink(const std::filesystem::path &path);
   virtual ~RTreeRawSink();

   virtual void Attach(RTree *tree) override { fTree = tree; }

   virtual void OnCreate() override;
   virtual void OnAddColumn(RTreeColumn *column) override;
   virtual void OnFullBasket(RBasket *basket, RTreeColumn *column) override;
};


class RTreeSource {
public:
   static std::unique_ptr<RTreeRawSource> MakeRawSource(
     const std::filesystem::path &path);

   virtual ~RTreeSource() { }

  virtual void Attach(RTree *tree) = 0;
};

class RTreeRawSource : public RTreeSource {
  std::filesystem::path fPath;
   RTree *fTree;
   int fd;

public:
  RTreeRawSource(const std::filesystem::path &path)
    : fPath(path)
    , fTree(nullptr)
    , fd(-1) {}
  ~RTreeRawSource();

   virtual void Attach(RTree *tree) override;
};

}

#endif  // RTREEMEDIUM_H_
