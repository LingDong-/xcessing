gcc $1 -O3 -I /opt/X11/include -L/opt/X11/lib -lX11 -lpng -lm -std=c99
if [ $? -eq 0 ]; then
    ./a.out
fi