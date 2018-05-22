#include "RTreeMedium.hxx"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <cassert>
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
  std::cout << "WRITING HEADER" << std::endl;
  std::uint32_t num_cols = fGlobalIndex.size();
  Write(&num_cols, sizeof(num_cols));
  for (auto& iter_col : fGlobalIndex) {
    Write(&(iter_col.second->fId), sizeof(iter_col.second->fId));
    RTreeColumnType type = iter_col.first->GetModel().GetType();
    Write(&type, sizeof(type));
    std::string name = iter_col.first->GetModel().GetName();
    std::uint32_t name_len = name.length();
    Write(&name_len, sizeof(name_len));
    Write(name.data(), name.length());
  }
}

void RTreeRawSink::OnAddColumn(RTreeColumn *column) {
  std::uint32_t id = fGlobalIndex.size();
  fGlobalIndex[column] = std::make_unique<RColumnIndex>(id);
  fEpochIndex[column] = std::make_unique<RColumnIndex>(id);
}

void RTreeRawSink::OnFullBasket(RBasket *basket, RTreeColumn *column) {
  std::size_t size = basket->GetSize();
  std::size_t epoch = fFilePos / kEpochSize;
  if (((fFilePos + size) / kEpochSize) > epoch) {
    size_t padding = (epoch + 1) * kEpochSize - fFilePos;
    WritePadding(padding);
    WriteMiniFooter();
  }

  std::pair<uint64_t, uint64_t> entry(0, fFilePos);
  auto iter_global_index = fGlobalIndex.find(column);
  auto iter_epoch_index = fEpochIndex.find(column);
  iter_global_index->second->fBasketHeads.push_back(entry);
  iter_epoch_index->second->fBasketHeads.push_back(entry);

  write(fd, basket->GetBuffer(), size);
  fFilePos += size;
}

void RTreeRawSink::Write(void *buf, std::size_t size) {
  ssize_t retval = write(fd, buf, size);
  assert(retval >= 0);
  assert(size_t(retval) == size);
  fFilePos += size;
}

void RTreeRawSink::WriteFooter() {
  std::cout << "WRITING FOOTER" << std::endl;
  size_t footer_pos = fFilePos;
  for (auto& iter_col : fGlobalIndex) {
    Write(&(iter_col.second->fId), sizeof(iter_col.second->fId));
    uint32_t nbaskets = iter_col.second->fBasketHeads.size();
    Write(&nbaskets, sizeof(nbaskets));
    if (nbaskets > 0)
      Write(iter_col.second->fBasketHeads.data(),
            nbaskets * sizeof(std::pair<uint64_t, uint64_t>));
    iter_col.second->fBasketHeads.clear();
  }
  Write(&footer_pos, sizeof(footer_pos));
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
  std::cout << "Write Mini Footer at " << fFilePos << std::endl;
  for (auto& iter_col : fEpochIndex) {
    Write(&(iter_col.second->fId), sizeof(iter_col.second->fId));
    uint32_t nbaskets = iter_col.second->fBasketHeads.size();
    Write(&nbaskets, sizeof(nbaskets));
    if (nbaskets > 0)
      Write(iter_col.second->fBasketHeads.data(),
            nbaskets * sizeof(std::pair<uint64_t, uint64_t>));
    iter_col.second->fBasketHeads.clear();
  }
}


//------------------------------------------------------------------------------


std::unique_ptr<RTreeRawSource> RTreeSource::MakeRawSource(
  const std::filesystem::path &path)
{
  return std::move(std::make_unique<RTreeRawSource>(path));
}

void RTreeRawSource::Read(void *buf, size_t size) {
  ssize_t retval = read(fd, buf, size);
  assert(retval >= 0);
  assert(size_t(retval) == size);
}

void RTreeRawSource::Attach(RTree *tree) {
  fTree = tree;
  fd = open(fPath.c_str(), O_RDONLY);
  size_t footer_pos;
  lseek(fd, -sizeof(footer_pos), SEEK_END);
  std::size_t eof_pos = lseek(fd, 0, SEEK_CUR);
  Read(&footer_pos, sizeof(footer_pos));
  std::cout << "Found footer pos at " << footer_pos
    << ", eof pos at " << eof_pos << std::endl;

  lseek(fd, footer_pos, SEEK_SET);
  std::size_t cur_pos = footer_pos;
  while (cur_pos < eof_pos) {
    std::uint32_t id;
    Read(&id, sizeof(id));  cur_pos += sizeof(id);
    std::uint32_t nbaskets;
    Read(&nbaskets, sizeof(nbaskets));  cur_pos += sizeof(nbaskets);
    for (unsigned i = 0; i < nbaskets; ++i) {
      std::pair<std::uint64_t, std::uint64_t> index_entry;
      Read(&index_entry, sizeof(index_entry));  cur_pos += sizeof(index_entry);
    }
    std::cout << "Read index of branch " << id <<
      " with " << nbaskets << " baskets" << std::endl;
  }
}

RTreeRawSource::~RTreeRawSource() {
  close(fd);
}


}
