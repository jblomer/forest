#include <experimental/filesystem>
#include <memory>

namespace Toy {

class TTreeMedium {
public:
   static std::unique_ptr<TTreeMedium> MakeFileSink(
     const std::experimental::filesystem::path &path);

   // TODO: constructor for chain of files
};

}