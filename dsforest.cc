/**
 * Playground for possible TTree/TBranch interfaces.
 */

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RForestDS.hxx"
#include "ROOT/RTree.hxx"
#include "ROOT/RTreeModel.hxx"

#include <chrono>
#include <memory>
#include <utility>

int main() {
   using RColumnSource = ROOT::Experimental::RColumnSource;
   using RTree = ROOT::Experimental::RTree;
   using RTreeModel = ROOT::Experimental::RTreeModel;

   std::chrono::high_resolution_clock stopwatch;
   auto start_time = stopwatch.now();


  float sum = 0.0;
  //float sum_e = 0.0;
  float sum_x = 0.0;
  //unsigned n_energy_sum_op = 0;
  start_time = stopwatch.now();

  auto source = RColumnSource::MakeSourceRaw("/dev/shm/test.toy");



  auto end_time = stopwatch.now();
  auto diff = end_time - start_time;
  auto milliseconds =
    std::chrono::duration_cast<std::chrono::milliseconds>(diff);
  std::cout << "reading took " << milliseconds.count()
            << " milliseconds,    sum " << sum
            << ",    sum_x " << sum_x
            //<< "   [n_energy_sum_op " << n_energy_sum_op << "]"
            << std::endl;

   return 0;
}
