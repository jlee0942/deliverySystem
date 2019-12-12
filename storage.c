#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"

/* 
  definition of storage cell structure ----
  members :
  int building : building number of the destination
  int room : room number of the destination
  int cnt : number of packages in the cell
  char passwd[] : password setting (4 characters)
  char *contents : package context (message string)
*/
typedef struct {
	int building;
	int room;
	int cnt;
	char passwd[PASSWD_LEN+1];
	
	char *context;
} storage_t;

//create structure instance
storage_t store;

static storage_t** deliverySystem; 			//deliverySystem
static int storedCnt = 0;					//number of cells occupied
static int systemSize[2] = {0, 0};  		//row/column of the delivery system
static char masterPassword[PASSWD_LEN+1];	//master password



// ------- inner functions ---------------

//print the inside context of a specific cell
//int x, int y : cell to print the context
static void printStorageInside(int x, int y) {
	printf("\n------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n");
	if (deliverySystem[x][y].cnt > 0)
		printf("<<<<<<<<<<<<<<<<<<<<<<<< : %s >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", deliverySystem[x][y].context);
	else
		printf("<<<<<<<<<<<<<<<<<<<<<<<< empty >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
		
	printf("------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n\n");
}

//initialize the storage
//set all the member variable as an initial value
//free store.context
//int x, int y : cell coordinate to be initialized
static void initStorage(int x, int y) {
	
	deliverySystem[x][y].building=NULL;
	deliverySystem[x][y].room=NULL;
	deliverySystem[x][y].cnt=NULL;
	deliverySystem[x][y].passwd[0]=NULL;
	deliverySystem[x][y].context=NULL;
	free(store.context);
	store.context=NULL;
}

//get password input and check if it is correct for the cell (x,y)
//int x, int y : cell for password check
//return : 0 - password is matching, -1 - password is not matching
static int inputPasswd(int x, int y) {

	char input[PASSWD_LEN+1];
	printf("Input your password:");
	
	scanf("%4s", input);	

	if(strcmp(input,masterPassword)!=0)	//input is not masterpassword
	{
		if(strcmp(input, deliverySystem[x][y].passwd)!=0)	//input is not masterpassword nor password
		{	
			return -1;
		}
	}
	
	return 0;
}





// ------- API function for main.c file ---------------

//backup the delivery system context to the file system
//char* filepath : filepath and name to write
//return : 0 - backup was successfully done, -1 - failed to backup
int str_backupSystem(char* filepath) {
	
	int i,j;
	
	FILE* fp;
	fp = fopen(filepath,"w");
	
	if((fp)==NULL)
	{
		return -1;
	}
	fprintf(fp, "%i %i\n", systemSize[0], systemSize[1]);
	fprintf(fp,"%s\n", masterPassword);
	
	//rewrite currently existing storages on textfile.	
	for(i=0;i<systemSize[0];i++)
		for (j=0;j<systemSize[1];j++)
		{

			if((deliverySystem[i][j].cnt)!=0)
			{
				fprintf(fp, "%i %i %i %i %s %s\n", i, j, deliverySystem[i][j].building, deliverySystem[i][j].room, deliverySystem[i][j].passwd, deliverySystem[i][j].context);	
			}
		
		}	
	
	fclose(fp);
		
	return 0;
}



//create delivery system on the double pointer deliverySystem
//char* filepath : filepath and name to read config parameters (row, column, master password, past contexts of the delivery system
//return : 0 - successfully created, -1 - failed to create the system
int str_createSystem(char* filepath) {
	int i,j;
	int x,y;		//for saving indexnumber
	char msg[MAX_MSG_SIZE+1];	//for saving message context	
	
	FILE* fp;
	fp = fopen(filepath,"r");
	
	
	if((fp)==NULL)
	{
		return -1;	
	}
	//get size of system&masterpassword
	fscanf(fp,"%d %d", &systemSize[0], &systemSize[1]);
	fscanf(fp,"%s", masterPassword);
	
	//create memory for pointer
	deliverySystem=(storage_t**)malloc(systemSize[0]*sizeof(storage_t*));
	for(i=0;i<systemSize[0];i++)
	{
		deliverySystem[i] = (storage_t*)malloc(systemSize[1]*sizeof(storage_t)); 
	}
	
	//when failed
	if (deliverySystem==NULL)
	{
		printf("error");					
	}
	
	//initialize cnt
	for (i=0;i<systemSize[0];i++)
	{
		for (j=0;j<systemSize[1];j++)
		{
			deliverySystem[i][j].cnt=0;
		}
	}
	
	
	//read from file & initialize struct. data
	while(fscanf(fp,"%d %d",&x,&y)==2)
	{
		fscanf(fp,"%d %d %s %s", &store.building, &store.room, store.passwd, msg);		
		storedCnt++;				
		//allocate memory of length of delivery context
		store.context = (char*)malloc((strlen(msg)+1)*sizeof(char));
		strcpy(store.context,msg);
		deliverySystem[x][y] = store;
		deliverySystem[x][y].cnt=1;
	}

	//close file
	fclose(fp);

	return 0;
}


//free the memory of the deliverySystem & store.context 
void str_freeSystem(void) {
	
	int i;
	
	for(i=0;i<systemSize[0];i++)
	{
		free(deliverySystem[i]);
	}
	free(deliverySystem);
	deliverySystem=NULL;
	free(store.context);
	store.context=NULL;

}



//print the current state of the whole delivery system (which cells are occupied and the destination of the each occupied cells)
void str_printStorageStatus(void) {
	int i, j;
	printf("----------------------------- Delivery Storage System Status (%i occupied out of %i )-----------------------------\n\n", storedCnt, systemSize[0]*systemSize[1]);
	
	printf("\t");
	for (j=0;j<systemSize[1];j++)
	{
		printf(" %i\t\t",j);
	}
	printf("\n-----------------------------------------------------------------------------------------------------------------\n");
	
	for (i=0;i<systemSize[0];i++)
	{
		printf("%i|\t",i);
		for (j=0;j<systemSize[1];j++)
		{
			if (deliverySystem[i][j].cnt > 0)
			{
				printf("%i,%i\t|\t", deliverySystem[i][j].building, deliverySystem[i][j].room);
			}
			else
			{
				printf(" -  \t|\t");
			}
		}
		printf("\n");
	}
	printf("--------------------------------------- Delivery Storage System Status --------------------------------------------\n\n");
}



//check if the input cell (x,y) is valid and whether it is occupied or not
int str_checkStorage(int x, int y) {
	if (x < 0 || x >= systemSize[0])
	{
		return -1;
	}
	
	if (y < 0 || y >= systemSize[1])
	{
		return -1;
	}
	
	return deliverySystem[x][y].cnt;	
}


//put a package (msg) to the cell
//input parameters
//int x, int y : coordinate of the cell to put the package
//int nBuilding, int nRoom : building and room numbers of the destination
//char msg[] : package context (message string)
//char passwd[] : password string (4 characters)
//return : 0 - successfully put the package, -1 - failed to put
int str_pushToStorage(int x, int y, int nBuilding, int nRoom, char msg[MAX_MSG_SIZE+1], char passwd[PASSWD_LEN+1]) {
	
	//in order to add info on txt file
	store.context = (char*)malloc((strlen(msg)+1)*sizeof(char));		//allocate memory for context
	
	if(store.context==NULL)
	{
		printf("error on allocating memory for message");
		return -1;
	}
	
	strcpy(store.context,msg);											//copy message to deliverySystem
	
	deliverySystem[x][y].building = nBuilding;
	deliverySystem[x][y].room = nRoom;
	strcpy(deliverySystem[x][y].passwd,passwd);
	deliverySystem[x][y].context= store.context;
	deliverySystem[x][y].cnt=1;
	
	printf("%i %i %i %i %s %s",x, y, deliverySystem[x][y].building,deliverySystem[x][y].room,deliverySystem[x][y].passwd,deliverySystem[x][y].context);
	
	storedCnt++;		//add to current number of occupied cell
	
	return 0;

}



//extract the package context with password checking
//after password checking, then put the msg string on the screen and re-initialize the storage
//int x, int y : coordinate of the cell to extract
//return : 0 - successfully extracted, -1 = failed to extract
int str_extractStorage(int x, int y) {

	//if pswd does not match -> return -1;
	if (inputPasswd(x,y)!=0)
	{
		return -1;
	}	
	//if pswd matches -> initStorage(x,y), print context & return 0;
	printf("extracting the storage (%i, %i)...\n", x, y );
	
	printStorageInside(x,y);	//print stored message for you
	initStorage(x,y);	//initialize
	
	storedCnt--;		//subtract from remaining number of occupied cell
		
	return 0;		
}


//find my package from the storage
//print all the cells (x,y) which has my package
//int nBuilding, int nRoom : my building/room numbers
//return : number of packages that the storage system has
int str_findStorage(int nBuilding, int nRoom) {
	
	int i, j;
	int cnt=0;		//set initial cnt as zero
	
	//find matching information
	for(i=0; i<systemSize[0];i++)
	{
		for(j=0; j<systemSize[1];j++)
		{
			if(nBuilding == deliverySystem[i][j].building)
			{
				if(nRoom == deliverySystem[i][j].room)
				{ 	
					printf(" -----------> Found a package in (%i,%i)\n", i, j);		
					//count num of matching info	
					cnt++;	
				}
			}
		}
	}
	return cnt;
}
