#ifndef KAZTEST_H
#define KAZTEST_H

#include <vector>
#include <functional>
#include <stdexcept>
#include <iostream>

#include <kazbase/unicode.h>
#include <kazbase/exceptions.h>
#include <kazbase/logging.h>
#include <kazbase/file_utils.h>

#define assert_equal(expected, actual) _assert_equal((expected), (actual), __FILE__, __LINE__)
#define assert_false(actual) _assert_false((actual), __FILE__, __LINE__)
#define assert_true(actual) _assert_true((actual), __FILE__, __LINE__)
#define assert_close(expected, actual, difference) _assert_close((expected), (actual), (difference), __FILE__, __LINE__)
#define assert_is_null(actual) _assert_is_null((actual), __FILE__, __LINE__)
#define assert_is_not_null(actual) _assert_is_not_null((actual), __FILE__, __LINE__)
#define assert_raises(exception, func) _assert_raises<exception>((func), __FILE__, __LINE__)
#define not_implemented() _not_implemented(__FILE__, __LINE__)

class TestCase {
public:
    virtual ~TestCase() {}

    virtual void set_up() {}
    virtual void tear_down() {}

    template<typename T, typename U>
    void _assert_equal(T expected, U actual, unicode file, int line) {
        if(expected != actual) {
            auto file_and_line = std::make_pair(file, line);
            throw AssertionError(file_and_line, unicode("{0} does not match {1}").format(actual, expected).encode());
        }
    }

    template<typename T>
    void _assert_true(T actual, unicode file, int line) {
        if(!bool(actual)) {
            auto file_and_line = std::make_pair(file, line);
            throw AssertionError(file_and_line, unicode("{0} is not true").format(bool(actual) ? "true" : "false").encode());
        }
    }

    template<typename T>
    void _assert_false(T actual, unicode file, int line) {
        if(bool(actual)) {
            auto file_and_line = std::make_pair(file, line);
            throw AssertionError(file_and_line, unicode("{0} is not false").format(bool(actual) ? "true" : "false").encode());
        }
    }

    template<typename T, typename U, typename V>
    void _assert_close(T expected, U actual, V difference, unicode file, int line) {
        if(actual < expected - difference ||
           actual > expected + difference) {
            auto file_and_line = std::make_pair(file, line);
            throw AssertionError(file_and_line, unicode("{0} is not close enough to {1}").format(actual, expected).encode());
        }
    }

    template<typename T>
    void _assert_is_null(T* thing, unicode file, int line) {
        if(thing != nullptr) {
            auto file_and_line = std::make_pair(file, line);
            throw AssertionError(file_and_line, "Pointer was not NULL");
        }
    }

    template<typename T>
    void _assert_is_not_null(T* thing, unicode file, int line) {
        if(thing == nullptr) {
            auto file_and_line = std::make_pair(file, line);
            throw AssertionError(file_and_line, "Pointer was unexpectedly NULL");
        }
    }

    template<typename T, typename Func>
    void _assert_raises(Func func, unicode file, int line) {
        try {
            func();
            auto file_and_line = std::make_pair(file, line);
            throw AssertionError(file_and_line, _u("Expected exception ({0}) was not thrown").format(typeid(T).name()));
        } catch(T& e) {}
    }

    void _not_implemented(unicode file, int line) {
        throw NotImplementedError(file.encode(), line);
    }
};

class TestRunner {
public:
    template<typename T, typename U>
    void register_case(std::vector<U> methods, std::vector<std::string> names) {
        std::shared_ptr<TestCase> instance = std::make_shared<T>();

        instances_.push_back(instance); //Hold on to it

        for(std::string name: names) {
            names_.push_back(name);
        }

        for(U& method: methods) {
            std::function<void()> func = std::bind(method, dynamic_cast<T*>(instance.get()));
            tests_.push_back([=]() {
                instance->set_up();
                func();
                instance->tear_down();
            });
        }
    }

    int32_t run(const std::string& test_case) {
        int failed = 0;
        int skipped = 0;
        int ran = 0;
        int crashed = 0;

        auto new_tests = tests_;
        auto new_names = names_;

        if(!test_case.empty()) {
            new_tests.clear();
            new_names.clear();

            for(int i = 0; i < names_.size(); ++i) {
                if(_u(names_[i]).starts_with(test_case)) {
                    new_tests.push_back(tests_[i]);
                    new_names.push_back(names_[i]);
                }
            }
        }

        std::cout << std::endl << "Running " << new_tests.size() << " tests" << std::endl << std::endl;


        int32_t k = 0;
        for(std::function<void ()> test: new_tests) {
            try {
                std::string output = "    " + new_names[ran];

                for(int i = output.length(); i < 76; ++i) {
                    output += " ";
                }

                std::cout << output;
                test();
                std::cout << "\033[32m" << "   OK   " << "\033[0m" << std::endl;
            } catch(NotImplementedError& e) {
                std::cout << "\033[34m" << " SKIPPED" << "\033[0m" << std::endl;
                ++skipped;
            } catch(AssertionError& e) {
                std::cout << "\033[33m" << " FAILED " << "\033[0m" << std::endl;
                std::cout << "        " << e.what() << std::endl;
                if(!e.file.empty()) {
                    std::cout << "        " << e.file << ":" << e.line << std::endl;
                    if(os::path::exists(e.file)) {
                        using file_utils::read_lines;

                        unicode file = e.file;
                        std::vector<unicode> lines = read_lines(file);
                        int line_count = lines.size();
                        if(line_count && e.line <= line_count) {
                            std::cout << lines.at(e.line - 1).encode() << std::endl << std::endl;
                        }
                    }
                }
                ++failed;
            } catch(std::exception& e) {
                std::cout << "\033[31m" << " EXCEPT " << std::endl;
                std::cout << "        " << e.what() << "\033[0m" << std::endl;
                ++crashed;
            }
            std::cout << "\033[0m";
            ++ran;
        }

        std::cout << "-----------------------" << std::endl;
        if(!failed && !crashed && !skipped) {
            std::cout << "All tests passed" << std::endl << std::endl;
        } else {
            if(skipped) {
                std::cout << skipped << " tests skipped";
            }

            if(failed) {
                if(skipped) {
                    std::cout << ", ";
                }
                std::cout << failed << " tests failed";
            }

            if(crashed) {
                if(failed) {
                    std::cout << ", ";
                }
                std::cout << crashed << " tests crashed";
            }
            std::cout << std::endl << std::endl;
        }

        return failed;
    }

private:
    std::vector<std::shared_ptr<TestCase>> instances_;
    std::vector<std::function<void()> > tests_;
    std::vector<std::string> names_;
};


#endif // KAZTEST_H
