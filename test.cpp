#include <iostream>
#include "vectorize.h"
using namespace vectorize;

int main(int argc, char* argv[]) {
  unsigned numValues;
  std::cin >> numValues;
  float* values = new float[numValues];

  for (unsigned i=0; i<numValues; ++i)
    std::cin >> values[i];

  apply(numValues, values, values, sqrt(_x));

  for (unsigned i=0; i<numValues; ++i) {
    std::cout << values[i]
              << ((i+1 < numValues) ? ", " : "\n");
  }

  delete [] values;
      
  return 0;
}
