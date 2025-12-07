cmake_minimum_required(VERSION 3.16)
project(SVDPythonBindings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT DEFINED Python3_EXECUTABLE)
    execute_process(
        COMMAND python3 -c "import sys; print(sys.executable)"
        OUTPUT_VARIABLE _python3_path
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE _python3_path_result)
    if(_python3_path_result EQUAL 0 AND _python3_path)
        set(Python3_EXECUTABLE "${_python3_path}" CACHE FILEPATH "Preferred python3 interpreter" FORCE)
    endif()
endif()

find_package(Python3 COMPONENTS Interpreter Development.Module REQUIRED)

# Allow pybind11 installed via pip to be discovered automatically.
execute_process(
    COMMAND ${Python3_EXECUTABLE} -m pybind11 --cmakedir
    OUTPUT_VARIABLE _pybind11_cmakedir
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE _pybind11_cmakedir_result)
if(_pybind11_cmakedir_result EQUAL 0 AND _pybind11_cmakedir)
    list(APPEND CMAKE_PREFIX_PATH "${_pybind11_cmakedir}")
    set(pybind11_DIR "${_pybind11_cmakedir}" CACHE PATH "pybind11 config location" FORCE)
endif()

find_package(pybind11 CONFIG QUIET)

add_library(svd_core
    src/matrix.cpp
    src/svd.cpp
)

target_include_directories(svd_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

add_library(svd::core ALIAS svd_core)

set(_svd_binding_target svd_bindings)

if(pybind11_FOUND)
    pybind11_add_module(${_svd_binding_target} src/bindings.cpp)
    set(_pybind11_include_dirs "")
else()
    execute_process(
        COMMAND ${Python3_EXECUTABLE} -m pybind11 --includes
        OUTPUT_VARIABLE _pybind11_includes
        ERROR_VARIABLE _pybind11_includes_error
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE _pybind11_includes_result)
    if(NOT _pybind11_includes_result EQUAL 0)
        message(FATAL_ERROR
            "pybind11 not found. Install it via `python3 -m pip install pybind11`.\n"
            "Details: ${_pybind11_includes_error}")
    endif()
    separate_arguments(_pybind11_include_list NATIVE_COMMAND "${_pybind11_includes}")
    set(_pybind11_include_dirs "${_pybind11_include_list}")

    add_library(${_svd_binding_target} MODULE src/bindings.cpp)
    target_link_libraries(${_svd_binding_target} PRIVATE Python3::Module)
    set_target_properties(${_svd_binding_target} PROPERTIES PREFIX "")
    if(Python3_EXTENSION_MODULE_SUFFIX)
        set_target_properties(${_svd_binding_target} PROPERTIES SUFFIX "${Python3_EXTENSION_MODULE_SUFFIX}")
    endif()
endif()

target_link_libraries(${_svd_binding_target} PRIVATE svd::core)

target_include_directories(${_svd_binding_target} PRIVATE include ${_pybind11_include_dirs})

set_target_properties(${_svd_binding_target} PROPERTIES OUTPUT_NAME "svd_bindings")

install(TARGETS svd_core EXPORT SVDTargets)
install(DIRECTORY include/ DESTINATION include)

install(TARGETS svd_bindings
        LIBRARY DESTINATION ${Python3_SITEARCH}
        ARCHIVE DESTINATION ${Python3_SITEARCH}
        RUNTIME DESTINATION ${Python3_SITEARCH})

install(EXPORT SVDTargets FILE SVDTargets.cmake NAMESPACE svd:: DESTINATION lib/cmake/SVDPythonBindings)
