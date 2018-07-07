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
#include "RTree.hxx"

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
   WriteFooter(fTree->GetNentries());
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
    std::size_t element_size = iter_col.first->GetModel().GetElementSize();
    Write(&element_size, sizeof(element_size));
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
  std::size_t num_elements = size / column->GetModel().GetElementSize();
  std::size_t epoch = fFilePos / kEpochSize;
  if (((fFilePos + size) / kEpochSize) > epoch) {
    size_t padding = (epoch + 1) * kEpochSize - fFilePos;
    WritePadding(padding);
    WriteMiniFooter();
  }

  std::pair<uint64_t, uint64_t> entry(basket->GetRangeStart(), fFilePos);
  auto iter_global_index = fGlobalIndex.find(column);
  auto iter_epoch_index = fEpochIndex.find(column);
  iter_global_index->second->fBasketHeads.push_back(entry);
  iter_epoch_index->second->fBasketHeads.push_back(entry);

  iter_global_index->second->fNumElements += num_elements;
  iter_epoch_index->second->fNumElements += num_elements;
  //std::cout << "adding " << num_elements << " elements"
  //          << ", basket size " << size << std::endl;
  //if (column->GetColumnType() == RTreeColumnType::kOffset) {
  //  std::cout << "OFFSET BASKET " << column->GetName()
  //    << "  1st element " << *(uint64_t *)(basket->GetBuffer())
  //    << std::endl;
  //} else {
  //  std::cout << "FLOAT BASKET " << column->GetName()
  //     << "  1st element " << *(float *)(basket->GetBuffer())
  //     << std::endl;
  //}

  write(fd, basket->GetBuffer(), size);
  fFilePos += size;
}

void RTreeRawSink::Write(void *buf, std::size_t size) {
  ssize_t retval = write(fd, buf, size);
  assert(retval >= 0);
  assert(size_t(retval) == size);
  fFilePos += size;
}

void RTreeRawSink::WriteFooter(std::uint64_t nentries) {
  std::cout << "WRITING FOOTER" << std::endl;
  size_t footer_pos = fFilePos;
  Write(&nentries, sizeof(nentries));
  for (auto& iter_col : fGlobalIndex) {
    Write(&(iter_col.second->fId), sizeof(iter_col.second->fId));
    Write(&(iter_col.second->fNumElements),
          sizeof(iter_col.second->fNumElements));
    uint32_t nbaskets = iter_col.second->fBasketHeads.size();
    Write(&nbaskets, sizeof(nbaskets));
    if (nbaskets > 0) {
      Write(iter_col.second->fBasketHeads.data(),
            nbaskets * sizeof(std::pair<uint64_t, uint64_t>));
    }
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
    Write(&(iter_col.second->fNumElements),
          sizeof(iter_col.second->fNumElements));
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

void RTreeRawSource::OnAddColumn(RTreeColumn *column) {
  auto iter = fColumnIds.find(column->GetModel().GetName());
  if (iter == fColumnIds.end())
    throw std::string("not found");
  std::uint32_t column_id = iter->second;
  std::cout << "Found column " << column->GetModel().GetName()
            << " under id " << column_id << std::endl;
  fLiveColumns[column] = column_id;
}

void RTreeRawSource::OnMapSlice(
  RTreeColumn *column,
  std::uint64_t num,
  RBasket *basket)
{
  auto iter = fLiveColumns.find(column);
  if (iter == fLiveColumns.end())
    throw "not found";
  std::uint32_t column_id = iter->second;
  //std::cout << "OnMapSlice column " << column_id
  //  << " #" << num << std::endl;

  // TODO: binary search
  std::uint64_t file_pos = 0;
  std::uint64_t first_in_basket = 0;
  std::uint64_t first_outside_basket = fColumnElements[column_id];
  Index *idx = fIndex[column_id].get();
  bool stop = false;
  for (auto idx_elem : *idx) {
    if (stop) {
      first_outside_basket = idx_elem.first;
      break;
    }
    // TODO: this works only for sequential access
    if (idx_elem.first == num) {
      first_in_basket = idx_elem.first;
      file_pos = idx_elem.second;
      stop = true;
    }
    //std::cout << "ELEM " << idx_elem.first << " FILEPOS " << idx_elem.second
    //          << std::endl;
  }
  std::uint64_t elements_in_basket = first_outside_basket - first_in_basket;
  std::uint64_t basket_size =
    elements_in_basket * fColumnElementSizes[column_id];

  //std::cout << "Basket has size " << basket_size << " and " <<
  //             elements_in_basket << " elements" << std::endl;
//
  //std::cout << "Mapping slice for element number " << num
  //          << " for column id " << column_id << std::endl;

  basket->Reset(first_in_basket);
  assert(basket->GetCapacity() >= basket_size);
  basket->Reserve(basket_size);
  Seek(file_pos);
  Read(basket->GetBuffer(), basket_size);
}

void RTreeRawSource::Seek(size_t pos) {
  lseek(fd, pos, SEEK_SET);
}

void RTreeRawSource::Read(void *buf, size_t size) {
  ssize_t retval = read(fd, buf, size);
  assert(retval >= 0);
  assert(size_t(retval) == size);
}

void RTreeRawSource::Attach(RTree *tree) {
  fTree = tree;
  fd = open(fPath.c_str(), O_RDONLY);

  std::uint32_t num_cols;
  Read(&num_cols, sizeof(num_cols));
  std::cout << "Found " << num_cols << " columns" << std::endl;
  for (unsigned i = 0; i < num_cols; ++i) {
    uint32_t id;
    Read(&id, sizeof(id));
    RTreeColumnType type;
    Read(&type, sizeof(type));
    std::size_t element_size;
    Read(&element_size, sizeof(element_size));
    uint32_t name_len;
    Read(&name_len, sizeof(name_len));
    char *name_raw = new char[name_len];
    Read(name_raw, name_len);
    std::string name(name_raw, name_len);
    delete[] name_raw;

    std::cout << "Column " << name << ", id " << id << ", type "
              << int(type) << ", element size " << element_size << std::endl;
    fIndex.push_back(nullptr);
    fColumnIds[name] = id;
    fColumnElementSizes[id] = element_size;
  }

  size_t footer_pos;
  lseek(fd, -sizeof(footer_pos), SEEK_END);
  std::size_t eof_pos = lseek(fd, 0, SEEK_CUR);
  Read(&footer_pos, sizeof(footer_pos));
  std::cout << "Found footer pos at " << footer_pos
    << ", eof pos at " << eof_pos << std::endl;

  lseek(fd, footer_pos, SEEK_SET);
  Read(&fNentries, sizeof(fNentries));
  std::cout << "Found #" << fNentries << " entries in file" << std::endl;
  std::size_t cur_pos = footer_pos + sizeof(fNentries);
  while (cur_pos < eof_pos) {
    std::uint32_t id;
    Read(&id, sizeof(id));  cur_pos += sizeof(id);
    std::uint64_t num_elements;
    Read(&num_elements, sizeof(num_elements));  cur_pos += sizeof(num_elements);
    std::uint32_t nbaskets;
    Read(&nbaskets, sizeof(nbaskets));  cur_pos += sizeof(nbaskets);
    Index* col_index = new Index();
    for (unsigned i = 0; i < nbaskets; ++i) {
      std::pair<std::uint64_t, std::uint64_t> index_entry;
      Read(&index_entry, sizeof(index_entry));  cur_pos += sizeof(index_entry);
      //std::cout << "  READ " << index_entry.first << "/" << index_entry.second << std::endl;
      col_index->push_back(index_entry);
    }
    fIndex[id] = std::move(std::unique_ptr<Index>(col_index));
    fColumnElements[id] = num_elements;
    std::cout << "Read index of column " << id <<
      " with " << nbaskets << " baskets" <<
      " and " << num_elements << " elements" << std::endl;
  }
}

RTreeRawSource::~RTreeRawSource() {
  close(fd);
}


}
