CFLAGS = -Wall -lncurses -g
keyvo : main.o chars.o input.o stat.o text.o window.o 
	cc $(CFLAGS) -o keyvo main.o chars.o input.o stat.o text.o window.o 

main.o: main.c stat.h input.h
	cc $(CFLAGS) -c main.c

chars.o: chars.c chars.h
	cc $(CFLAGS) -c chars.c

input.o: input.c stat.h chars.h
	cc $(CFLAGS) -c input.c

text.o: text.c text.h window.h 
	cc $(CFLAGS) -c text.c

stat.o: stat.c stat.h text.h chars.h
	cc $(CFLAGS) -c stat.c

window.o: window.c text.h
	cc $(CFLAGS) -c window.c

clean:
	rm keyvo main.o input.o stat.o text.o window.o chars.o
