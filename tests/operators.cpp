#include "vectorize.h"
#include "gtest/gtest.h"
#include <vector>
using namespace vectorize;
using std::vector;

class Operators : public ::testing::Test {
public:
  Operators() :
    inputs(generateinputs()),
    expected(inputs.size()),
    actual(inputs.size()) {
  }
  virtual ~Operators() {
  }
protected:
  const vector<float> inputs;
  vector<float> expected;
  vector<float> actual;
private:
  static vector<float> generateinputs() {
    vector<float> newinputs(65);
    for (unsigned i=0; i<newinputs.size(); ++i) {
      newinputs[i] = 50.f * (static_cast<float>(i) - 20);
    }
    return newinputs;
  }
};

TEST_F(Operators, Add) {
  apply(inputs.size(), &inputs[0], &actual[0], _x + 4.0f);
  for (unsigned i=0; i<inputs.size(); ++i) {
    expected[i] = inputs[i] + 4.0f;
  }
  for (unsigned i=0; i<inputs.size(); ++i) {
    EXPECT_FLOAT_EQ(expected[i], actual[i])
      << "index=" << i
      << ", input=" << inputs[i];
  }
}
