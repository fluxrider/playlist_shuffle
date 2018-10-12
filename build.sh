set -e

gcc src/stats.c -ldl -lm
gcc -o print src/print.c -ldl -lm
gcc -shared -fPIC -o order.so src/order.c
gcc -shared -fPIC -o random.so src/random.c -lbsd
gcc -shared -fPIC -o shuffle.so src/shuffle.c -lbsd
gcc -shared -fPIC -o broken.so src/broken.c -lbsd
gcc -shared -fPIC -o split.so src/split.c -lbsd
gcc -shared -fPIC -o split_r.so src/split_r.c -lbsd
gcc -shared -fPIC -o overlap.so src/overlap.c -lbsd
