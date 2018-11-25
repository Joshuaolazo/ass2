all: multiThreadSorter_thread.c mergesort.o
	gcc multiThreadSorter_thread.c mergesort.o -o  -pthread multiThreadSorter 
mergesort.o: mergesort.c
	gcc -c mergesort.c -o mergesort.o
clean:
	rm -rf multiThreadSorter mergesort.o
