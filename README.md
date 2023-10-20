memory-test
===========

This repository contains unit tests for `mcech/memory`.

    cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build
    cmake --build build -j
    
    lcov --directory . --zerocounters
    build/memory-test --gtest_output=xml:report/unittest/index.xml
    xsltproc report/unittest/gtest-result.xsl report/unittest/index.xml > report/unittest/index.html
    gcov -a -c -H -m $(find build/CMakeFiles/memory-test.dir/src/*.o | paste -sd ' ')
    lcov --rc lcov_branch_coverage=1 -c -d . -o total.info
    lcov --rc lcov_branch_coverage=1 -r total.info "/usr/include/*" "*/gtest/*" "*/src/*" -o filtered.info
    genhtml --function-coverage --branch-coverage --demangle-cpp filtered.info --output-directory report/coverage
