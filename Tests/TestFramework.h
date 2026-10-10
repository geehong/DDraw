#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <iostream>
#include <vector>
#include <string>
#include <functional>

struct TestItem {
    std::string name;
    std::function<void()> func;
};

inline std::vector<TestItem>& GetTestRegistry() {
    static std::vector<TestItem> registry;
    return registry;
}

inline bool RegisterTest(const std::string& name, std::function<void()> func) {
    GetTestRegistry().push_back({ name, func });
    return true;
}

#define TEST(test_name) \
    static void test_name(); \
    struct test_name##_registrar { \
        test_name##_registrar() { RegisterTest(#test_name, test_name); } \
    } test_name##_registrar_instance; \
    static void test_name()

#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        std::cerr << "Assertion failed: (" #condition ") at " << __FILE__ << ":" << __LINE__ << std::endl; \
        std::exit(1); \
    }

#define ASSERT_FALSE(condition) ASSERT_TRUE(!(condition))
#define ASSERT_EQ(val1, val2) ASSERT_TRUE((val1) == (val2))

#endif // TEST_FRAMEWORK_H
