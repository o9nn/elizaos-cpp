#pragma once
/**
 * @file gtest.h
 * @brief Google Test compatibility header for ElizaOS C++
 * 
 * This header provides a lightweight implementation of Google Test macros
 * for compilation purposes. For actual testing, link against the real
 * Google Test library.
 * 
 * To use real Google Test:
 *   - Install: sudo apt-get install libgtest-dev
 *   - Or use CMake FetchContent to download googletest
 *   - Then remove or exclude this header from include path
 */

#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace testing {

// ============================================================================
// Test Result Tracking
// ============================================================================

class TestResult {
public:
    bool passed = true;
    std::vector<std::string> failures;
    
    void addFailure(const std::string& msg) {
        passed = false;
        failures.push_back(msg);
    }
};

// ============================================================================
// Test Info
// ============================================================================

class TestInfo {
public:
    std::string test_suite_name;
    std::string test_name;
    std::function<void()> test_func;
    TestResult result;
    
    TestInfo(const std::string& suite, const std::string& name, std::function<void()> func)
        : test_suite_name(suite), test_name(name), test_func(func) {}
};

// ============================================================================
// Test Registry (Singleton)
// ============================================================================

class TestRegistry {
public:
    static TestRegistry& instance() {
        static TestRegistry registry;
        return registry;
    }
    
    void registerTest(std::shared_ptr<TestInfo> test) {
        tests_.push_back(test);
    }
    
    int runAllTests() {
        int passed = 0;
        int failed = 0;
        
        std::cout << "[==========] Running " << tests_.size() << " tests.\n";
        
        for (auto& test : tests_) {
            std::cout << "[ RUN      ] " << test->test_suite_name << "." << test->test_name << "\n";
            
            current_test_ = test.get();
            try {
                test->test_func();
            } catch (const std::exception& e) {
                test->result.addFailure(std::string("Exception: ") + e.what());
            } catch (...) {
                test->result.addFailure("Unknown exception");
            }
            current_test_ = nullptr;
            
            if (test->result.passed) {
                std::cout << "[       OK ] " << test->test_suite_name << "." << test->test_name << "\n";
                passed++;
            } else {
                std::cout << "[  FAILED  ] " << test->test_suite_name << "." << test->test_name << "\n";
                for (const auto& failure : test->result.failures) {
                    std::cout << "    " << failure << "\n";
                }
                failed++;
            }
        }
        
        std::cout << "[==========] " << tests_.size() << " tests ran.\n";
        std::cout << "[  PASSED  ] " << passed << " tests.\n";
        if (failed > 0) {
            std::cout << "[  FAILED  ] " << failed << " tests.\n";
        }
        
        return failed > 0 ? 1 : 0;
    }
    
    TestInfo* currentTest() { return current_test_; }
    
private:
    TestRegistry() = default;
    std::vector<std::shared_ptr<TestInfo>> tests_;
    TestInfo* current_test_ = nullptr;
};

// ============================================================================
// Test Registration Helper
// ============================================================================

class TestRegistrar {
public:
    TestRegistrar(const std::string& suite, const std::string& name, std::function<void()> func) {
        auto test = std::make_shared<TestInfo>(suite, name, func);
        TestRegistry::instance().registerTest(test);
    }
};

// ============================================================================
// Assertion Helpers
// ============================================================================

inline void recordFailure(const std::string& file, int line, const std::string& msg) {
    std::ostringstream oss;
    oss << file << ":" << line << ": " << msg;
    auto* test = TestRegistry::instance().currentTest();
    if (test) {
        test->result.addFailure(oss.str());
    }
    std::cerr << oss.str() << "\n";
}

// ============================================================================
// Test Base Class
// ============================================================================

class Test {
public:
    virtual ~Test() = default;
    virtual void SetUp() {}
    virtual void TearDown() {}
    virtual void TestBody() = 0;
    
    void Run() {
        SetUp();
        TestBody();
        TearDown();
    }
};

// ============================================================================
// Initialization
// ============================================================================

inline void InitGoogleTest(int* argc, char** argv) {
    // Parse command line arguments if needed
    (void)argc;
    (void)argv;
}

// ============================================================================
// Run All Tests
// ============================================================================

inline int RUN_ALL_TESTS() {
    return TestRegistry::instance().runAllTests();
}

// ============================================================================
// AssertionResult for EXPECT_* and ASSERT_*
// ============================================================================

class AssertionResult {
public:
    explicit AssertionResult(bool success) : success_(success) {}
    
    operator bool() const { return success_; }
    
    AssertionResult& operator<<(const std::string& msg) {
        message_ += msg;
        return *this;
    }
    
    template<typename T>
    AssertionResult& operator<<(const T& val) {
        std::ostringstream oss;
        oss << val;
        message_ += oss.str();
        return *this;
    }
    
    std::string message() const { return message_; }
    
private:
    bool success_;
    std::string message_;
};

inline AssertionResult AssertionSuccess() { return AssertionResult(true); }
inline AssertionResult AssertionFailure() { return AssertionResult(false); }

} // namespace testing

// ============================================================================
// Test Definition Macros
// ============================================================================

#define TEST(test_suite, test_name) \
    void test_suite##_##test_name##_TestBody(); \
    static ::testing::TestRegistrar test_suite##_##test_name##_registrar( \
        #test_suite, #test_name, test_suite##_##test_name##_TestBody); \
    void test_suite##_##test_name##_TestBody()

#define TEST_F(test_fixture, test_name) \
    class test_fixture##_##test_name##_Test : public test_fixture { \
    public: \
        void TestBody() override; \
    }; \
    static ::testing::TestRegistrar test_fixture##_##test_name##_registrar( \
        #test_fixture, #test_name, []() { \
            test_fixture##_##test_name##_Test test; \
            test.Run(); \
        }); \
    void test_fixture##_##test_name##_Test::TestBody()

// ============================================================================
// Assertion Macros - EXPECT (non-fatal)
// ============================================================================

#define EXPECT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            ::testing::recordFailure(__FILE__, __LINE__, \
                "Expected: " #condition " to be true"); \
        } \
    } while (0)

#define EXPECT_FALSE(condition) \
    do { \
        if (condition) { \
            ::testing::recordFailure(__FILE__, __LINE__, \
                "Expected: " #condition " to be false"); \
        } \
    } while (0)

#define EXPECT_EQ(val1, val2) \
    do { \
        const auto& _v1 = (val1); \
        const auto& _v2 = (val2); \
        if (!(_v1 == _v2)) { \
            std::ostringstream _oss; \
            _oss << "Expected: " #val1 " == " #val2; \
            ::testing::recordFailure(__FILE__, __LINE__, _oss.str()); \
        } \
    } while (0)

#define EXPECT_NE(val1, val2) \
    do { \
        const auto& _v1 = (val1); \
        const auto& _v2 = (val2); \
        if (!(_v1 != _v2)) { \
            std::ostringstream _oss; \
            _oss << "Expected: " #val1 " != " #val2; \
            ::testing::recordFailure(__FILE__, __LINE__, _oss.str()); \
        } \
    } while (0)

#define EXPECT_LT(val1, val2) \
    do { \
        const auto& _v1 = (val1); \
        const auto& _v2 = (val2); \
        if (!(_v1 < _v2)) { \
            std::ostringstream _oss; \
            _oss << "Expected: " #val1 " < " #val2; \
            ::testing::recordFailure(__FILE__, __LINE__, _oss.str()); \
        } \
    } while (0)

#define EXPECT_LE(val1, val2) \
    do { \
        const auto& _v1 = (val1); \
        const auto& _v2 = (val2); \
        if (!(_v1 <= _v2)) { \
            std::ostringstream _oss; \
            _oss << "Expected: " #val1 " <= " #val2; \
            ::testing::recordFailure(__FILE__, __LINE__, _oss.str()); \
        } \
    } while (0)

#define EXPECT_GT(val1, val2) \
    do { \
        const auto& _v1 = (val1); \
        const auto& _v2 = (val2); \
        if (!(_v1 > _v2)) { \
            std::ostringstream _oss; \
            _oss << "Expected: " #val1 " > " #val2; \
            ::testing::recordFailure(__FILE__, __LINE__, _oss.str()); \
        } \
    } while (0)

#define EXPECT_GE(val1, val2) \
    do { \
        const auto& _v1 = (val1); \
        const auto& _v2 = (val2); \
        if (!(_v1 >= _v2)) { \
            std::ostringstream _oss; \
            _oss << "Expected: " #val1 " >= " #val2; \
            ::testing::recordFailure(__FILE__, __LINE__, _oss.str()); \
        } \
    } while (0)

#define EXPECT_STREQ(str1, str2) \
    do { \
        std::string _s1 = (str1) ? (str1) : ""; \
        std::string _s2 = (str2) ? (str2) : ""; \
        if (_s1 != _s2) { \
            std::ostringstream _oss; \
            _oss << "Expected: " #str1 " == " #str2 " (\"" << _s1 << "\" vs \"" << _s2 << "\")"; \
            ::testing::recordFailure(__FILE__, __LINE__, _oss.str()); \
        } \
    } while (0)

#define EXPECT_STRNE(str1, str2) \
    do { \
        std::string _s1 = (str1) ? (str1) : ""; \
        std::string _s2 = (str2) ? (str2) : ""; \
        if (_s1 == _s2) { \
            std::ostringstream _oss; \
            _oss << "Expected: " #str1 " != " #str2 " (both are \"" << _s1 << "\")"; \
            ::testing::recordFailure(__FILE__, __LINE__, _oss.str()); \
        } \
    } while (0)

#define EXPECT_NEAR(val1, val2, abs_error) \
    do { \
        const auto& _v1 = (val1); \
        const auto& _v2 = (val2); \
        auto _err = (abs_error); \
        if (std::abs(_v1 - _v2) > _err) { \
            std::ostringstream _oss; \
            _oss << "Expected: |" #val1 " - " #val2 "| <= " #abs_error \
                 << " (|" << _v1 << " - " << _v2 << "| = " << std::abs(_v1 - _v2) << ")"; \
            ::testing::recordFailure(__FILE__, __LINE__, _oss.str()); \
        } \
    } while (0)

#define EXPECT_DOUBLE_EQ(val1, val2) EXPECT_NEAR(val1, val2, 1e-9)
#define EXPECT_FLOAT_EQ(val1, val2) EXPECT_NEAR(val1, val2, 1e-5f)

#define EXPECT_THROW(statement, exception_type) \
    do { \
        bool _caught = false; \
        try { \
            statement; \
        } catch (const exception_type&) { \
            _caught = true; \
        } catch (...) { \
            ::testing::recordFailure(__FILE__, __LINE__, \
                "Expected: " #statement " throws " #exception_type ", but threw different exception"); \
        } \
        if (!_caught) { \
            ::testing::recordFailure(__FILE__, __LINE__, \
                "Expected: " #statement " throws " #exception_type ", but it didn't throw"); \
        } \
    } while (0)

#define EXPECT_NO_THROW(statement) \
    do { \
        try { \
            statement; \
        } catch (const std::exception& e) { \
            std::ostringstream _oss; \
            _oss << "Expected: " #statement " doesn't throw, but threw: " << e.what(); \
            ::testing::recordFailure(__FILE__, __LINE__, _oss.str()); \
        } catch (...) { \
            ::testing::recordFailure(__FILE__, __LINE__, \
                "Expected: " #statement " doesn't throw, but threw unknown exception"); \
        } \
    } while (0)

// ============================================================================
// Assertion Macros - ASSERT (fatal)
// ============================================================================

#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            ::testing::recordFailure(__FILE__, __LINE__, \
                "Assertion failed: " #condition " is not true"); \
            return; \
        } \
    } while (0)

#define ASSERT_FALSE(condition) \
    do { \
        if (condition) { \
            ::testing::recordFailure(__FILE__, __LINE__, \
                "Assertion failed: " #condition " is not false"); \
            return; \
        } \
    } while (0)

#define ASSERT_EQ(val1, val2) \
    do { \
        const auto& _v1 = (val1); \
        const auto& _v2 = (val2); \
        if (!(_v1 == _v2)) { \
            std::ostringstream _oss; \
            _oss << "Assertion failed: " #val1 " == " #val2 " (" << _v1 << " vs " << _v2 << ")"; \
            ::testing::recordFailure(__FILE__, __LINE__, _oss.str()); \
            return; \
        } \
    } while (0)

#define ASSERT_NE(val1, val2) \
    do { \
        const auto& _v1 = (val1); \
        const auto& _v2 = (val2); \
        if (!(_v1 != _v2)) { \
            std::ostringstream _oss; \
            _oss << "Assertion failed: " #val1 " != " #val2 " (" << _v1 << " vs " << _v2 << ")"; \
            ::testing::recordFailure(__FILE__, __LINE__, _oss.str()); \
            return; \
        } \
    } while (0)

#define ASSERT_LT(val1, val2) \
    do { \
        const auto& _v1 = (val1); \
        const auto& _v2 = (val2); \
        if (!(_v1 < _v2)) { \
            std::ostringstream _oss; \
            _oss << "Assertion failed: " #val1 " < " #val2 " (" << _v1 << " vs " << _v2 << ")"; \
            ::testing::recordFailure(__FILE__, __LINE__, _oss.str()); \
            return; \
        } \
    } while (0)

#define ASSERT_LE(val1, val2) \
    do { \
        const auto& _v1 = (val1); \
        const auto& _v2 = (val2); \
        if (!(_v1 <= _v2)) { \
            std::ostringstream _oss; \
            _oss << "Assertion failed: " #val1 " <= " #val2 " (" << _v1 << " vs " << _v2 << ")"; \
            ::testing::recordFailure(__FILE__, __LINE__, _oss.str()); \
            return; \
        } \
    } while (0)

#define ASSERT_GT(val1, val2) \
    do { \
        const auto& _v1 = (val1); \
        const auto& _v2 = (val2); \
        if (!(_v1 > _v2)) { \
            std::ostringstream _oss; \
            _oss << "Assertion failed: " #val1 " > " #val2 " (" << _v1 << " vs " << _v2 << ")"; \
            ::testing::recordFailure(__FILE__, __LINE__, _oss.str()); \
            return; \
        } \
    } while (0)

#define ASSERT_GE(val1, val2) \
    do { \
        const auto& _v1 = (val1); \
        const auto& _v2 = (val2); \
        if (!(_v1 >= _v2)) { \
            std::ostringstream _oss; \
            _oss << "Assertion failed: " #val1 " >= " #val2 " (" << _v1 << " vs " << _v2 << ")"; \
            ::testing::recordFailure(__FILE__, __LINE__, _oss.str()); \
            return; \
        } \
    } while (0)

#define ASSERT_STREQ(str1, str2) \
    do { \
        std::string _s1 = (str1) ? (str1) : ""; \
        std::string _s2 = (str2) ? (str2) : ""; \
        if (_s1 != _s2) { \
            std::ostringstream _oss; \
            _oss << "Assertion failed: " #str1 " == " #str2 " (\"" << _s1 << "\" vs \"" << _s2 << "\")"; \
            ::testing::recordFailure(__FILE__, __LINE__, _oss.str()); \
            return; \
        } \
    } while (0)

#define ASSERT_NEAR(val1, val2, abs_error) \
    do { \
        const auto& _v1 = (val1); \
        const auto& _v2 = (val2); \
        auto _err = (abs_error); \
        if (std::abs(_v1 - _v2) > _err) { \
            std::ostringstream _oss; \
            _oss << "Assertion failed: |" #val1 " - " #val2 "| <= " #abs_error \
                 << " (|" << _v1 << " - " << _v2 << "| = " << std::abs(_v1 - _v2) << ")"; \
            ::testing::recordFailure(__FILE__, __LINE__, _oss.str()); \
            return; \
        } \
    } while (0)

#define ASSERT_THROW(statement, exception_type) \
    do { \
        bool _caught = false; \
        try { \
            statement; \
        } catch (const exception_type&) { \
            _caught = true; \
        } catch (...) { \
            ::testing::recordFailure(__FILE__, __LINE__, \
                "Assertion failed: " #statement " throws " #exception_type ", but threw different exception"); \
            return; \
        } \
        if (!_caught) { \
            ::testing::recordFailure(__FILE__, __LINE__, \
                "Assertion failed: " #statement " throws " #exception_type ", but it didn't throw"); \
            return; \
        } \
    } while (0)

#define ASSERT_NO_THROW(statement) \
    do { \
        try { \
            statement; \
        } catch (const std::exception& e) { \
            std::ostringstream _oss; \
            _oss << "Assertion failed: " #statement " doesn't throw, but threw: " << e.what(); \
            ::testing::recordFailure(__FILE__, __LINE__, _oss.str()); \
            return; \
        } catch (...) { \
            ::testing::recordFailure(__FILE__, __LINE__, \
                "Assertion failed: " #statement " doesn't throw, but threw unknown exception"); \
            return; \
        } \
    } while (0)

// ============================================================================
// Additional Macros
// ============================================================================

#define SUCCEED() ((void)0)

#define FAIL() \
    do { \
        ::testing::recordFailure(__FILE__, __LINE__, "Explicit FAIL()"); \
        return; \
    } while (0)

#define ADD_FAILURE() \
    ::testing::recordFailure(__FILE__, __LINE__, "Explicit ADD_FAILURE()")

#define GTEST_SKIP() return

// Typed test support (minimal)
#define TYPED_TEST_SUITE(test_suite, types) /* no-op for now */
#define TYPED_TEST(test_suite, test_name) TEST(test_suite, test_name)

// Parameterized test support (minimal)
#define INSTANTIATE_TEST_SUITE_P(prefix, test_suite, ...) /* no-op for now */
#define TEST_P(test_suite, test_name) TEST(test_suite, test_name)

// Value-parameterized tests
namespace testing {
    template<typename T>
    class WithParamInterface {
    public:
        const T& GetParam() const { return param_; }
    protected:
        T param_;
    };
    
    template<typename... Args>
    auto Values(Args... args) {
        return std::vector<std::common_type_t<Args...>>{args...};
    }
    
    template<typename T>
    auto Range(T start, T end, T step = 1) {
        std::vector<T> result;
        for (T i = start; i < end; i += step) {
            result.push_back(i);
        }
        return result;
    }
}
