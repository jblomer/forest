#include "RColumnStorage.hxx"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <string>
#include <utility>

#include "RColumnSlice.hxx"
#include "RTree.hxx"

namespace Toy {

std::unique_ptr<RColumnSinkRaw> RColumnSink::MakeSinkRaw(
  const std::filesystem::path &path)
{
   return std::move(std::make_unique<RColumnSinkRaw>(path));
}

RColumnSinkRaw::RColumnSinkRaw(const std::filesystem::path &path)
  : fTree(nullptr)
  , fPath(path)
  , fd(open(fPath.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0600))
{ }

RColumnSinkRaw::~RColumnSinkRaw() {
   WriteFooter(fTree->GetNentries());
   close(fd);
}


void RColumnSinkRaw::OnCreate() {
  fFilePos = 0;
  std::cout << "WRITING HEADER" << std::endl;
  std::uint32_t num_cols = fGlobalIndex.size();
  Write(&num_cols, sizeof(num_cols));
  for (auto& iter_col : fGlobalIndex) {
    Write(&(iter_col.second->fId), sizeof(iter_col.second->fId));
    RColumnType type = iter_col.first->GetModel().GetType();
    Write(&type, sizeof(type));
    std::size_t element_size = iter_col.first->GetModel().GetElementSize();
    Write(&element_size, sizeof(element_size));
    std::string name = iter_col.first->GetModel().GetName();
    std::uint32_t name_len = name.length();
    Write(&name_len, sizeof(name_len));
    Write(name.data(), name.length());
  }
}

void RColumnSinkRaw::OnAddColumn(RColumn *column) {
  std::uint32_t id = fGlobalIndex.size();
  fGlobalIndex[column] = std::make_unique<RColumnIndex>(id);
  fEpochIndex[column] = std::make_unique<RColumnIndex>(id);
}

void RColumnSinkRaw::OnFullSlice(RColumnSlice *slice, RColumn *column) {
  std::size_t size = slice->GetSize();
  std::size_t num_elements = size / column->GetModel().GetElementSize();
  std::size_t epoch = fFilePos / kEpochSize;
  if (((fFilePos + size) / kEpochSize) > epoch) {
    size_t padding = (epoch + 1) * kEpochSize - fFilePos;
    WritePadding(padding);
    WriteMiniFooter();
  }

  std::pair<uint64_t, uint64_t> entry(slice->GetRangeStart(), fFilePos);
  auto iter_global_index = fGlobalIndex.find(column);
  auto iter_epoch_index = fEpochIndex.find(column);
  iter_global_index->second->fSliceHeads.push_back(entry);
  iter_epoch_index->second->fSliceHeads.push_back(entry);

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

  write(fd, slice->GetBuffer(), size);
  fFilePos += size;
}

void RColumnSinkRaw::Write(void *buf, std::size_t size) {
  ssize_t retval = write(fd, buf, size);
  assert(retval >= 0);
  assert(size_t(retval) == size);
  fFilePos += size;
}

void RColumnSinkRaw::WriteFooter(std::uint64_t nentries) {
  std::cout << "WRITING FOOTER" << std::endl;
  size_t footer_pos = fFilePos;
  Write(&nentries, sizeof(nentries));
  for (auto& iter_col : fGlobalIndex) {
    Write(&(iter_col.second->fId), sizeof(iter_col.second->fId));
    Write(&(iter_col.second->fNumElements),
          sizeof(iter_col.second->fNumElements));
    uint32_t nslices = iter_col.second->fSliceHeads.size();
    Write(&nslices, sizeof(nslices));
    if (nslices > 0) {
      Write(iter_col.second->fSliceHeads.data(),
            nslices * sizeof(std::pair<uint64_t, uint64_t>));
    }
    iter_col.second->fSliceHeads.clear();
  }
  Write(&footer_pos, sizeof(footer_pos));
}

void RColumnSinkRaw::WritePadding(std::size_t padding) {
  std::array<unsigned char, 4096> zeros;
  size_t written = 0;
  while (written < padding) {
    size_t to_write = std::min(padding - written, std::size_t(4096));
    write(fd, zeros.data(), to_write);
    written += to_write;
  }
  fFilePos += padding;
}

void RColumnSinkRaw::WriteMiniFooter() {
  std::cout << "Write Mini Footer at " << fFilePos << std::endl;
  for (auto& iter_col : fEpochIndex) {
    Write(&(iter_col.second->fId), sizeof(iter_col.second->fId));
    Write(&(iter_col.second->fNumElements),
          sizeof(iter_col.second->fNumElements));
    uint32_t nslices = iter_col.second->fSliceHeads.size();
    Write(&nslices, sizeof(nslices));
    if (nslices > 0)
      Write(iter_col.second->fSliceHeads.data(),
            nslices * sizeof(std::pair<uint64_t, uint64_t>));
    iter_col.second->fSliceHeads.clear();
  }
}


//------------------------------------------------------------------------------


std::unique_ptr<RColumnSourceRaw> RColumnSource::MakeSourceRaw(
  const std::filesystem::path &path)
{
  return std::move(std::make_unique<RColumnSourceRaw>(path));
}

void RColumnSourceRaw::OnAddColumn(RColumn *column) {
  auto iter = fColumnIds.find(column->GetModel().GetName());
  if (iter == fColumnIds.end())
    throw std::string("not found");
  std::uint32_t column_id = iter->second;
  std::cout << "Found column " << column->GetModel().GetName()
            << " under id " << column_id << std::endl;
  fLiveColumns[column] = column_id;
}

void RColumnSourceRaw::OnMapSlice(
  RColumn *column,
  std::uint64_t num,
  RColumnSlice *slice)
{
  auto iter = fLiveColumns.find(column);
  if (iter == fLiveColumns.end())
    throw "not found";
  std::uint32_t column_id = iter->second;
//  std::cout << "OnMapSlice column " << column_id
//    << " #" << num << std::endl;

  // TODO: binary search
  std::uint64_t file_pos = 0;
  std::uint64_t first_in_slice = 0;
  std::uint64_t first_outside_slice = fColumnElements[column_id];
  Index *idx = fIndex[column_id].get();
  bool stop = false;
  for (auto idx_elem : *idx) {
    if (stop) {
      first_outside_slice = idx_elem.first;
      break;
    }
    // TODO: this works only for sequential access
    if (idx_elem.first == num) {
      first_in_slice = idx_elem.first;
      file_pos = idx_elem.second;
      stop = true;
    }
    //std::cout << "ELEM " << idx_elem.first << " FILEPOS " << idx_elem.second
    //          << std::endl;
  }
  std::uint64_t elements_in_slice = first_outside_slice - first_in_slice;
  std::uint64_t slice_size =
    elements_in_slice * fColumnElementSizes[column_id];

//  std::cout << "Basket has size " << basket_size << " and " <<
//               elements_in_basket << " elements" << std::endl;
//  std::cout << "Mapping slice for element number " << num
//            << " for column id " << column_id << std::endl;

  slice->Reset(first_in_slice);
  assert(slice->GetCapacity() >= slice_size);
  slice->Reserve(slice_size);
  Seek(file_pos);
  Read(slice->GetBuffer(), slice_size);
}

std::uint64_t RColumnSourceRaw::GetNElements(RColumn *column) {
  auto iter = fLiveColumns.find(column);
  if (iter == fLiveColumns.end())
    throw "not found";
  std::uint32_t column_id = iter->second;
  std::uint64_t nelements = fColumnElements[column_id];
  std::cout << "Column #" << column_id << " has " << nelements << " elements"
            << std::endl;
  return fColumnElements[column_id];
}

void RColumnSourceRaw::Seek(size_t pos) {
  lseek(fd, pos, SEEK_SET);
}

void RColumnSourceRaw::Read(void *buf, size_t size) {
  ssize_t retval = read(fd, buf, size);
  assert(retval >= 0);
  assert(size_t(retval) == size);
}

void RColumnSourceRaw::Attach(RTree *tree) {
  fTree = tree;
  fd = open(fPath.c_str(), O_RDONLY);

  std::uint32_t num_cols;
  Read(&num_cols, sizeof(num_cols));
  std::cout << "Found " << num_cols << " columns" << std::endl;
  for (unsigned i = 0; i < num_cols; ++i) {
    uint32_t id;
    Read(&id, sizeof(id));
    RColumnType type;
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
    std::uint32_t nslices;
    Read(&nslices, sizeof(nslices));  cur_pos += sizeof(nslices);
    Index* col_index = new Index();
    for (unsigned i = 0; i < nslices; ++i) {
      std::pair<std::uint64_t, std::uint64_t> index_entry;
      Read(&index_entry, sizeof(index_entry));  cur_pos += sizeof(index_entry);
      //std::cout << "  READ " << index_entry.first << "/" << index_entry.second << std::endl;
      col_index->push_back(index_entry);
    }
    fIndex[id] = std::move(std::unique_ptr<Index>(col_index));
    fColumnElements[id] = num_elements;
    std::cout << "Read index of column " << id <<
      " with " << nslices << " slices" <<
      " and " << num_elements << " elements" << std::endl;
  }
}

RColumnSourceRaw::~RColumnSourceRaw() {
  close(fd);
}


}
