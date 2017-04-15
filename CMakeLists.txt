cmake_minimum_required(VERSION 2.8)

project(text-processor)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

set(SRC "src/main.cpp"
    "src/grammar.cpp"
    "src/grammar.h"
    "src/grammar-parser.h"
    "src/grammar-parser.cpp"
    "src/grammar-rule.cpp"
    "src/grammar-rule.h"
    "src/utils/converter.h"
    "src/utils/converter.cpp"
    "src/utils/logger.cpp"
    "src/utils/logger.h"
    "src/lr0-items.cpp"
    "src/lr0-items.h"
    "src/global-defs.h"
    "src/utils/string-helper.h"
    "src/utils/string-helper.cpp")

find_package(PkgConfig REQUIRED)
pkg_check_modules(DEPS REQUIRED icu-uc icu-io)

include_directories(src ${DEPS_INCLUDE_DIRS})
link_directories(${DEPS_LIBRARY_DIRS})
add_definitions(${DEPS_CFLAGS_OTHER})

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} ${DEPS_LIBRARIES})
