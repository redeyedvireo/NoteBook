project(test)
cmake_minimum_required(VERSION 2.8)

find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Sql)
find_package(Qt5Widgets)
find_package(crypto)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_executable(${PROJECT_NAME}  db_creation_tests.cpp
                                db_content_tests.cpp
                                page_tree_tests.cpp
                                mockNoteBook.cpp
                                "../PageTree.cpp"
                                "../Database.cpp"
                                "../Encrypter.cpp"
                                "../PageData.cpp"
                                "../PageCache.cpp"
                                )
target_link_libraries(${PROJECT_NAME} gtest gtest_main
                                      "/usr/lib64/libcrypto.so"
                                      Qt5::Core
                                      Qt5::Gui
                                      Qt5::Widgets
                                      Qt5::Sql)

include_directories ("${PROJECT_SOURCE_DIR}/../../generated_files")
