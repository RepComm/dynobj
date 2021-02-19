gcc ./src/main.c -Wl,-rpath='${ORIGIN}' -Wall -o demo

chmod +x ./demo

echo done compiling