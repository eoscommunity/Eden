add_executable(test-chainbase test-chainbase.cpp)
target_include_directories(test-chainbase PUBLIC ../include ../../abieos/include)
target_link_libraries(test-chainbase catch2)
target_compile_options(test-chainbase PUBLIC -g -O0)

add_executable(bench-chainbase bench-chainbase.cpp)
target_include_directories(bench-chainbase PUBLIC ../include ../../abieos/include)
target_compile_options(bench-chainbase PUBLIC -DNDEBUG -O3)
