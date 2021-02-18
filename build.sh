gcc ./src/main.c -Wl,-rpath='${ORIGIN}' -Wall -o multiconnect

chmod +x ./multiconnect

echo done compiling