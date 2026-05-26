#!/bin/bash

echo "================================"
echo "RB-Tree Laboratory Work Tests"
echo "================================"
echo

# Build the project
echo "Building project..."
make clean
make all

if [ ! -f ./lab4 ]; then
    echo "Build failed!"
    exit 1
fi

echo
echo "=== TEST SUITE 1 ==="
./lab4 tests/test_input.txt test_output1.txt
cat test_output1.txt

echo
echo "=== TEST SUITE 2 (Edge Cases) ==="
./lab4 tests/test_input2.txt test_output2.txt
cat test_output2.txt

echo
echo "================================"
echo "All tests completed!"
echo "================================"