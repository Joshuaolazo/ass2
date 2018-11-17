#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "multiThreadSorter_thread.h"

void mergesorter(Node** front) {
	Node* one;
	Node* two;
	Node* head = *front;
	
	if(head->next == NULL||head == NULL)
		return;
	
	half(head, &one, &two);
	
	
	mergesorter(&one);
	mergesorter(&two);
	
	*front = merge(one,two);
}

Node* merge(Node* one,  Node* two) {
	Node* esp = NULL;
	
	if (one == NULL)
		return two;
	else if (two==NULL)
		return one;
	
	//fix this later
	int numcheck= isNumeric(one->cat);
	if(numcheck == 1){
		double aa, bb;
		sscanf(one->cat, "%lf", &aa);
		sscanf(two->cat, "%lf", &bb);
		if (aa<=bb) {
			esp = one;
			esp->next = merge(one->next, two);
		}
		else{
			esp = two;
			esp->next = merge(one, two->next);
		}
	}
	else{
		if (strcmp(one->cat,two->cat)<=0) {
			esp = one;
			esp->next = merge(one->next, two);
		}
		else{
			esp = two;
			esp->next = merge(one, two->next);
		}
	}
	return esp;
}


void half( Node* base, Node** first,  Node** last)
{
	Node* fast;
	Node* slow;
	slow = base;
	fast = base->next;
	
	while (fast != NULL)
	{
		fast = fast->next;
		if (fast != NULL) {
			slow = slow->next;
			fast = fast->next;
		}
	}
	
	
	*first = base;
	*last = slow->next;
	slow->next = NULL;
}

//checks if string is numeric
//returns 1 if numeric, 0 if non-numeric
int isNumeric(char* data){
	int i = 0;
	int isNum = 1; //set to 0 if not numeric
	int period = 0; //shouldnt be more than one period
	int length = (int)(strlen(data));
	for(i=0; i<length;i++){
		char c = data[i];
		if(c < '0' || c > '9'){
			if(c == '.')
				period++;
			else
				isNum = 0;
			if(period > 1)
				isNum = 0;
		}
	}
	return isNum;
}
