g++ -c -fPIC ImageProcessing.cpp -o ImageProcessing.o
g++ -shared -Wl,-soname,libimgproc.so -o libimgproc.so  ImageProcessing.o
sudo python core.py
