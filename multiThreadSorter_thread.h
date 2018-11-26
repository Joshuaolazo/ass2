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

typedef struct _TIDNode{
	int TID;
	struct _TIDNode * next;
}TIDNode;


Node* merge(Node* one, Node* two);
void half( Node* base, Node** first,  Node** last);
void mergesorter(Node** front);
void * directory_crawler(void* param);
int sortCSV(char *, char*);
int isNumeric(char*);
char* column_reader(char* , int );
int column_finder(char*);
int keymaker(char*,int ,int*);
void * addCSV( void* param);

#endif /* multiThreadSorter_thread_h */
