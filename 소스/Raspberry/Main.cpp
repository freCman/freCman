#include"Header.h"

int sm_fd[2];
char sm_buf[40];
int _proc_handle;

__sighandler_t kill_proc(){
	if(_proc_handle == 0)
	kill(_proc_handle, SIGKILL);
}


int main(){


 if (pipe(sm_fd) != 0 )
    {
        perror("pipe");
    return 1;
    }
 

    if ((_proc_handle=fork()) == 0)
    {
        printf("Starting up Python interface...\n");
        dup2(sm_fd[0], STDIN_FILENO);
 
        close(sm_fd[1]);
        execlp("python","python","mybluetooth.py", (char*)NULL);
        perror("execlp");
        printf("Error executing Python.\n");
        exit(1);
    }
	close(sm_fd[0]);

	signal(SIGKILL,(__sighandler_t)kill_proc);


	FingerKeyboard* fk = new FingerKeyboard();	
	fk->programSetUp(0);
	fk->programRun();	
	fk->programExit();
	delete fk;
}
