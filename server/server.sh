rm -r build
mkdir build
cd build
qmake -makefile ../code/TCPServer.pro
make
