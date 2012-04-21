#include "vectorize.h"
#include "gtest/gtest.h"
#include <vector>
using namespace vectorize;
using std::vector;

class Apply2 : public ::testing::Test {
public:
  Apply2() :
    inputs1(generate_inputs1()),
    inputs2(generate_inputs2()),
    expected(inputs1.size()),
    actual(inputs1.size()) {
  }
protected:
  const vector<float> inputs1, inputs2;
  vector<float> expected;
  vector<float> actual;
private:
  static vector<float> generate_inputs1() {
    vector<float> newinputs(65);
    for (unsigned i=0; i<newinputs.size(); ++i) {
      newinputs[i] = 50.0f * (static_cast<float>(i) - 20);
    }
    return newinputs;
  }
  static vector<float> generate_inputs2() {
    vector<float> newinputs(65);
    for (unsigned i=0; i<newinputs.size(); ++i) {
      newinputs[i] = 200.0f * (static_cast<float>(i) + 5);
    }
    return newinputs;
  }
};


TEST_F(Apply2, Add) {
  apply2(inputs1.size(), &inputs1[0], &inputs2[0], &actual[0], _x + _y);
  for (unsigned i=0; i<inputs1.size(); ++i) {
    expected[i] = inputs1[i] + inputs2[i];
  }
  for (unsigned i=0; i<inputs1.size(); ++i) {
    EXPECT_FLOAT_EQ(expected[i], actual[i])
      << "index=" << i
      << ", input1=" << inputs1[i]
      << ", input2=" << inputs2[i];
  }
}
