#include "ROOT/RTree.hxx"
#include "ROOT/RTreeModel.hxx"

#include <chrono>
#include <memory>
#include <utility>

int main(int argc, char **argv) {
   using ERangeType = ROOT::Experimental::ERangeType;
   using RColumnSink = ROOT::Experimental::RColumnSink;
   using RColumnSource = ROOT::Experimental::RColumnSource;
   using RColumnRange = ROOT::Experimental::RColumnRange;
   using RTree = ROOT::Experimental::RTree;
   using RTreeModel = ROOT::Experimental::RTreeModel;
   using RColumnRawSettings = ROOT::Experimental::RColumnRawSettings;

   auto event_model = std::make_shared<RTreeModel>();
   RTree tree(event_model, RColumnSource::MakeSourceRaw(argv[1]));

   return 0;
}