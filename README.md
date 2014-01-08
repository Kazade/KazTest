# KazTest

After being frustrated with the difficulty creating test cases in C++ test frameworks in comparison to say Python, I went and wrote my own.

It's dead simple to use, and uses Python to for test discovery so you don't need to register your test cases with crazy macros!

## Usage

To use, simply do the following:
    
 - Create a header file for your tests, I tend to prefix mine with `test_` 
 - In your header file, include `<kaztest/testing.h>`
 - In your header file, create a class that subclasses TestCase
 - Write your tests in methods prefixed with `test_` you can also override `set_up()` and `tear_down()`
 - Hook `kaztest_gen` into your build system. This is a Python file that takes a list of test files as input, and generates a main.cpp file.

For example, to hook `kaztest_gen` into CMake do the following:
    
    ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp 
        COMMAND kaztest_gen ${TEST_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        DEPENDS ${TEST_FILES}
    )

    ADD_EXECUTABLE(my_tests ${TEST_FILES} ${TEST_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
    ADD_TEST(my_suite my_tests)

Where `${TEST_FILES}` is a list of your `test_X.h` test files. You can then run your tests by executing `my_tests` or running ctest

## Dependencies

testing.h makes use of code from the KazBase library. If you use KazTest in your project, your generated test executable will need to depend on the KazBase library.

## TODO:

 - Allow specifying specific test cases, and test methods when running the generated executable (e.g. `./my_tests MyTestCase.test_thing )
 - Make sure the generated executable returns values suitable for ctest (it might already do this)
 - Make discovery of tests more robust
 - Allow specifying a directory to kaztest_gen, which would then discover all files prefixed with `test_` in subdirectories
 - Fix output colouring (doesn't properly reset to the default colour)

