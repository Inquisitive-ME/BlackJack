# Benchmarks are always optimized so numbers are meaningful regardless of build type.
add_executable(bench_shoe bench_shoe.cpp)
target_link_libraries(bench_shoe PRIVATE bjsim)
target_compile_options(bench_shoe PRIVATE -O2 -march=native)
