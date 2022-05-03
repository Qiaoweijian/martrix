#/bin/bash

lcov -c -d ./ -c -o $1.info --rc lcov_branch_coverage=1
genhtml -o results $1.info --branch-coverage
