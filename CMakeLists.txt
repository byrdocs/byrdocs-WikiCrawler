cmake_minimum_required(VERSION 3.22)
project(byrdocs_wiki_bot VERSION 0.2.1)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
enable_testing()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
file(GLOB_RECURSE HeaderFileList "include/*")
file(GLOB_RECURSE LibraryFileList "lib/*")
include_directories(include /usr/include /usr/local/include)
link_directories(lib /usr/lib /usr/local/lib)
add_library(wikibot ${HeaderFileList} ${LibraryFileList})
find_package(curlpp NAMES curlpp curl)
find_package(nlohmann_json 3.11.3 REQUIRED)
target_link_libraries(wikibot PUBLIC nlohmann_json::nlohmann_json curlpp curl)
add_executable(.get test/get.cpp)
target_link_libraries(.get wikibot)
add_test(NAME get COMMAND .get)
add_executable(.raw test/raw.cpp)
target_link_libraries(.raw wikibot)
add_test(NAME raw COMMAND .raw)
add_executable(.query_main_page test/query_main_page.cpp)
target_link_libraries(.query_main_page wikibot)
add_test(NAME query_main_page COMMAND .query_main_page)
add_executable(.metadata test/metadata.cpp)
target_link_libraries(.metadata wikibot)
add_test(NAME metadata COMMAND .metadata)
add_executable(traversal src/traversal.cpp)
target_link_libraries(traversal wikibot)
