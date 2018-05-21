#include "RTreeMedium.hxx"

#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <string>
#include <utility>

#include "RBasket.hxx"

namespace Toy {

std::unique_ptr<RTreeRawSink> RTreeSink::MakeRawSink(
  const std::filesystem::path &path)
{
   return std::move(std::make_unique<RTreeRawSink>(path));
}

RTreeRawSink::RTreeRawSink(const std::filesystem::path &path)
  : fTree(nullptr)
  , fPath(path)
  , fd(open(fPath.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0600))
{ }

RTreeRawSink::~RTreeRawSink() {
   WriteFooter();
   close(fd);
}


void RTreeRawSink::OnCreate() {
  fFilePos = 0;
  //std::string header = "ROOT v7 Tree";
  //write(fd, header.data(), header.size());
}

void RTreeRawSink::OnFullBasket(RBasket *basket, RTreeColumn *column) {
  std::size_t size = basket->GetSize();
  std::size_t epoch = fFilePos / kEpochSize;
  if (((fFilePos + size) / kEpochSize) > epoch) {
    size_t padding = (epoch + 1) * kEpochSize - fFilePos;
    WritePadding(padding);
    WriteMiniFooter();
  }

  BasketHeads* basket_heads;
  auto iter_handle = fEpochIndex.find(column);
  if (iter_handle == fEpochIndex.end()) {
    RColumnHandle handle(fEpochIndex.size(), new BasketHeads());
    basket_heads = handle.fBasketHeads.get();
    fEpochIndex.insert(std::pair<void *, RColumnHandle>(column, handle));
  } else {
    basket_heads = iter_handle->second.fBasketHeads.get();
  }

  basket_heads->push_back(std::make_pair(0, fFilePos));
  write(fd, basket->GetBuffer(), size);
  fFilePos += size;
}

void RTreeRawSink::WriteFooter() {
  std::string footer = "\n";
  write(fd, footer.data(), footer.size());
}

void RTreeRawSink::WritePadding(std::size_t padding) {
  std::array<unsigned char, 4096> zeros;
  size_t written = 0;
  while (written < padding) {
    size_t to_write = std::min(padding - written, std::size_t(4096));
    write(fd, zeros.data(), to_write);
    written += to_write;
  }
  fFilePos += padding;
}

void RTreeRawSink::WriteMiniFooter() {
  std::size_t size = sizeof(std::uint32_t);
  for (auto iter_col_handle : fEpochIndex) {
    size += 2 * sizeof(std::uint32_t) +
      iter_col_handle.second.fBasketHeads->size() * 2 * sizeof(std::uint64_t);
  }
  std::cout << "Write Mini Footer at " << fFilePos <<
      " size " << size << " kB" << std::endl;
  WritePadding(size);
  fEpochIndex.clear();
}

}
