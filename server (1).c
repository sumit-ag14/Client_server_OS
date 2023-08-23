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
#define  CLIENT_NAME_WRITTEN     0
#define  KEY_ASSIGNED      1
#define  TAKEN     2
#define  COMPLETE  3
#define  NOT_UNIQUE 4
#define  REQUEST_MADE 5
#define  QUERY_SOLVED 6
#define  UNREGISTER 7

int gc=0;
bool flag=0;

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
	int nor
};
// struct threadValue{
// 	int Shmid_client;
// 	struct commMemory  *Shmptr_client;
// };

void *worker(void *data)
{
	
	struct commMemory *ptr = (struct commMemory*)(data);
	//int id=((struct threadValue*)data)->Shmptr_client;
	while(1){
		while(ptr->status!=REQUEST_MADE);	
		gc+=1;
		flag=1;
		printf("parent id of thread %d\n", getppid());
		if(ptr->top==1){
			if(ptr->operator==1){
				printf("Addition Function Called.\n");
				ptr->result=ptr->a+ptr->b;

			}
			if(ptr->operator==2){
				printf("Subtraction Function Called.\n");

				ptr->result=ptr->a-ptr->b;
			}
			if(ptr->operator==3){
				printf("Multiplication Function Called.\n");

				ptr->result=ptr->a*ptr->b;
			}
			if(ptr->operator==4){
				printf("Division Function Called.\n");

				ptr->result=ptr->a/ptr->b;
			}
		}else if(ptr->top==2){
				printf("Even ODD Function Called.\n");

			if(ptr->a&1){
				ptr->result=1;
			}else{
				ptr->result=0;
			}
		}else if(ptr->top==3){
				printf("Prime Function Called.\n");

			for(int i=2;i<ptr->a;i++){
				if(ptr->a%i==0){
					ptr->result =1;
					break;
				}else{
					ptr->result=0;
				}
			}
		}
		else if(ptr->top==4){
				printf("Unregister Function Called.\n");

			ptr->status=UNREGISTER;
			//pthread_kill();
			return NULL;
		}
		else if(ptr->top==5){
			//printf("Client name: %s \n",ptr->name[0]);
			printf("Number of requests made: %d \n",ptr->nor);
		}
		ptr->status=QUERY_SOLVED;
	}
	
}


void  main(int  argc, char *argv[])
{

	
	int ShmaID = shm_open("connectChannel", O_RDWR | O_EXCL, 0666);
     if (ShmaID != -1) {
        printf("Deleting any existing memory");
        shm_unlink("connectChannel");
        return ; 
     }
	int brk_cdn;

     key_t          ShmKEY;
     int            ShmID;
     struct Memory  *ShmPTR;
     
     // int gc=0;
	// int pgc=0;
     int c_ind=0;
     char names [20][20];
     struct commMemory* shmptr_arr[20];
     int shmid_client_arr[20];
     char key_strs[20][20];
     int memcheck[20];
     for(int i=0;i<20;i++) memcheck[i]=1;

     ShmID = shm_open("connectChannel",O_CREAT | O_RDWR, 0666);
     if (ShmID ==-1) {
          printf("*** shmget error open (server) *** %d\n",ShmID);
          exit(1);
     }
     //printf("%d\n",ShmID);
     if (ftruncate(ShmID, sizeof(struct Memory)) == -1) printf("*** shmget error truncate (server) ***\n");

     ShmPTR = mmap(NULL, sizeof(struct Memory),PROT_READ | PROT_WRITE, MAP_SHARED, ShmID, 0);
     printf("Server has received a shared memory and Connect channel is created ...\n");
    

     int temp_key=52;
     ShmPTR->status=NOT_READY;

     while(1){
		while (ShmPTR->status != CLIENT_NAME_WRITTEN && ShmPTR->status!= COMPLETE);

	
			for(int i=0;i<c_ind;i++){
				// printf("dsfjs \n");
				if(strcmp(ShmPTR->name[0],names[i])==0){
					// printf("%s\n",names[i]);
					// printf("%s\n",ShmPTR->name[0]);
					ShmPTR->status=NOT_UNIQUE;
					while(ShmPTR->status != CLIENT_NAME_WRITTEN);
					i=-1;
					
				}
			}
			strcpy(names[c_ind],ShmPTR->name[0]);
	
      		

     		//assign shared memory
		key_t          Shmkey_client;
     	int            Shmid_client;
     	struct commMemory  *Shmptr_client;
		Shmkey_client = ftok(".",temp_key);
     	ShmPTR->key[c_ind]=temp_key++;
     	char key_str[20];
		printf("%d", sizeof( struct commMemory));
		


		sprintf(key_str,"%d",Shmkey_client);
		Shmid_client = shm_open(key_str,O_CREAT | O_RDWR, 0666);
	     if (Shmid_client ==-1) {
	          printf("*** shmget error open (server) *** %d\n",Shmid_client);
	          exit(1);
	     }
	     if (ftruncate(Shmid_client, sizeof(struct commMemory)) == -1) printf("*** shmget error truncate (server) ***\n");

	     Shmptr_client= mmap(NULL, sizeof(struct commMemory),PROT_READ | PROT_WRITE, MAP_SHARED, ShmID, 0);
	     printf("Server has created Communication Channel ...\n");	


     		
		printf("Please start the client in another window... %d\n", c_ind);
		
		ShmPTR->cur_ind=c_ind;
     	ShmPTR->status = KEY_ASSIGNED;
     	
     	while (ShmPTR->status != TAKEN);

     	strcpy(key_strs[c_ind],key_str);
     	shmptr_arr[c_ind]=Shmptr_client;
     	shmid_client_arr[c_ind]=Shmid_client;
     	c_ind+=1;
		
		//Refreshing status to take new user entry
		ShmPTR->status= NOT_READY;
		
		//Thread value initialization
		struct threadValue *smptr;
		

		//Printing Registered Clients
		int temp_cnt=0;
		printf("The following clients are registered currently: \n");
		for(int i=0;i<c_ind;i++){
			if(names[i][0]!='\0'){
				printf ("%s \n", names[i]);
				temp_cnt++;
			}
		}
		printf("Total number of clients are :%d \n", temp_cnt);


		pthread_t th1;
		int t1=pthread_create(&th1, NULL, worker, Shmptr_client);
		if(t1!=0){
		printf("Error in creating thread\n");
		}
		else {
		printf("Thread created succesfully with pid: %d\n",th1);
		}

		
		
		do{

			if(flag){
				printf("Total number of requests made: %d\n",gc);
				//pgc=gc;
				flag=!flag;
			}
			int s_ind=0;
			for(int i=0;i<c_ind;i++){
				if(memcheck[i]){
					struct commMemory* tptr=shmptr_arr[i];
					if(tptr->status==UNREGISTER){
						strcpy(names[i],"");
						// printf("dereg\n");
						tptr->status=6;
						if (munmap(tptr, sizeof(struct commMemory)) == -1) {
							perror("munmap\n");
							exit(1);
						}
							// printf("dereg\n");


						// // unlink the shared memory object
						if (shm_unlink(key_strs[i]) == -1) {
							perror("shm_unlink\n");
							exit(1);
						}
						printf("Unlinked succesfully\n");
						memcheck[i]=0;
						//shmdt((void *) tptr);
						//shmctl(shmid_client_arr[i], IPC_RMID, NULL);
					}

				}
				
			}
			
		}while(ShmPTR->status!=CLIENT_NAME_WRITTEN);

     }
     

     //------------------Detachment Procedure----------------------------//

     	//printf("Server has detected the completion of its child...\n");
     	//shmdt((void *) ShmPTR);
     	//printf("Server has detached its shared memory...\n");
     	//shmctl(ShmID, IPC_RMID, NULL);
     	//printf("Server has removed its shared memory...\n");
     	//printf("Server exits...\n");
     	//exit(0);

    //------------------Detachment Procedure----------------------------//
}
