# ===================================================================
# parser のための CMakeLists.txt
# ===================================================================

# ===================================================================
# CMAKE のおまじない
# ===================================================================
cmake_minimum_required (VERSION 3.2)


# ===================================================================
# プロジェクト名，バージョンの設定
# ===================================================================
project (parser)
set (PARSER_VERSION_MAJOR 1)
set (PARSER_VERSION_MINOR 0)
set (PARSER_VERSION "${PARSER_VERSION_MAJOR}.${PARSER_VERSION_MINOR}")


# ===================================================================
# CTest モジュールの読み込み
# ===================================================================
include (CTest)


# ===================================================================
# パッケージの検査
# ===================================================================

find_package (GTest)

find_package(YmTools REQUIRED)


# ===================================================================
# サブディレクトリの設定
# ===================================================================

if (GTEST_FOUND)
  add_subdirectory(gtest)
endif (GTEST_FOUND)


# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_BINARY_DIR}
  ${YmTools_INCLUDE_DIRS}
  )


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set (parser_SOURCES
  src/Grammer.cc
  src/LALR1Set.cc
  src/LR0Set.cc
  src/LR0State.cc
  src/LR0Term.cc
  src/LR1Term.cc
  src/Rule.cc
  src/Token.cc
  )


# ===================================================================
#  ターゲットの設定
# ===================================================================

add_library(parser
  ${parser_SOURCES}
  )

if (GPERFTOOLS_FOUND)
  target_link_libraries(parser
    ${GPERFTOOLS_LIBRARIES}
    )
endif (GPERFTOOLS_FOUND)

add_executable(Grammer_test
  tests/Grammer_test.cc
  )

target_link_libraries(Grammer_test
  parser
  ym_utils
  )


# ===================================================================
#  インストールターゲットの設定
# ===================================================================

#install(
#  TARGETS parser
#  DESTINATION lib
#  EXPORT ymtools-targets
#  )

#install(
#  DIRECTORY YmUtils
#  DESTINATION include
#  )
