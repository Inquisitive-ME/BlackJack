# Benchmarks are always optimized so numbers are meaningful regardless of build type.
add_executable(bench_shoe bench_shoe.cpp)
target_link_libraries(bench_shoe PRIVATE bjsim)
target_compile_options(bench_shoe PRIVATE -O2 -march=native)

add_executable(bench_engine bench_engine.cpp)
target_link_libraries(bench_engine PRIVATE bjsim)
target_compile_options(bench_engine PRIVATE -O2 -march=native)
