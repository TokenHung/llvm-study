echo "===test1.ll==="
opt -load ./hw1.so -Hw1 test1.ll -o /dev/null
echo "===test2.ll==="
opt -load ./hw1.so -Hw1 test2.ll -o /dev/null
echo "===test3.ll==="
opt -load ./hw1.so -Hw1 test3.ll -o /dev/null