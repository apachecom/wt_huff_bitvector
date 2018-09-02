cmake_minimum_required(VERSION 3.8)
project(CDSProject)

set(CMAKE_CXX_STANDARD 11)


# Location of additional cmake modules
#
set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        ${PROJECT_SOURCE_DIR}/cmake)

# Guard against in-source builds and bad build-type strings
#
include(ConfigSafeGuards)



#Testing
include(CTest)
#set(LIBS string_dictionaries)

find_package(Threads REQUIRED)
find_package(GTest)
if (GTEST_FOUND)
    set(LIBS ${LIBS} GTest::Main)
else ()

    if (CMAKE_COMPILER_IS_GNUCXX)
        add_definitions(-Wall -ansi -Wno-deprecated -pthread)
    endif ()

    if (MSVC)
        #vc 2012 fix for vararg templates
        set(MSVC_COMPILER_DEFS "-D_VARIADIC_MAX=10")
    endif ()

    include(ConfigGTest)
    set(LIBS ${LIBS} gtest gtest_main)
endif ()



set(SOURCE_FILES main.cpp
        huff_vector.h huff_vector.cpp
        utils/pair_coder.h utils/pair_coder.cpp

        )
add_executable(CDSProject ${SOURCE_FILES})




include(internal_utils)
cxx_test_with_flags(first_test "" "${LIBS};sdsl" tests/first_test.cpp )
cxx_test_with_flags(wt_huff_test "" "${LIBS};sdsl" tests/wt_huff_test.cpp )
cxx_test_with_flags(pair_coder_test "" "${LIBS};sdsl" tests/pair_coder_test.cpp )
cxx_test_with_flags(huff_vector_test "" "${LIBS};sdsl" tests/huff_vector_test.cpp )


target_link_libraries(CDSProject sdsl)