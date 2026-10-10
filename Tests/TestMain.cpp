#include "TestFramework.h"

int main(int argc, char** argv) {
    int passed = 0;
    int total = static_cast<int>(GetTestRegistry().size());
    std::cout << "[========] Running " << total << " tests.\n";
    for (const auto& test : GetTestRegistry()) {
        std::cout << "[ RUN      ] " << test.name << "\n";
        test.func();
        std::cout << "[       OK ] " << test.name << "\n";
        passed++;
    }
    std::cout << "[========] " << passed << " / " << total << " tests passed.\n";
    return 0;
}

TEST(SampleSanityTest) {
    ASSERT_EQ(1 + 1, 2);
}
