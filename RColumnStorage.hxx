#ifndef RCOLUMNSTORAGE_H_
#define RCOLUMNSTORAGE_H_

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "RColumn.hxx"

namespace Toy {

class RColumnSlice;
class RTree;
class RTreeModel;
class RColumnSinkRaw;
class RColumnSourceRaw;

class RColumnSink {
public:
   static std::unique_ptr<RColumnSinkRaw> MakeSinkRaw(
     const std::filesystem::path &path);

   virtual ~RColumnSink() { }

   virtual void Attach(RTree *tree) = 0;

   virtual void OnCreate() = 0;
   virtual void OnAddColumn(RColumn *column) = 0;
   virtual void OnFullSlice(RColumnSlice *slice, RColumn *column) = 0;
};


class RColumnSinkRaw : public RColumnSink {
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
  std::unordered_map<RColumn*, std::unique_ptr<RColumnIndex>> fGlobalIndex;
  std::unordered_map<RColumn*, std::unique_ptr<RColumnIndex>> fEpochIndex;

  void Write(void *buf, std::size_t size);
  void WriteFooter(std::uint64_t nentries);
  void WriteMiniFooter();
  void WritePadding(std::size_t padding);

public:
   RColumnSinkRaw(const std::filesystem::path &path);
   virtual ~RColumnSinkRaw();

   virtual void Attach(RTree *tree) override { fTree = tree; }

   virtual void OnCreate() override;
   virtual void OnAddColumn(RColumn *column) override;
   virtual void OnFullSlice(RColumnSlice *slice, RColumn *column) override;
};


class RColumnSource {
public:
   static std::unique_ptr<RColumnSourceRaw> MakeSourceRaw(
     const std::filesystem::path &path);

  virtual ~RColumnSource() { }

  virtual void Attach(RTree *tree) = 0;
  virtual void OnAddColumn(RColumn *column) = 0;
  virtual void OnMapSlice(
    RColumn *column, std::uint64_t num, RColumnSlice *slice) = 0;
  virtual std::uint64_t GetNentries() = 0;
  virtual std::uint64_t GetNElements(RColumn *column) = 0;
};

class RColumnSourceRaw : public RColumnSource {
  using Index = std::vector<std::pair<std::uint64_t, std::uint64_t>>;
  using ColumnIds = std::unordered_map<std::string, std::uint32_t>;
  using ColumnElementSizes = std::unordered_map<std::uint32_t, std::size_t>;
  using ColumnElements = std::unordered_map<std::uint32_t, std::uint64_t>;
  using LiveColumns = std::unordered_map<RColumn*, std::uint32_t>;

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
  RColumnSourceRaw(const std::filesystem::path &path)
    : fPath(path)
    , fTree(nullptr)
    , fd(-1)
    , fNentries(0)
  { }
  ~RColumnSourceRaw();

   virtual void Attach(RTree *tree) override;
   virtual void OnAddColumn(RColumn *column) override;
   virtual void OnMapSlice(
    RColumn *column, std::uint64_t num, RColumnSlice *slice) override;
   virtual std::uint64_t GetNentries() override {
     return fNentries;
   }
   virtual std::uint64_t GetNElements(RColumn *column) override;
};

}

#endif  // RCOLUMNSTORAGE_H_
