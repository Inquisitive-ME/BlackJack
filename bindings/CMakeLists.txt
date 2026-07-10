find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(_bjsim bjsim_py.cpp)
target_link_libraries(_bjsim PRIVATE bjsim)
target_compile_options(_bjsim PRIVATE -O2)

# Drop the compiled module next to the Python package so `import bjrl` finds it.
set_target_properties(_bjsim PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/python/bjrl)
