#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>

#define SHM_SIZE 1024

#define  NOT_READY  -1
#define  CLIENT_NAME_WRITTEN      0
#define  KEY_ASSIGNED      1
#define  TAKEN      2
#define  COMPLETE  3
#define  NOT_UNIQUE 4
#define  REQUEST_MADE 5
#define  QUERY_SOLVED 6
#define  UNREGISTER 7


struct Memory {
	int  status;
	char name[1][20];
	int key[2];
	int st_ind;
	int cur_ind;

};
struct commMemory{
	int a;
	int b;
	int  status;
	int operator;
	int top;
	int result;
	int nor;
};

void  main(void)
{
    


     key_t          ShmKEY;
     int            ShmID;
     struct Memory  *ShmPTR;
     char naam[50];
     
     
    ShmID = shm_open("connectChannel", O_RDWR | O_EXCL, 0666);
    if (ShmID == -1) {
        printf("Shared memory does not exist\n");
        return 1;
    }
    else {
        printf("Shared memory exists\n");

    }


     //printf("%d\n",ShmID);
     if (ftruncate(ShmID, sizeof(struct Memory)) == -1) printf("*** shmget error truncate (server) ***\n");

     ShmPTR = mmap(NULL, sizeof(struct Memory),PROT_READ | PROT_WRITE, MAP_SHARED, ShmID, 0);
     printf("Server has received a shared memory of four integers...\n");


     //ShmPTR->cur_ind=0;
     //struct map* ptr= ShmPTR->data;
     key_t          Shmkey_client;
     int            Shmid_client;
     struct commMemory  *Shmptr_client;
     struct commMemory  *ptr;
     
     for(int i=0;i<1;i++){
	//Taking name
	printf("Kindly enter your name \n");
	scanf("%s",ShmPTR->name[0]);
	strcpy(naam,ShmPTR->name[0]);
	ShmPTR->status= CLIENT_NAME_WRITTEN;

	//Waiting Until some Key gets assigned to our user

     	
	//--------A unique client creating--------//
	while(ShmPTR->status != KEY_ASSIGNED){
		if(ShmPTR->status == NOT_UNIQUE){
			printf("Kindly enter your unique name \n");
			scanf("%s",ShmPTR->name[0]);
			ShmPTR->status= CLIENT_NAME_WRITTEN;
		}
		
	}
	//-------------------A unique client created------------------------------//


	//---------------------------Attaching to the shared memory---------------------------//
	
	Shmkey_client=ftok(".",ShmPTR->key[ShmPTR->cur_ind]);
	// printf("%d\n",Shmkey_client);
	char key_str[20];
	sprintf(key_str,"%d",Shmkey_client);
	// Shmid_client = shmget(Shmkey_client,sizeof( struct commMemory), IPC_CREAT | 0666);
     // 		if (Shmid_client < 0) {
     //      		printf("*** shmget error (client) ***\n");
     //      		exit(1);
     // 		}
     
     // 		Shmptr_client = (struct commMemory *) shmat(Shmid_client, NULL, 0);
     // 		if ((int) Shmptr_client == -1) {
     //      		printf("*** shmat error (client) ***\n");
     //      		exit(1);
     // 		}
	// 	printf("   Client has attached the shared memory...\n");
	// 	ptr=Shmptr_client;

	Shmid_client = shm_open(key_str, O_RDWR, 0666);
	     if (Shmid_client ==-1) {
	          printf("*** shmget error open (server) *** %d\n",Shmid_client);
	          exit(1);
	     }
	     if (ftruncate(Shmid_client, sizeof(struct commMemory)) == -1) printf("*** shmget error truncate (server) ***\n");

	     Shmptr_client= mmap(NULL, sizeof(struct commMemory),PROT_READ | PROT_WRITE, MAP_SHARED, ShmID, 0);
	     printf("Server has received a shared memory of four integers...\n");
	     ptr=Shmptr_client;
	//-------------------------------Attaching completed-------------------------------------//

	
 
     printf("   Key assigned to %s is %d \n", ShmPTR->name[0], ShmPTR->key[ShmPTR->cur_ind]);
     Shmptr_client->top=-1;
	Shmptr_client->status=6;
	Shmptr_client->nor=0;
	//Shmptr_client->b=0;
	ShmPTR->status = TAKEN;	
	//Wait until server assigns a new index for the next user
	while(ShmPTR->status!= NOT_READY);
  	
     }

	
     //-------------------------------Menu for Operations-------------------------------------//
	while(1){
		printf("Please choose the type of operation you wish to peform \n");
		printf("Press 1 for arithmetic operations \n");
		printf("Press 2 for Even/Odd operation \n");
		printf("Press 3 for Isprime operation \n");
		printf("Press 4 for Deregisteration\n");
		printf("Press 5 for Summary of Client\n");
		int ch;
		scanf("%d",&ch);
		if(ch==1){
			printf("Enter type of operation you wish to perform \n");
			printf("Press 1 for addition\n");
			printf("Press 2 for subtraction\n");
			printf("Press 3 for multiplication\n");
			printf("Press 4 for division\n");
			int op;
			scanf("%d",&op);
			printf("Enter two numbers\n");
			int a,b;
			scanf("%d %d",&a,&b);
			Shmptr_client->top=1;
			Shmptr_client->operator=op;
			Shmptr_client->a=a;
			Shmptr_client->b=b;
			
			ptr->status=REQUEST_MADE;
			//Waiting for result
			while(Shmptr_client->status!=QUERY_SOLVED);
			Shmptr_client->nor+=1;
			printf("Result is: %d \n",ptr->result);
		}else if(ch==2){
			printf("Enter a number\n");
			int a;
			scanf("%d",&a);
			printf("hi %d\n",a);
			Shmptr_client->top=2;
			Shmptr_client->a=a;

			ptr->status=REQUEST_MADE;
			//Waiting for result
			while(Shmptr_client->status!=QUERY_SOLVED);
			if(ptr->result==1){
				printf("Result is: odd \n");
			}else{
				printf("Result is: even \n");
			}
			Shmptr_client->nor+=1;
		}else if(ch==3){
			printf("Enter a number\n");
			int a;
			scanf("%d",&a);
			Shmptr_client->top=3;
			Shmptr_client->a=a;

			ptr->status=REQUEST_MADE;
			//Waiting for result
			while(Shmptr_client->status!=QUERY_SOLVED);
			if(ptr->result==1){
				printf("Result is: Not Prime \n");
			}else{
				printf("Result is: Prime \n");
			}
			Shmptr_client->nor+=1;
		}else if(ch==4){
			Shmptr_client->top=4;
			while(Shmptr_client->status!=QUERY_SOLVED);
			Shmptr_client->nor+=1;
			break;
		}
		else if(ch==5){
			//strcpy(Shmptr_client->name[0],naam);
			Shmptr_client->top=5;
			ptr->status=REQUEST_MADE;
			while(Shmptr_client->status!=QUERY_SOLVED);
		}
		else{
			printf("Kindly enter a valid input");
			Shmptr_client->nor+=1;
		}
	}
	//-------------------------------Menu for Operations-------------------------------------//
     	Shmptr_client->status=REQUEST_MADE;
     //ShmPTR->status= COMPLETE;

     
     printf("   Client has informed server data have been taken...\n");
     shmdt((void *) ShmPTR);
     printf("   Client has detached its shared memory...\n");
     printf("   Client exits...\n");
     exit(0);
}
