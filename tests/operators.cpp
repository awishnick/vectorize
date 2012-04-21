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

TEST_F(Operators, Sub) {
  apply(inputs.size(), &inputs[0], &actual[0], _x - 4.0f);
  for (unsigned i=0; i<inputs.size(); ++i) {
    expected[i] = inputs[i] - 4.0f;
  }
  for (unsigned i=0; i<inputs.size(); ++i) {
    EXPECT_FLOAT_EQ(expected[i], actual[i])
      << "index=" << i
      << ", input=" << inputs[i];
  }
}

TEST_F(Operators, Mul) {
  apply(inputs.size(), &inputs[0], &actual[0], _x * 4.0f);
  for (unsigned i=0; i<inputs.size(); ++i) {
    expected[i] = inputs[i] * 4.0f;
  }
  for (unsigned i=0; i<inputs.size(); ++i) {
    EXPECT_FLOAT_EQ(expected[i], actual[i])
      << "index=" << i
      << ", input=" << inputs[i];
  }
}

TEST_F(Operators, Max) {
  apply(inputs.size(), &inputs[0], &actual[0], max(_x, 4.0f));
  for (unsigned i=0; i<inputs.size(); ++i) {
    expected[i] = std::max(inputs[i], 4.0f);
  }
  for (unsigned i=0; i<inputs.size(); ++i) {
    EXPECT_FLOAT_EQ(expected[i], actual[i])
      << "index=" << i
      << ", input=" << inputs[i];
  }
}

TEST_F(Operators, Min) {
  apply(inputs.size(), &inputs[0], &actual[0], min(_x, 4.0f));
  for (unsigned i=0; i<inputs.size(); ++i) {
    expected[i] = std::min(inputs[i], 4.0f);
  }
  for (unsigned i=0; i<inputs.size(); ++i) {
    EXPECT_FLOAT_EQ(expected[i], actual[i])
      << "index=" << i
      << ", input=" << inputs[i];
  }
}

TEST_F(Operators, Abs) {
  apply(inputs.size(), &inputs[0], &actual[0], abs(_x));
  for (unsigned i=0; i<inputs.size(); ++i) {
    expected[i] = std::abs(inputs[i]);
  }
  for (unsigned i=0; i<inputs.size(); ++i) {
    EXPECT_FLOAT_EQ(expected[i], actual[i])
      << "index=" << i
      << ", input=" << inputs[i];
  }
}

TEST_F(Operators, Sqrt) {
  apply(inputs.size(), &inputs[0], &actual[0], sqrt(_x));
  for (unsigned i=0; i<inputs.size(); ++i) {
    expected[i] = std::sqrt(inputs[i]);
  }
  for (unsigned i=0; i<inputs.size(); ++i) {
    // Negative numbers should be NaN
    if (inputs[i] < 0) {
      EXPECT_TRUE(actual[i] != actual[i])
        << "Expected NaN. "
        << "index=" << i
        << ", input=" << inputs[i];
      continue;
    }

    EXPECT_FLOAT_EQ(expected[i], actual[i])
      << "index=" << i
      << ", input=" << inputs[i];
  }
}
