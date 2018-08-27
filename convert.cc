/*
This macro demonstrates how to dump columns of fundamental types and vectors
thereof into forest using rdf.
We achieve this using custom actions.
In order to run it: root -b -q rdf2forest.C
WARNING: enrich your LinkDef with these lines

//#ifdef __CLING__
//#pragma link C++ nestedtypedefs;
//#pragma link C++ nestedclasses;
//#pragma link C++ class ROOT::Experimental::RTree-;
//#pragma link C++ class ROOT::Experimental::RColumnSink-;
//#pragma link C++ class ROOT::Experimental::RColumnRawSettings-;
//#pragma link C++ class ROOT::Experimental::RTreeModel-;
//#pragma link C++ class ROOT::Experimental::RColumn-;
//#pragma link C++ class ROOT::Experimental::RCargo<double>-;
//#pragma link C++ class ROOT::Experimental::RCargo<int>-;
//#endif
*/

#include "ROOT/RColumnStorageFile.hxx"
#include "ROOT/RDataFrame.hxx"
#include "ROOT/RForestDS.hxx"
#include "ROOT/RTree.hxx"
#include "ROOT/RTreeModel.hxx"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <unistd.h>


using RColumnSink = ROOT::Experimental::RColumnSink;
using RColumnRawSettings = ROOT::Experimental::RColumnRawSettings;
using RTree = ROOT::Experimental::RTree;
using RTreeModel = ROOT::Experimental::RTreeModel;
using RColumnSource = ROOT::Experimental::RColumnSource;
using ERangeType = ROOT::Experimental::ERangeType;
using ColNames_t = std::vector<std::string>;

// This is a custom action which respects a well defined interface.
// It does not support parallelism!
// We template it on the type of the columns to be written (forest requires this)
template <typename... ColumnTypes_t>
class RForestHelper : public ROOT::Detail::RDF::RActionImpl<RForestHelper<ColumnTypes_t...>> {
public:
   using Result_t = RTree;
private:
   using ColumnValues_t = std::tuple<std::shared_ptr<ColumnTypes_t>...>;

   std::string fDatasetName;
   ColNames_t fColNames;
   ColumnValues_t fColumnValues;
   static constexpr const auto fNColumns = std::tuple_size<ColumnValues_t>::value;
   std::shared_ptr<RTree> fTree;

   template<std::size_t... S>
   void InitializeImpl(std::index_sequence<S...>) {
      auto event_model = std::make_shared<RTreeModel>();
      std::initializer_list<int> expander {
         (std::get<S>(fColumnValues) = event_model->Branch<ColumnTypes_t>(fColNames[S])
         , 0)...};
      RColumnRawSettings settings(fDatasetName);
      //settings.fCompressionSettings = 104;  // ZLIB, level 4
      fTree = std::make_shared<RTree>(event_model, RColumnSink::MakeSinkRaw(settings));
   }

   template<std::size_t... S>
   void ExecImpl(std::index_sequence<S...>, ColumnTypes_t... values) {
      std::initializer_list<int> expander{
         (*std::get<S>(fColumnValues) = values
         , 0)...};
   }

public:
   RForestHelper(std::string_view datasetName, const ColNames_t& colNames) :
      fDatasetName(datasetName), fColNames(colNames) {
      InitializeImpl(std::make_index_sequence<fNColumns>());
      }

   RForestHelper(RForestHelper &&) = default;

   RForestHelper(const RForestHelper &) = delete;

   std::shared_ptr<RTree> GetResultPtr() const {return fTree;}

   void Initialize() {}

   void InitTask(TTreeReader *, unsigned int) {}

   /// This is a method executed at every entry
   void Exec(unsigned int slot, ColumnTypes_t... values)
   {
      ExecImpl(std::make_index_sequence<fNColumns>(), values...);
      fTree->Fill();
   }

   void Finalize()
   {
      // Do we need to flush data here?
      fTree->Commit();
   }
};


int main(int argc, char** argv) {
   std::string inputFile;
   std::string outputFile;
   std::string treeName;

   int c;
   while ((c = getopt(argc, argv, "i:t:o:")) != -1) {
      switch (c) {
      case 'i':
         inputFile = optarg;
         break;
      case 'o':
         outputFile = optarg;
         break;
      case 't':
         treeName = optarg;
         break;
      default:
         fprintf(stderr, "Unknown option: -%c\n", c);
         return 1;
      }
   }

   //ROOT::RDF::RSnapshotOptions snapshotOptions;
   //snapshotOptions.fCompressionAlgorithm = ROOT::kZLIB;
   //snapshotOptions.fCompressionLevel = 4;
   ROOT::RDataFrame rdf(treeName, inputFile);

   ColNames_t colNames = {
     "B_FlightDistance",
     "B_VertexChi2",
     "H1_PX",
     "H1_PY",
     "H1_PZ",
     "H1_ProbK",
     "H1_ProbPi",
     "H1_Charge",
     "H1_isMuon",
     "H1_IpChi2",
     "H2_PX",
     "H2_PY",
     "H2_PZ",
     "H2_ProbK",
     "H2_ProbPi",
     "H2_Charge",
     "H2_isMuon",
     "H2_IpChi2",
     "H3_PX",
     "H3_PY",
     "H3_PZ",
     "H3_ProbK",
     "H3_ProbPi",
     "H3_Charge",
     "H3_isMuon",
     "H3_IpChi2"
     };
   using Helper_t = RForestHelper<
     double, double,
     double,
     double,
     double,
     double,
     double,
     int,
     int,
     double,
     double,
     double,
     double,
     double,
     double,
     int,
     int,
     double,
     double,
     double,
     double,
     double,
     double,
     int,
     int,
     double>;

   Helper_t helper {outputFile, colNames};
   auto myRTree = rdf.Book<
     double, double,
     double,
     double,
     double,
     double,
     double,
     int,
     int,
     double,
     double,
     double,
     double,
     double,
     double,
     int,
     int,
     double,
     double,
     double,
     double,
     double,
     double,
     int,
     int,
     double>(std::move(helper), colNames);

   *myRTree;

   //rdf.Snapshot(treeName, outputFile, "" /* all columns */, snapshotOptions);
}