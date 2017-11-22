/*
	Burger Buddies Problem
	Cooks, Cashiers, and Customers are each modeled as a thread.
 	Cashiers sleep until a customer is present.
	A Customer approaching a cashier can start the order process.
	A Customer cannot order until the cashier is ready. Once the order is placed, a cashier has to get a burger from the rack.
	If a burger is not available, a cashier must wait until one is made.
	The cook will always make burgers and place them on the rack.
	The cook will wait if the rack is full.
	There are NO synchronization constraints for a cashier presenting food to the customer.

	Usage: ./BurgerBuddies #Cooks #Cashiers #Customers #RackSize
*/

#include<stdio.h>
#include<semaphore.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#define THREADNUM 10000


int Cooks;
int Cashiers;
int Customers;
int Racksize;
int create=-1;

sem_t mutex;
sem_t getburger;
sem_t have_burger;
sem_t service;  
sem_t empty;

int thread_id[THREADNUM];
void CookerWork(void * pointer)
{
/*
	The cooker wait semphore (empty)
	Cook a burger
	Post semphore (have_burger)
*/
	int num=*((int*)(pointer));
	while(1)
	{
		sem_wait(&empty);
		printf("Cook [%d] makes a burger.\n",num);
		sem_post(&have_burger);
		usleep(200);
	}
}
void CashierWork(void *  pointer)
{
/*
	Cashier wait semaphore (service)
	Cashier accepts an order
	Cashier wait semaphore (have_burger)
	Cashier takes a burger to a customer
	Post semaphore (empty)
	Post semaphore (getburger)
*/
	int num=*((int *)(pointer));
	while(1)
	{
		sem_wait(&service);
		printf("Cashier [%d] accepts an order.\n",num);
		sem_wait(&have_burger);
		printf("Cashier [%d] takes a burger to customer.\n",num);
		sem_post(&empty);
		sem_post(&getburger);
		usleep(50);
	}
}
void CustomerWork(void * pointer)
{
/*
	Customer comes
	Customer post semaphore (service)
	Customer wait semaphore (getburger)
*/
	int num=*((int*)(pointer));
	printf("Customer [%d] comes.\n",num);
	sem_post(&service);
	sem_wait(&getburger);
}

int main(int argc,char ** argv)
{
	if (argc != 5) {
        printf("USAGE: YOU NEED TO INPUT FOUR ARGUMENTS!!! \n");
        return -1;
    	}

	Cooks=atoi(argv[1]);
	Cashiers=atoi(argv[2]);
	Customers=atoi(argv[3]);
	Racksize=atoi(argv[4]);
	//Get arguments
	if(Cooks<=0||Cashiers<=0||Customers<=0||Racksize<=0)
	{
		printf("INVALID ARGUMENTS!!!\n");
		return -1;
	}
	//Check invalid arguments
	printf("Cooks [%d], Cashiers [%d], Customers [%d] \nBegin Run\n",Cooks,Cashiers,Customers);
	int j=0;
	int i=0;

	for(j=0;j<THREADNUM;++j)
		thread_id[j]=j+1;

	pthread_t cookThread[Cooks];
	pthread_t cashierThread[Cashiers];
	pthread_t customerThread[Customers];

	//Initialize semaphores and check error.
	create=sem_init(&service,0,0);
	if(create==-1)
	{
		printf("SEMAPHORE SERVICE INITIAL FAILED!!!\n ");
		return -1;
	}
	create=sem_init(&getburger,0,0);
	if(create==-1)
	{
		printf("SEMAPHORE GETBURGER INITIAL FAILED!!!\n ");
		return -1;
	}
	create=sem_init(&have_burger,0,0);
	if(create==-1)
	{
		printf("SEMAPHORE HAVEBURGER INITIAL FAILED!!!\n ");
		return -1;
	}
	
	create=sem_init(&empty,0,Racksize);
	if(create==-1)
	{
		printf("SEMAPHORE EMPTY INITIAL FAILED!!!\n ");
		return -1;
	}

//Create process
	for(i=0;i<Cooks;++i)
	{
		int * para=thread_id+i;
		create=pthread_create(&cookThread[i],NULL,(void*)CookerWork,(void*) para);
		if(create!=0)
		{
			printf("COOK PTHREAD CREATED FAIL!!!\n");
			return -1;
		}
	}
	for(i=0;i<Cashiers;++i)
	{
		int * para=thread_id+i;
		create=pthread_create(&cashierThread[i],NULL,(void*)CashierWork,(void*) para);
		if(create!=0)
		{
			printf("CASHIER PTHREAD CREATED FAIL!!!\n");
			return -1;
		}
	}
	for(i=0;i<Customers;++i)
	{
		int * para=thread_id+i;
		create=pthread_create(&customerThread[i],NULL,(void*)CustomerWork,(void*) para);
		if(create!=0)
		{
			printf("CUSTOMER PTHREAD CREATED FAIL!!!\n");
			return -1;
		}
	}
//Join process
	for(i=0;i<Customers;++i)
	{
		
		create=pthread_join(customerThread[i],NULL);
		if(create!=0)
		{
			printf("CUSTOMER PTHREAD JOINED FAIL!!!\n");
			return -1;
		}
	}
//Destroy semaphores
	sem_destroy(&service);
	sem_destroy(&getburger);
	sem_destroy(&have_burger);
	sem_destroy(&mutex);
	printf("End Run\n");
	return 0;


}
