#include "TTreeMedium.hxx"

namespace Toy {

std::unique_ptr<TTreeMedium> MakeFileSink(
   const std::experimental::filesystem::path &path)
{
   return std::make_unique<TTreeMedium>();
}

}
