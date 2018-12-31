#!/bin/bash

# Save the bash setting for not matching case
SHELLNOCASEMATCH=$(shopt -p nocasematch; true)
shopt -s nocasematch

# Setup the flags for configuring HELICS with CMake
OPTION_FLAGS_ARR=()
OPTION_FLAGS_ARR+=("-DBUILD_C_SHARED_LIB=ON" "-DBUILD_SHARED_LIBS=ON" "-DEXAMPLES_WARNINGS_AS_ERROR=ON")
OPTION_FLAGS_ARR+=("-DZMQ_USE_STATIC_LIBRARY=ON -DUSE_BOOST_STATIC_LIBS=ON")

# Options to control building swig interfaces
if [[ "${DISABLE_INTERFACES}" != *"Java"* ]]; then
    OPTION_FLAGS_ARR+=("-DBUILD_JAVA_INTERFACE=ON")
fi
if [[ "${DISABLE_INTERFACES}" != *"Python"* ]]; then
    OPTION_FLAGS_ARR+=("-DBUILD_PYTHON_INTERFACE=ON" "-DPYTHON_LIBRARY=${PYTHON_LIB_PATH}" "-DPYTHON_INCLUDE_DIR=${PYTHON_INCLUDE_PATH}")
fi
if [[ "$USE_SWIG" == 'false' ]]; then
    OPTION_FLAGS_ARR+=("-DENABLE_SWIG=OFF")
fi

# Options related to the CMake build type
if [[ "$BUILD_TYPE" ]]; then
    OPTION_FLAGS_ARR+=("-DCMAKE_BUILD_TYPE=${BUILD_TYPE}")
    if [[ "$BUILD_TYPE" == "Coverage" ]]; then
        OPTION_FLAGS_ARR+=("-DTEST_CODE_COVERAGE=ON")
    fi
fi

# CPack/Install options
if [[ "$ENABLE_CPACK" ]]; then
    OPTION_FLAGS_ARR+=("-DENABLE_PACKAGE_BUILD=ON")
fi
if [[ "$INSTALL_SYSTEM_LIBRARIES" ]]; then
    OPTION_FLAGS_ARR+=("-DINSTALL_SYSTEM_LIBRARIES=ON")
fi

# MPI options
if [[ "$USE_MPI" ]]; then
    OPTION_FLAGS_ARR+=("-DENABLE_MPI=ON")
    CC=${CI_DEPENDENCY_DIR}/mpi/bin/mpicc
    CXX=${CI_DEPENDENCY_DIR}/mpi/bin/mpic++
fi

# Compiler/language options
if [[ "$CXX_STANDARD" == 17 ]]; then
    OPTION_FLAGS+=("-DENABLE_CXX_17=ON")
fi

# Travis related options
if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    OPTION_FLAGS_ARR+=("-DDISABLE_SYSTEM_CALL_TESTS=ON")
fi
export HELICS_OPTION_FLAGS=${OPTION_FLAGS_ARR[@]}

# Setup the flags for controlling test execution
TEST_FLAGS_ARR=("$TEST_TYPE")
if [[ "$CI_TEST_CONFIG" ]]; then
    TEST_FLAGS_ARR+=("$CI_TEST_CONFIG")
fi

# Valgrind options
if [[ "$RUN_VALGRIND" ]]; then
    TEST_FLAGS_ARR+=("--valgrind" "--disable-unit-tests")
fi
if [[ "$RUN_CACHEGRIND" ]]; then
    TEST_FLAGS_ARR+=("--cachegrind")
fi

# Sanitizer tests
if [[ "$RUN_ASAN" ]]; then
    TEST_FLAGS_ARR+=("--asan")
fi
if [[ "$RUN_MSAN" ]]; then
    TEST_FLAGS_ARR+=("--msan")
fi
if [[ "$RUN_TSAN" ]]; then
    TEST_FLAGS_ARR+=("--tsan")
fi
if [[ "$RUN_UBSAN" ]]; then
    TEST_FLAGS_ARR+=("--ubsan")
fi

# Misc options
if [[ "$NO_CTEST" ]]; then
    TEST_FLAGS_ARR+=("--no-ctest")
fi
if [[ "$CTEST_VERBOSE" ]]; then
    TEST_FLAGS_ARR+=("--ctest-verbose")
fi
if [[ "$DISABLE_CI_TESTS" ]]; then
    TEST_FLAGS_ARR+=("--disable-unit-tests")
fi
export CI_TEST_FLAGS=${TEST_FLAGS_ARR[@]}

# Restore the origin nocasematch setting
$SHELLNOCASEMATCH