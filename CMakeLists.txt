cmake_minimum_required(VERSION 2.8)

project(text-processor)

set(CMAKE_CXX_FLAGS_DEBUG " -O0 -g")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g")

set(MORPH_ANALYZER_SCRIPT "${CMAKE_SOURCE_DIR}/src/scripts/morph.pyx")
set(BISON_DIR "src/bison")

# ======== custom target for generating cpp source files with cython ====================
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/morph-analyzer.cpp" "${CMAKE_CURRENT_BINARY_DIR}/morph-analyzer.h"
    COMMAND cython ${MORPH_ANALYZER_SCRIPT} -I ${CMAKE_SOURCE_DIR}/src -o "${CMAKE_CURRENT_BINARY_DIR}/morph-analyzer.cpp" -2
    DEPENDS ${MORPH_ANALYZER_SCRIPT}
    COMMENT "Generating cpp source from python code")

add_custom_target(CYTHON_GENERATOR
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/morph-analyzer.cpp" "${CMAKE_CURRENT_BINARY_DIR}/morph-analyzer.h")
# ========================================================================================

# ====== boost ==========
find_package(Boost REQUIRED COMPONENTS program_options)
# =======================

# ====== icu and python =====
find_package(PkgConfig REQUIRED)
pkg_check_modules(ICU REQUIRED icu-uc icu-io)
pkg_check_modules(PYTHON REQUIRED python-2.7)
# ===========================


# ======= flex and bison ========
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(grammar_parser
             "${BISON_DIR}/gparser.yy"
             ${CMAKE_CURRENT_BINARY_DIR}/gparser.tab.cc)
FLEX_TARGET(grammar_lexer
            "${BISON_DIR}/glexer.ll"
            ${CMAKE_CURRENT_BINARY_DIR}/glexer.yy.cc)
ADD_FLEX_BISON_DEPENDENCY(grammar_lexer grammar_parser)
# ===============================

set(SRC
    "src/main.cpp"
    "src/grammar.cpp"
    "src/grammar.h"
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
    "${CMAKE_CURRENT_BINARY_DIR}/morph-analyzer.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/morph-analyzer.h"
    ${FLEX_grammar_lexer_OUTPUTS}
    ${BISON_grammar_parser_OUTPUTS}
    "${BISON_DIR}/g-parser-driver.hpp"
    "${BISON_DIR}/g-parser-driver.cpp"
    "src/action.cpp"
    "src/action.hpp"
    "src/predicate.cpp"
    "src/predicate.hpp"
    "src/grammar-rules-generator.cpp"
    "src/grammar-rules-generator.hpp"
    "src/grammar-words-storage.cpp"
    "src/grammar-words-storage.hpp"
    "src/fields-extractor.cpp"
    "src/fields-extractor.hpp"
    "src/contrib/tinyxml2/tinyxml2.cpp"
    "src/contrib/tinyxml2/tinyxml2.h")

include_directories(src
    ${CMAKE_BINARY_DIR}
    ${ICU_INCLUDE_DIRS}
    ${PYTHON_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${BISON_DIR}
    src/contrib/)
link_directories(${ICU_LIBRARY_DIRS} ${PYTHON_LIBRARY_DIRS})
add_definitions(${ICU_CFLAGS_OTHER} ${PYTHON_CFLAGS_OTHER})

add_executable(${PROJECT_NAME} ${SRC})
add_dependencies(${PROJECT_NAME} CYTHON_GENERATOR)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)

target_link_libraries(${PROJECT_NAME}
    ${ICU_LIBRARIES}
    ${PYTHON_LIBRARIES}
    ${Boost_LIBRARIES})
