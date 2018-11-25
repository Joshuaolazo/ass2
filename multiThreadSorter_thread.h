//
//  multiThreadSorter_thread.h
//  ass2
//
//  Created by Joshua Olazo on 11/15/18.
//

#ifndef multiThreadSorter_thread_h
#define multiThreadSorter_thread_h

#include <stdio.h>

typedef struct _Node{
	char* data;
	char* cat;
	struct _Node * next;
}Node;

typedef struct _addCSVargs{
	char* sorting_column;
	char* file_name;
	char* input_directory;
}addCSVargs;

typedef struct _directory_crawlerargs{
	char* sorting_directory;
	char* sorting_column;
	char* output_directory;
	int* count;
	pthread_t tid[255];
}directory_crawlerargs;


Node* merge(Node* one, Node* two);
void half( Node* base, Node** first,  Node** last);
void mergesorter(Node** front);
int directory_crawler(directory_crawlerargs* args);
int sortCSV(char *, char* , char* ,char*);
int isNumeric(char*);
char* column_reader(char* , int );
int column_finder(char*);
int keymaker(char*,int ,int[]);
int addCSV( addCSVargs* args);
int argchecker( int , char*[], char* , char* ,char*);

#endif /* multiThreadSorter_thread_h */
