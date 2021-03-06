//
//  multiThreadSorter_thread.c
//  ass2
//
//  Created by Joshua Olazo on 11/15/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <pthread.h>
#include "multiThreadSorter_thread.h"


const int numeric[28] = {0,0,1,1,1,1,0,1,1,0,0,0,1,1,0,1,0,0,1,0,0,0,1,1,1,1,1,1};

Node* Global;
const int PRINT = 9;


pthread_mutex_t lock;
pthread_mutex_t lock2;

int main(int argc, char *argv[]){


	// Make space for Global
	Global = (Node*) malloc(sizeof(Node));
	// Check for good arguments example below
	// ./sorter -c movie_title -d thisdir -o thatdir
	
	// More descriptive error messages for bad flags
	char* sorting_column = NULL;
	char* sorting_directory = NULL;
	char* output_directory = NULL;
	int x;
	switch (argc) {
		// "Bad" Input Args
		// Returns descriptive error message
		case 0 :
		case 1 :
		fprintf(stderr,"%s\n","Not enough input arguments");
		return -1;
		case 2 :
		case 4 :
		case 6 :
		fprintf(stderr,"%s\n","Bad number of input arguments");
		return -1;

		// "Good" input args
		// Runs the code below each case
		// each case below is a subset
		case 7 :
		x = 7;
		if(strcmp(argv[x-2],"-o") == 0){
			if(output_directory==NULL ){
				output_directory= argv[x-1];
			}
			else{
				fprintf(stderr,"%s\n","Double set flag");
				return -1;
			}
		}
		else if(strcmp(argv[x-2],"-d") == 0){
			if(sorting_directory==NULL ){
				sorting_directory= argv[x-1];
			}
			else{
				fprintf(stderr,"%s\n","Double set flag");
				return -1;
			}
		}
		else if(strcmp(argv[x-2],"-c") == 0){
			if(sorting_column==NULL ){
				sorting_column= argv[x-1];
			}
			else{
				fprintf(stderr,"%s\n","Double set flag");
				return -1;
			}
		}
		else{
			fprintf(stderr,"%s\n","Invalid Flag");
			return -1;
		}
		case 5:
		x = 5;
		if(strcmp(argv[x-2],"-o") == 0){
			if(output_directory==NULL ){
				output_directory= argv[x-1];
			}
			else{
				fprintf(stderr,"%s\n","Double set flag");
				return -1;
			}
		}
		else if(strcmp(argv[x-2],"-d") == 0){
			if(sorting_directory==NULL ){
				sorting_directory= argv[x-1];
			}
			else{
				fprintf(stderr,"%s\n","Double set flag");
				return -1;
			}
		}
		else if(strcmp(argv[x-2],"-c") == 0){
			if(sorting_column==NULL ){
				sorting_column= argv[x-1];
			}
			else{
				fprintf(stderr,"%s\n","Double set flag");
				return -1;
			}
		}
		else{
			fprintf(stderr,"%s\n","Invalid Flag");
			return -1;
		}
		case 3 :
		x = 3;
		if(strcmp(argv[x-2],"-o") == 0){
			if(output_directory==NULL ){
				output_directory= argv[x-1];
			}
			else{
				fprintf(stderr,"%s\n","Double set flag");
				return -1;
			}
		}
		else if(strcmp(argv[x-2],"-d") == 0){
			if(sorting_directory==NULL ){
				sorting_directory= argv[x-1];
			}
			else{
				fprintf(stderr,"%s\n","Double set flag");
				return -1;
			}
		}
		else if(strcmp(argv[x-2],"-c") == 0){
			if(sorting_column==NULL ){
				sorting_column= argv[x-1];
			}
			else{
				fprintf(stderr,"%s\n","Double set flag");
				return -1;
			}
		}
		else{
			fprintf(stderr,"%s\n","Invalid Flag");
			return -1;
		}
		break;

		// Bad input if over 7 args
		default:
		fprintf(stderr,"%s\n","Too many input arguments");
		return -1;
	}

	// Sorting Column must be defined
	if(sorting_column==NULL ){
		fprintf(stderr,"%s\n","-c flag is required");
		return -1;
	}
	if(sorting_directory==NULL ){
		sorting_directory = "./";
	}
	//Checking output directory
	struct stat st = {0};
	if(output_directory!=NULL&&stat(output_directory, &st)== -1){
		fprintf(stderr, "%s\n","Error: Invalid Output Directory.");
		return(-1);
	}
	
	// Mutex lock Initial
	if (pthread_mutex_init(&lock, NULL) != 0){
		fprintf(stderr,"\n Mutex Initialize failed\n");
		return -1;
	}
	if (pthread_mutex_init(&lock2, NULL) != 0){
		fprintf(stderr,"\n Mutex Initialize failed\n");
		return -1;
	}

	pthread_t tid[255];

	// Start sorting process
	int i = 0;
	int * count;
	count = &i;
	directory_crawlerargs * original_args = (directory_crawlerargs*)malloc(sizeof(directory_crawlerargs));
	original_args-> sorting_directory = sorting_directory;
	original_args-> sorting_column = sorting_column;
	original_args-> output_directory = output_directory;
	original_args-> count = count;

	TIDNode * tidlist = (TIDNode*) malloc(sizeof(TIDNode));

	tidlist = directory_crawler(original_args);




    // Meta Data Print
	int threadcount  =0;
	int parent_pid= getpid();
	printf("main\n");
	printf("Initial PID: %d\n",parent_pid);
	printf("main\n");
	char message[]  = "TIDS of all spawned threads: \0";
	printf("%s",message);
    while(tidlist->next != NULL ){
		printf("%d, ", tidlist->TID );
        tidlist= tidlist->next;
		threadcount++;
	}	
	printf("\nTotal number of threads: %d\n", threadcount);
    
	pthread_mutex_destroy(&lock);
	pthread_mutex_destroy(&lock2);

	return 0;
}


void * directory_crawler( void * param ){
	TIDNode * tidlist = (TIDNode*) malloc(sizeof(TIDNode));
	tidlist = NULL;
    directory_crawlerargs *  args= (directory_crawlerargs * )param;
	char * sorting_directory = args->sorting_directory;
	char * sorting_column = args->sorting_column;
	char * output_directory= args->output_directory;
	int * count= args->count;

	DIR *directory;
	struct dirent *dirent;
	directory = opendir(sorting_directory);
	if( !directory){
		fprintf(stderr,"Cannot open directory: %s\n", strerror (errno));
		return tidlist;
	}
	int t= 0;
	void* status;
	while(t==0){

		const char * d_name;
		dirent = readdir (directory);
		if (! dirent) {
			t= 1;
			break;
		}
		d_name = dirent->d_name;
		if(PRINT==0|| PRINT ==3){
			printf ("%s\n",  d_name);
		}
        //&& strcmp( d_name, ".git")
		if (dirent->d_type & DT_DIR) {
			if (strcmp (d_name, "..") != 0 && strcmp (d_name, ".") != 0 && strcmp( d_name, ".git")) {
				if(PRINT == 3)
				printf("DIRECTORY\n");
				int directorylen= (int) strlen(sorting_directory);
				int d_namelen= (int) strlen(d_name);
				char* new_directory = malloc(directorylen+d_namelen+1);
				strcpy(new_directory, sorting_directory);
				strcat(new_directory, d_name);
				strcat(new_directory, "/");
				if(PRINT==0){
					printf("New directory: %s\n", new_directory);
				}

				directory_crawlerargs * directory_args = malloc(sizeof(directory_crawlerargs));
				directory_args-> sorting_directory = new_directory;
				directory_args-> sorting_column = sorting_column;
				directory_args-> output_directory = output_directory;
				*count = *count +1 ;
				directory_args-> count = count;
				fflush(stdout);
				pthread_t cThread;
				pthread_create(&cThread, NULL, directory_crawler, (void *) directory_args);

					pthread_join(cThread, &status);

					pthread_mutex_lock(&lock2);
					TIDNode * tid = malloc(sizeof(TIDNode));
					tid->TID = cThread;
					tid->next = tidlist;
					tidlist= tid;

					TIDNode * ret = malloc(sizeof(TIDNode));
					ret = (TIDNode *)status;
					TIDNode * ptr = tidlist;
					while(ptr->next != NULL){
							ptr = ptr->next;
					}
					ptr->next = ret;
					pthread_mutex_unlock(&lock2);


			}

		}else{
			fflush(stdout);
			addCSVargs * csv_args = malloc(sizeof(addCSVargs));
			csv_args-> sorting_column = sorting_column;
			csv_args-> input_directory = sorting_directory;
			csv_args-> file_name = (char*) d_name;
			*count = *count +1;
			pthread_t cThread;
			pthread_create(&cThread, NULL, addCSV, (void *) csv_args);


				pthread_join(cThread, &status);

				pthread_mutex_lock(&lock2);
				TIDNode * tid = malloc(sizeof(TIDNode));
                if(PRINT ==0)
				    printf("Csv thread ID: %d\n", cThread);
                tid->TID = cThread;
				tid->next = tidlist;
				tidlist= tid;
                if(PRINT ==0)
                    printf("New csv tid is: %d\n",tidlist->TID);
				pthread_mutex_unlock(&lock2);


		}
	}
	// Meta Data 
    fflush(stdout);
	int threadcount  =0;
	int parent_pid= getpid();
    if(PRINT ==9)
        printf("\nDirectory is :%s \n", sorting_directory);
	printf("Initial PID: %d\n",parent_pid);
	printf("TIDS of all spawned threads: \0");
    
    TIDNode * print = malloc(sizeof(TIDNode));
    print = tidlist; 
    if (print != NULL){      
        while(print!= NULL ){
		  printf("%d, ", print->TID );
          print= print->next;
		  threadcount++;
	   }
    }
	printf("\nTotal number of threads: %d\n", threadcount);


	return (void*) tidlist;
}



//Where argv is what we're sorting by , file, output directory
void * addCSV(void* param){

   

	addCSVargs* args= (addCSVargs*)param;
	char *argv = args->sorting_column;
	char* file_name = args->file_name;
	char* input_directory = args->input_directory;
    
      
    // Meta Data Print
	int threadcount  =0;
	int parent_pid= getpid();
    if(PRINT ==9)
        printf("\nFile is :%s \n", file_name);   
	printf("Initial PID: %d\n",parent_pid);
	char message[]  = "TIDS of all spawned threads: \0";
	printf("%s",message);
	printf("\nTotal number of threads: %d\n", 0);

    
    
    printf("one\n");
	//file + directory
	char * full_file_path;
	full_file_path = (char*) malloc(sizeof(char*) * (strlen(input_directory) + strlen(file_name)) );

	//Checking directory status
	struct stat stt = {0};

	//Appends input_directory to beginning of file_name name if input directory exists
	if(stat(input_directory, &stt)!= -1){
		strcpy(full_file_path,input_directory);
		strcat(full_file_path,file_name);
	}
	else{
		strcpy(full_file_path,file_name);
	}

	//check if file exists
    FILE *fp;
    fp = fopen(full_file_path,"r");

    //fileDoesNotExist
    if(fp==NULL){
        fprintf(stderr,"File DNExist\n");
        return (void*) -1;
    }
    //check last 4 letters .csv
    //check if extension is csv file
    char end[5] = "";
    int z=0;
    int h=0;
    for(z=(int)strlen(file_name)-4;z< (int)strlen(file_name);z++){
        end[h]=file_name[z];
        h++;
    }
    if(strcmp(end,".csv")!=0){
        fclose(fp);
	    return (void*) -1;
    }

    //Therefore we know we are working with csv file which exists

    char * buffer = NULL;
    size_t len = 0;
    Node * local = NULL;
    Node * ptr  = NULL;
    long llength = 0;

    // make local linked list of movies

    // Reading CSV files
    printf("header\n");
    // read first line (header row)
    getline(&buffer, &len, fp);
    char * header = buffer;
    int column_number = column_finder(header);
    int* key = (int*) malloc(column_number * sizeof(int));
    printf("call keymaker\n");
    int kz = keymaker(header, column_number, key);
    printf("key\n");
    int shit;
    
    for(shit = 0; shit<column_number;shit++){
        printf("key at i = %d: %d\n",shit, key[shit]);
    }
    if ( kz <0){
        // maybe add which file
        fprintf(stderr,"%d %s\n",z," :# of bad columns found");
    	return (void*) -1;
    }
    printf("reader\n");
    // read other rows (movie rows)
    while((getline(&buffer, &len, fp)!=-1)){
        char *movie_array[28];
        // if one movie does not have the correct number of columns, the file is bad and all the movies do not get added to the list
    	if( column_finder(buffer)!=  column_number){
            return (void*) -1;
        }
		int i;
        // For every column serparate by comma and find column data
        for(i=0; i<column_number;i++){
            char* data= column_reader(header,i);
            movie_array[key[i]]= data;
            // convert movie array to string
            int j;
            char* movie_string = (char*) malloc((strlen(buffer)+column_number+5000)*sizeof(char*));
            for(j=0; j<28;j++){
                if(j==28){
                    if( movie_array[i]!= NULL){
                        strcat(movie_string,movie_array[i]);
                    }
                    // might be null terminator
                    strcat(movie_string,"\n");
                }
                if( movie_array[i]!= NULL){
                    strcat(movie_string,movie_array[i]);
                }
                strcat(movie_string,",");
            }
            // add to local linked list

            if(local == NULL){
        		Node * movie = (Node*)malloc(sizeof(Node));
        		movie->data = movie_string;
        		movie->next = NULL;
        		local = movie;
        		ptr = local;

        	}else{
                Node * movie = (Node*)malloc(sizeof(Node));
        		movie->data = movie_string;
        		ptr->next = movie;
        		ptr = ptr->next;
        	}

        }
    }
    printf("adder\n");
    // add to global linked list
	pthread_mutex_lock(&lock);
    Node * temp  = NULL;
    temp = Global;
    while(temp){
        temp= temp->next;
    }
    temp = local;
	pthread_mutex_unlock(&lock);

	

	return (void *)0;
}




//Input: header,number of columns in header, key array Output: string in column
int keymaker(char* header,int column_number,int* key){
    char *position[28] = {"color",
                         "director_name",
                         "num_critic_for_reviews",
                         "duration",
                         "director_facebook_likes",
                         "actor_3_facebook_likes",
                         "actor_2_name",
                         "actor_1_facebook_likes",
                         "gross",
                         "genres",
                         "actor_1_name",
                         "movie_title",
                         "num_voted_users",
                         "cast_total_facebook_likes",
                         "actor_3_name",
                         "facenumber_in_poster",
                         "plot_keywords",
                         "movie_imdb_link",
                         "num_user_for_reviews",
                         "language","country",
                         "content_rating",
                         "budget",
                         "title_year",
                         "actor_2_facebook_likes",
                         "imdb_score",
                         "aspect_ratio",
                         "movie_facebook_likes"};
    // Initialize variables
    int length = strlen(header);
    int i;
    /*
    printf("Entire header row: \n %s", header);
    printf("length is : %d\n", length);
    printf("column number: %d\n", column_number);
    printf("cat\n");
    printf("before for loop\n");
    */
    // For every column, serparate by comma and find heading
    for(i=0; i<column_number;i++){
        //printf("i is : %d\n", i);
        char* column= column_reader(header,i);
        //printf("column is: %s\n", column);
        int j;
        int all_correct=0;
        // For every heading, match to find its position in global
        for(j=0;j<28;j++){
            int lcol= strlen(column);
            int lpos= strlen(position[j]);
            int diff = strcmp(column,position[j]);
            //printf("column is: %s and position[j] is: %s and j is : %d\n", column,  position[j], j);
            //printf("strlen col: %d and strlen pos: %d\n\n", lcol, lpos);

            
            // if match add to key
            if(diff==0){
                printf("column is: %s and position[j] is: %s and j is : %d\n", column,  position[j], j);
                key[i]= j;
                
                break;
            }
            else{
                if(j == 28){
                    //printf("column is: %s and position[j] is: %s and j is : %d\n", column,  position[j], j);
                    key[i]= -1;
					return -1;
                }
            }
            
        }
        printf("key at i = %d: %d\n",i, key[i]);
    }
    return 0;

}
// finds column number
// almost the same as column_reader
// input:row, output: column number
int column_finder(char* row){
	// loop setup
	int commas = 0;
	int inquotes = 0;
	int i= 0;
    int length = strlen(row);

	//counts commas to find column number
	while(i<length){
		char c = row[i];
		// quote check
		if(c =='"'){
			if(inquotes ==0)
					inquotes--;
			else
				inquotes++;
		}
		// if in quotes do not count commas
		if(inquotes == 0){
			if(c == ',')
				commas++;
		}
		i++;
	}
	// There are 1 less comma than column number;
	return commas+1;
}



//Input: Column number and row string, Output: string in column
char* column_reader(char* row, int column){
    
	int commas = column+1; 	// There are 1 less comma than column number
	int diff=0; 	// Store beginning and end index
	int end;
    
	// If parenthesis for movie title, there is a comma in the title
	// using int as pseudo boolean. 0 for false 1 for true
	int inquotes = 0;
    int quoted=0;
    int last=0;
	// Setup for loop
	int i=0;
    //printf("commas is : %d\n", commas);
	while(commas>0){

		if(commas == 1){
			diff++;
            if(inquotes!=0){
                quoted++;
            }
		}
		char c = row[i];
        //printf("char is %c\n", c);
        if(c=='\n'){
            last++;
            break;
        }

		// quote check
		if(c =='"'){
			if(inquotes ==0){
				inquotes--;
			}
			else{
				inquotes++;
			}
		}
		// if in quotes do not count commas
		if(inquotes == 0){
			if(c == ','){
				commas--;
			}
		}
		i++;
	}

    end= i-1;

	// includes the commas so +1 and -1 at the end
	int start = end-diff+1;
	diff--;
    //diff--;
    if(diff==0){
        return NULL;
    }
    if(last>0){
        start++;
        diff--;
    }
    if(quoted>0){
        start++;
        diff= diff-2;
    }
	// for storing the output

    char* column_string = (char*) malloc(diff*2+10*sizeof(char*));
    int x = 0;
    while(row[start]==' '){
        start++;
    }
    while (x<diff) {
        row[start]==' ';
        column_string[x]=row[start];
        x++;
        start++;
    }
    //column_string[x+1]='\0';
    return column_string;
}


//argv is what we are sorting by, ddir is output directory
//Previous ffile and idir are handled outside of method now
int sortCSV(char *argv, char* ddir){

	Node *front=NULL;
	Node * ptr = NULL;
	long llength = 0;

	//new stuff GLOBAL is new char* LL

	Node * Globalptr = NULL;
	Globalptr = Global;


	while(Globalptr!=NULL){
		llength++;


		if(front == NULL){
			Node * current = (Node*)malloc(sizeof(Node));
			char * copystring = (char *)malloc(sizeof(char)*strlen(Globalptr->data)*2);
			strcpy(copystring, Globalptr->data);
			current->data = copystring;
			current->next = NULL;
			front = current;
			ptr = front;

		}else{
			Node * current = (Node*)malloc(sizeof(Node));
			char * copystring = (char *)malloc(sizeof(char)*strlen(Globalptr->data)*2);
			strcpy(copystring, Globalptr->data);
			//for last input which ends in EOF
			if(copystring[strlen(copystring)-1]!='\n')
				copystring[strlen(copystring)] = '\n';
			current->data = copystring;
			ptr->next = current;
			ptr = ptr->next;

		}

		Globalptr=Globalptr->next;
	}



	// fix length kill extra
	long llength1 =llength;
	Node *extra = front;
	Node *t;
	//printf("length: %ld\n",llength1);
	while (llength1>0){
		t=extra;
		extra = extra->next;
		llength1--;
		//printf("length: %ld\n",llength1);
		if( llength1==0){
			t->next=NULL;
			break;
		}
	}



	char * ind = (char*)malloc(sizeof(char)*strlen(front->data));
	char * potato = (char*)malloc(sizeof(char)*strlen(front->data));
	char * potato1 = (char*)malloc(sizeof(char)*strlen(front->data));

	ind = argv;				//index to sort by

	char * copystring1 = (char *)malloc(sizeof(char)*strlen(front->data));
	strcpy(copystring1, front->data);
	char * copystring2 = (char *)malloc(sizeof(char)*strlen(front->data));
	strcpy(copystring2, front->data);
	potato = copystring1;		//first line of categories
	potato1 = copystring2;


	//split commas
	int comma = 0; //first comma the relevant data is on
	int commamax = 0;
	char *found = (char*)malloc(sizeof(char)*strlen(front->data));
	//char *found1= (char*)malloc(sizeof(char)*strlen(front->data));

	int notfound= 1;
	if(strstr(front->data,argv))
	notfound=0;

	while(strsep(&potato1, ",")!=NULL){


		commamax++; //max commas
	}


	while((found = (strsep(&potato, ",")))!=NULL){
	if(strcmp(found,argv)==0){
	notfound = 0;
	break;
	}
		comma++;
	}
	if(comma==commamax)
	comma--;





	//add cat values
	Node * temp1 = (Node*)malloc(sizeof(Node));
	temp1 = front;
	char dumbo;
		int titlename = 0;
		//int koo = 0;
	int i;
	int u;
	int fakecommas;
	//int commacomma;
	int commacheck;
	int totalfakes;
	//long llength2 =llength;
	while(temp1!=NULL){

		if(temp1->data == NULL){
			fprintf(stderr,"blank column\n");
			return -1;
		}
		/*
		printf("start one\n");
		printf("start two\n");
		printf("Temp data is: %s", temp1->data);
		printf("Size of malloc is: %lu\n",sizeof(char)*strlen(temp1->data));
		*/
		char *find = (char*)malloc(sizeof(char)*strlen(temp1->data));
		char * copy = (char *)malloc(sizeof(char)*strlen(temp1->data));


		strcpy(copy, temp1->data);


		int stupid = (int) strlen(copy);
		int commacheck = 0;
		int fakecommas = 0;
		int totalfakes = 0;
		int titlename=0;
		int firsty = 0;
		int fraud = 0;
		int quotes=0;
		int openingcomma=0;
		int closingcomma=0;
		//finding fake commas per category


		for(i=0;i<stupid;i++){
			dumbo=copy[i];

			if(dumbo==',')
				commacheck++;

			if(dumbo=='"'){
				titlename++;
				titlename=titlename%2;
			}
			if(titlename==1&&dumbo==',')
				fakecommas++;

			//if comma-- ==commacheck - fakecommas take keep both indices and take string between them to use in cat
			//if I am 1 commas away I will


			if((commacheck-fakecommas)==comma&&firsty==0){
				firsty=1;
			}

			if(firsty==1&&dumbo=='"'&&quotes==0){
				if(PRINT == 4){
					printf("opening:%d\t",i);
				}
				openingcomma = i;
				quotes=1;
			}
			if(quotes>0)
				quotes++;

			if(quotes>2&&dumbo=='"')
				fraud=1;


			if(fraud==1){
				if(PRINT == 4){
					printf("closing: %d\n",i);
				}
				closingcomma=i;
				break;
			}


		}


		// finding fake commas in total
		titlename=0;
		commacheck=0;
		for(i=0;i<stupid;i++){
			dumbo=copy[i];

			if(dumbo==',')
				commacheck++;

			if(dumbo=='"'){
				titlename++;
				titlename=titlename%2;
			}
			if(titlename==1&&dumbo==',')
				totalfakes++;

		}
		//printf("%d-%d-%d \n", commacheck,totalfakes,commamax);


		if(commacheck-totalfakes!=commamax-1){
			//ignore formatted incorrectly
			fprintf(stderr, "%s\n","Error: Inconsistent commas between rows.");
			//fclose(fp);
			return -1;
		}
		//THERE WERE COMMAS IN PARENTHESES
		u=0;

		if(fraud==1){
			if(PRINT == 4)
				printf("fraud\n");
			for(i=openingcomma+1;i<=closingcomma-1;i++){
				find[u]=copy[i];
				u++;
			}
			if(PRINT == 4)
				printf("%s\n",find);
		}
		else{

			for(u=0;u<=(comma+fakecommas);u++){

				find = strsep(&copy, ",");
			}
		}
		int k;

		while(find[0]==' '){

			if(strlen(find)==1)
			break;

			for(k=1;k<strlen(find);k++)
			find[k-1]=find[k];

			find[strlen(find)-1]= '\0';
		}

		while(find[strlen(find)-1]==' '){
			find[strlen(find)-1]='\0';
		}
		if(find[strlen(find)-1]=='\n'){
			find[strlen(find)-1]='\0';
		}
		temp1->cat=find;
		//printf("end one\n");

		temp1=temp1->next;
		//llength2--;

		//printf("end two\n");
	}
	//printf("out\n");
	//exit
	if(notfound==1){
		//ignore
		fprintf(stderr, "%s\n","Error: Parameter not found.");
		//fclose(fp);
		if(PRINT == 0 || PRINT ==3 )
			printf("three\n");
		return -1;
	}

	//Checking directory
	struct stat st = {0};

	//Creates directory if it DNE
	if(ddir!=NULL&&stat(ddir, &st)== -1){
		//mkdir(ddir, 0700);
		//fclose(fp);
		//if(PRINT == 0)
		fprintf(stderr, "%s\n","Error: Invalid Output Directory.");
		return(-1);
	}


	//creating new CSV file

	char* fileStub = (char*)malloc((100)*sizeof(char));
	strcpy(fileStub,"Allfiles");

	/*	Previous removing ".csv" from ending
	for(i=0;i<4;i++)
	fileStub[strlen(fileStub)-1] = '\0';
	*/

	char* newFileName = (char*) malloc(strlen(argv)+100*sizeof(char));

	if(ddir!=NULL){
		if(PRINT!= 1){
			printf("ddir is not null\n");
			printf("ddir is: %s\n", ddir);
			//printf("idir is: %s\n", idir);
		}
		strcpy(newFileName,ddir);
		strcat(newFileName,fileStub);
	}else{
		if(PRINT!= 1){
			printf("ddir is: %s\n", ddir);
			//printf("idir is: %s\n", idir);
		}
		//strcpy(newFileName,idir);
		strcpy(newFileName,fileStub);
	}
	strcat(newFileName,"-sorted-");
	strcat(newFileName,argv);
	strcat(newFileName,".csv");
	if(PRINT== 0 || PRINT ==3 )
		printf("%s\n",newFileName);

	//fclose(fp);

	FILE *new;
	new = fopen(newFileName, "w+");
	mergesorter(&(front->next));

	Node * temp = front;
	//long llength3 =llength;
	while (temp!=NULL){
		fprintf(new,"%s", temp->data);
		temp = temp->next;
		//llength3--;
	}

	fclose(new);
	if(PRINT == 0 || PRINT ==3 )
		printf("five\n");
	/*
	free(potato);
	free(potato1);
	free(copystring1);
	free(copystring2);
	free(found);
	free(fileStub);
	free(newFileName);
	free(buffer);
	free(temp1);
	free(temp);
	free(front);
	 */
	return 0;
}

