#include <RInside.h>
#include "DataStructures/Field.hpp"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Wrong number of arguments!" << std::endl;
  }

  RInside R(argc, argv);

  // Load R util functions
  R.parseEval("source(\"../src/Rfiles/Rfunctions.R\")");

  // Load RDS files
  int iterations = argc[0]; //TODO iteratrions
  std::vector<std::vector<double>> RTempField = R.parseEval("import_data(\"../RData/\", 1)");

  // Test for keras 3 
  if (argc[1] == "keras3") {
    // setup
    R.parseEval("source(\"../src/Rfiles/Rkeras3.R\")");
    
    R.parseEval("model <- initiate_model(\"../src/Rfiles//barite_50ai_all.keras\")");
    R.parseEval("print(gpu_info())");

    // predict + train iterations

  }


  return 0;
} 