cmake_minimum_required(VERSION 3.10)

project(bnp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

option(BNP_BUILD_TOOLS "Build tools, instead of just the library" ON)

add_library(
    bnp OBJECT
    include/bnp/bnp.h source/bnp/bnp.cpp
    source/test/test.h source/test/test.cpp
)

target_include_directories(bnp PUBLIC include)


if(BNP_BUILD_TOOLS)
    include(FetchContent)
    FetchContent_Declare(
        Boost
        GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_TAG boost-1.86.0
        GIT_PROGRESS ON
        GIT_SHALLOW TRUE
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(Boost)
    
    add_executable(
        bnp_viewer
        source/viewer/bnp_viewer.cpp
    )

    set_property(TARGET bnp_viewer PROPERTY CXX_STANDARD 20)
        
    target_link_libraries(bnp_viewer PRIVATE bnp)
    target_link_libraries(bnp_viewer PRIVATE Boost::uuid Boost::lexical_cast)

    add_executable(
        tests
        source/test/test_main.cpp
    )

    target_link_libraries(tests PRIVATE bnp)

    enable_testing()

    add_test(
        NAME tests
        COMMAND tests
    )
endif()
