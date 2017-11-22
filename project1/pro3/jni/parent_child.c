
/*
	This file generate a process and its child process. In child process it calls ptree. After child process completes, parents completes and print a message.
*/
#include <unistd.h>  
#include <stdio.h>
#include <sys/wait.h> 
#include<sys/types.h>

int main(){
    pid_t fpid=fork();//Child process created
    if(fpid<0){
        printf("fork error!\n");
        return -1;
    }
    if(fpid==0){//child process there
        printf("515021910459Child is %d\n",getpid());
        if(execl("/data/misc/test_ptree","test_ptree",NULL)<0){
            printf("Execl error! Check if test_ptree is in the path /data/misc\n");
            return -1;
        }
            
    }else{//parent process there
        printf("515021910459Parent is %d\n",getpid());
        waitpid(fpid,NULL, 0);//Wait for the child process finished.
        printf("Child process finished, parent process exited!\n");
        
    }
    return 0;
}
