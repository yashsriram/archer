fusion: src/
	g++ src/main.cpp -Wall lib/libsprite.a -Iinclude -I/usr/include/X11 -L/usr/X11R6/lib -lX11 -std=c++11 -o lightplay

play:
	./lightplay

clean:
	rm lightplay