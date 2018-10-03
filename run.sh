set -e
set -v

./a.out ./random.so 100 10000 -
./a.out ./shuffle.so 100 10000 -
./a.out ./order.so 100 10000 -
./a.out ./split.so 100 10000 -
./a.out ./split_r.so 100 10000 -
./a.out ./disjoint.so 100 10000 -
./a.out ./overlap.so 100 10000 -
