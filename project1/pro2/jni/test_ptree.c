/*
	This file test the new system call ptree. It calls ptree and print the entire process tree (in DFS order) using tabs to indent children with respect to their parents.
*/

#include <stdio.h>
#include<stdlib.h>
struct prinfo {
    pid_t parent_pid;/* process id of parent, set 0 if it has no parent*/
    pid_t pid;/* process id */
    pid_t first_child_pid;/* pid of youngest child, set 0 if it has no child */
    pid_t next_sibling_pid; /* pid of older sibling, set 0 if it has no sibling*/
    long state;/* current state of process */
    long uid;/* user id of process owner */
    char comm[64];/* name of program executed */
};

void dfs_private(struct prinfo * d,int * visited ,int t,int i,int flag);
//The private function of dfs

void dfs(struct prinfo * d,int * visited ,int t)//The public function of dfs
{
	int i;
	for (i=0;i<t;++i)
	{
		if(d[i].parent_pid==0&&visited[d[i].parent_pid]==0)
			{
				
				dfs_private(d,visited, t,i,0);
			}
	}

}

int locate(struct prinfo * d,int findpid,int t)
{
	int k;
	for(k=0;k<t;++k)
		if(d[k].pid==findpid)
			return k;
	printf("NOT FOUND!!!\n");
	return -1;
}
//This function find the index of pid

void dfs_private(struct prinfo * d,int * visited ,int t,int i,int flag)
{
	
	int times;
	for(times=0;times<flag;times++)
		printf("\t");
	//Print space indent to show process relation

	printf("%s,%d,%ld,%d,%d,%d,%ld\n",d[i].comm,d[i].pid,d[i].state,d[i].parent_pid,d[i].first_child_pid,d[i].next_sibling_pid,d[i].uid);
	visited[i]=1;
	if(d[i].first_child_pid !=0&&visited[locate(d,d[i].first_child_pid,t)==0])
	{
		dfs_private(d,visited,t,locate(d,d[i].first_child_pid,t),flag+1);
	}

	if(d[i].next_sibling_pid!=0&&visited[locate(d,d[i].next_sibling_pid,t)==0])
	{
		dfs_private(d,visited,t,locate(d,d[i].next_sibling_pid,t),flag);
	}
	return;
}
int main()
{
	//printf("This is a test\n");
	int n=1,i,t;
	int * visited;
    struct prinfo * d;
    d=malloc(sizeof(struct prinfo));
    if(d==0)
		{
			printf("MALLOC ERROR!!!\n");
			return -1;
		}
	t=syscall(356,d,&n);
	
	while(t>n)
	{
		n*=2;
		free(d);
		d=malloc(sizeof(struct prinfo)*n);
		if(d==0)
		{
			printf("MALLOC ERROR!!!\n");
			return -1;
		}
		t=syscall(356,d,&n);
	}
	/*
	Use double space algorithm to find the propriate value of n
	*/

	visited=malloc(sizeof(int)*t);
	for (i=0;i<t;++i)
		visited [i]=0;
	dfs(d,visited,t);
	//printf("Test is end\n");
	
	return 0;
}
