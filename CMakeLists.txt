cmake_minimum_required(VERSION 2.8)

project(text-processor)

set(CMAKE_CXX_FLAGS_DEBUG " -O0 -g")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g")

set(MORPH_ANALYZER_SCRIPT "${CMAKE_SOURCE_DIR}/src/scripts/morph.pyx")
set(GENERATED_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src/generated")

add_custom_command(OUTPUT "${GENERATED_SOURCE_DIR}/morph-analyzer.cpp" "${GENERATED_SOURCE_DIR}/morh-analyzer.h"
    COMMAND mkdir -p ${GENERATED_SOURCE_DIR}
    COMMAND cython ${MORPH_ANALYZER_SCRIPT} -I ${CMAKE_SOURCE_DIR}/src -o ${GENERATED_SOURCE_DIR}/morph-analyzer.cpp -2
    DEPENDS ${MORPH_ANALYZER_SCRIPT}
    COMMENT "Generating cpp source from python code")

add_custom_target(RunGenerator
    DEPENDS ${MORPH_ANALYZER_SCRIPT}
    COMMENT "Checking if re-generation is required")

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
    "src/utils/string-helper.cpp"
    "src/parser-table.cpp"
    "src/parser-table.h"
    "src/parser.cpp"
    "src/parser.h"
    "src/tokenizer.cpp"
    "src/tokenizer.h"
    "src/stack.cpp"
    "src/stack.h"
    "${GENERATED_SOURCE_DIR}/morph-analyzer.cpp"
    "${GENERATED_SOURCE_DIR}/morph-analyzer.h")

find_package(PkgConfig REQUIRED)
pkg_check_modules(ICU REQUIRED icu-uc icu-io)
pkg_check_modules(PYTHON REQUIRED python-2.7)

include_directories(src ${GENERATED_SOURCE_DIR} ${ICU_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
link_directories(${ICU_LIBRARY_DIRS} ${PYTHON_LIBRARY_DIRS})
add_definitions(${ICU_CFLAGS_OTHER} ${PYTHON_CFLAGS_OTHER})

add_executable(${PROJECT_NAME} ${SRC})
add_dependencies(${PROJECT_NAME} RunGenerator)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)

target_link_libraries(${PROJECT_NAME} ${ICU_LIBRARIES} ${PYTHON_LIBRARIES})
