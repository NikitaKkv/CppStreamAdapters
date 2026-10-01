#include <processing.h>

#include <gtest/gtest.h>

#include <sstream>
#include <vector>

TEST(OutTest, Out) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    std::stringstream file_emulator;
    AsDataFlow(input) | Out(file_emulator);
    ASSERT_EQ(file_emulator.str(), "12345");
}
