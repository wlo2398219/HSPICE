program: main.o  table.o res.o vol.o ckt.o module.o file.o spmatrix.o cap.o mosload.o mosmpar.o mosset.o
	gcc -o main main.o table.o res.o vol.o ckt.o module.o file.o spmatrix.o cap.o mosload.o mosmpar.o mosset.o -O3
main.o: main.c 
	gcc -c main.c -O3
table.o: table.c
	gcc -c table.c -O3
res.o: res.c
	gcc -c res.c -O3
vol.o: vol.c
	gcc -c vol.c -O3
ckt.o: ckt.c
	gcc -c ckt.c -O3
spmatrix.o: spmatrix.c
	gcc -c spmatrix.c -O3
module.o: module.c
	gcc -c module.c -O3
file.o: file.c
	gcc -c file.c -O3
cap.o: cap.c
	gcc -c cap.c -O3
mosload.o: mosload.c
	gcc -c mosload.c -O3
mosmpar.o: mosmpar.c
	gcc -c mosmpar.c -O3
mosset.o: mosset.c
	gcc -c mosset.c -O3
clean:
	rm *.o

 