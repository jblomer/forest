#include "RTreeMedium.hxx"

#include <fcntl.h>
#include <unistd.h>

#include <string>
#include <utility>

#include "RBasket.hxx"

namespace Toy {

std::unique_ptr<RTreeRawSink> RTreeSink::MakeRawSink(
  const std::experimental::filesystem::path &path)
{
   return std::move(std::make_unique<RTreeRawSink>(path));
}

RTreeRawSink::RTreeRawSink(const std::experimental::filesystem::path &path)
  : fTree(nullptr)
  , fPath(path)
  , fd(open(fPath.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0600))
{ }

RTreeRawSink::~RTreeRawSink() {
   WriteFooter();
   close(fd);
}


void RTreeRawSink::OnCreate() {
   std::string header = "ROOT v7 Tree";
   write(fd, header.data(), header.size());
}

void RTreeRawSink::OnFullBasket(RBasket *basket) {
   write(fd, basket->GetBuffer(), basket->GetSize());
}

void RTreeRawSink::WriteFooter() {
   std::string footer = "\n";
   write(fd, footer.data(), footer.size());
}


}
