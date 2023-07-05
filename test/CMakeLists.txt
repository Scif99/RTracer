add_executable(unit_tests math_tests.cpp)


#Let the compiler know where the header files are located
target_include_directories(unit_tests
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/source/include/
)

target_link_libraries(unit_tests
    gtest_main gmock_main
)

#execute GoogleTest CMake script 
#define a common set of functions or macros in the calling script. 
include(GoogleTest)

#The above command defines the following function 
#This function automatically scans for and adds tests 
gtest_discover_tests(unit_tests) 