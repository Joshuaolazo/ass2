all: multiThreadSorter_thread.c mergesort.o
	gcc multiThreadSorter_thread.c mergesort.o  -pthread -o  multiThreadSorter 
mergesort.o: mergesort.c
	gcc -c mergesort.c -o mergesort.o
clean:
	rm -rf multiThreadSorter mergesort.o
