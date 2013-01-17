echo "Compiling binaries."
g++ -c -fPIC ImageProcessing.cpp -o ImageProcessing.o
g++ -shared -Wl,-soname,libimgproc.so -o libimgproc.so  ImageProcessing.o

echo "Adjusting camera settings."
sudo uvcdynctrl -d /dev/video1 -s 'White Balance Temperature, Auto' 0

echo "Running python controller."
sudo python test.py
