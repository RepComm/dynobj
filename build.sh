gcc ./src/main.c -g -std=c99 -Wl,-rpath='${ORIGIN}' -Wall -Werror -o demo

chmod +x ./demo

echo done compiling