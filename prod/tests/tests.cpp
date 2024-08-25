#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include "core.hpp"

using namespace testing;
using namespace app;

namespace test {

class TestProcessUnit final: public IProcessing {

};

class CoreTest: public ::testing::Test {
public:
    CoreTest():core_(std::make_shared<TestProcessUnit>()){}
protected:
    Core core_;
};

TEST(CoreTest, constructor) {

}

} //namespace test

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

