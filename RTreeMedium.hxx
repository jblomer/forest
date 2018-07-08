#ifndef RTREEMEDIUM_H_
#define RTREEMEDIUM_H_

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "RTreeColumn.hxx"

namespace Toy {

class RColumnSlice;
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
   virtual void OnFullSlice(RColumnSlice *slice, RTreeColumn *column) = 0;
};


class RTreeRawSink : public RTreeSink {
  static constexpr std::size_t kEpochSize = 1024 * 1024 * 10;

  using SliceHeads = std::vector<std::pair<uint64_t, uint64_t>>;

  struct RColumnIndex {
    RColumnIndex(std::uint32_t id)
      : fId(id)
      , fNumElements(0)
    { }
    std::uint32_t fId;
    std::uint64_t fNumElements;
    SliceHeads fSliceHeads;
  };

  RTree *fTree;
  std::filesystem::path fPath;
  int fd;
  std::size_t fFilePos;
  std::unordered_map<RTreeColumn*, std::unique_ptr<RColumnIndex>> fGlobalIndex;
  std::unordered_map<RTreeColumn*, std::unique_ptr<RColumnIndex>> fEpochIndex;

  void Write(void *buf, std::size_t size);
  void WriteFooter(std::uint64_t nentries);
  void WriteMiniFooter();
  void WritePadding(std::size_t padding);

public:
   RTreeRawSink(const std::filesystem::path &path);
   virtual ~RTreeRawSink();

   virtual void Attach(RTree *tree) override { fTree = tree; }

   virtual void OnCreate() override;
   virtual void OnAddColumn(RTreeColumn *column) override;
   virtual void OnFullSlice(RColumnSlice *slice, RTreeColumn *column) override;
};


class RTreeSource {
public:
   static std::unique_ptr<RTreeRawSource> MakeRawSource(
     const std::filesystem::path &path);

  virtual ~RTreeSource() { }

  virtual void Attach(RTree *tree) = 0;
  virtual void OnAddColumn(RTreeColumn *column) = 0;
  virtual void OnMapSlice(
    RTreeColumn *column, std::uint64_t num, RColumnSlice *slice) = 0;
  virtual std::uint64_t GetNentries() = 0;
  virtual std::uint64_t GetNElements(RTreeColumn *column) = 0;
};

class RTreeRawSource : public RTreeSource {
  using Index = std::vector<std::pair<std::uint64_t, std::uint64_t>>;
  using ColumnIds = std::unordered_map<std::string, std::uint32_t>;
  using ColumnElementSizes = std::unordered_map<std::uint32_t, std::size_t>;
  using ColumnElements = std::unordered_map<std::uint32_t, std::uint64_t>;
  using LiveColumns = std::unordered_map<RTreeColumn*, std::uint32_t>;

  std::filesystem::path fPath;
  RTree *fTree;
  int fd;
  std::uint64_t fNentries;
  std::vector<std::unique_ptr<Index>> fIndex;
  ColumnIds fColumnIds;
  ColumnElementSizes fColumnElementSizes;
  ColumnElements fColumnElements;
  LiveColumns fLiveColumns;

  void Read(void *buf, std::size_t size);
  void Seek(std::size_t size);

public:
  RTreeRawSource(const std::filesystem::path &path)
    : fPath(path)
    , fTree(nullptr)
    , fd(-1)
    , fNentries(0)
  { }
  ~RTreeRawSource();

   virtual void Attach(RTree *tree) override;
   virtual void OnAddColumn(RTreeColumn *column) override;
   virtual void OnMapSlice(
    RTreeColumn *column, std::uint64_t num, RColumnSlice *slice) override;
   virtual std::uint64_t GetNentries() override {
     return fNentries;
   }
   virtual std::uint64_t GetNElements(RTreeColumn *column) override;
};

}

#endif  // RTREEMEDIUM_H_
