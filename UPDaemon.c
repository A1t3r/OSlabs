#define NN 32
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <resolv.h>
#include <sys/socket.h>
#include <sys/un.h>

sem_t sem1;
int flag = 0;
int Dflag = 0;

void sigalarm_handler(){
flag=1;
}

void term_handler(){
Dflag=1;
}

int Daemon(char*path[]){
    signal(SIGALRM,sigalarm_handler);
    signal(SIGTERM,term_handler);

  //const char *semname="/semname";
   sem_init(&sem1, 0, 0);
   sem_post(&sem1);
   //sem1 = sem_open(semname,O_CREAT);//|O_EXCL);
  // sem_post(sem1);
  // sem_wait(&sem1);
    char* tmp=NULL;
    char* newpar[NN];
    for(int i=0;i<NN;i++){
          newpar[i]=NULL;
      }

for(;;) {
    if (flag == 1) {
    int fd;
    char buf[256]={};
    fd = open(path[1],O_CREAT|O_RDWR,S_IRWXU);
    if(read(fd, buf, sizeof(buf)-1)==-1) exit(9);
     close(fd);
/*  
    int cnt=0;
    int cnnt=0;
    int mcnt=4;
    for(int i =0;i< sizeof(buf);i++) {
       if (buf[i] == '\n'){ 
            cnt++;
            cnnt=0;
       }
       if (buf[i] == ' '){
            cnnt++;
            if (mcnt<cnnt) mcnt=cnnt;
       }
    }
    mcnt++;
*/
  //=(char**)calloc((mcnt),sizeof(char*));
 // for(int i=0;i<mcnt;i++){
   //  newpar[i]=(char*)calloc(32,sizeof(char));
   //  }

    int k=0;
    int n =0;
    int j =0;
    int frst=0;

   tmp = (char*)calloc(32,sizeof(char));
   for(int i =0;i< strlen(buf);i++) {
   if (buf[i] == ' ') {
        tmp[j] = '\0';
        newpar[n] = tmp;
        tmp = (char *) calloc(32 , sizeof(char));
        j = 0;
        i++;
        n++;
       }
   if (buf[i] == '\n' || (buf[i]=='\0')) {
        tmp[j] = '\0';
        newpar[n] = tmp;
        tmp = (char *) calloc(32 , sizeof(char));
        j = 0;
        i++;
        n++;

        pid_t testpid;
        testpid = fork();
             if(testpid==0) {
                     sem_wait(&sem1);
                    // sem_wait(sem1);
                     int fd2 = open("UPdemLOG.txt",O_CREAT|O_RDWR,S_IRWXU);
                     lseek(fd2, 0, SEEK_END);
                     write(fd2,"\n NOW WORKING:\n",14);
                     write(fd2,newpar[0],strlen(newpar[0])); 
                     write(fd2,"\n",1);
                     close(fd2);
                     int fd3 = open("UPdemEXECVEOUT.txt",O_CREAT|O_RDWR,S_IRWXU);
                     lseek(fd3, 0, SEEK_END);
                     close(1);
                     dup2(fd3,1);
                     sem_post(&sem1);
                      //sem_post(sem1);
                     execve(newpar[0], newpar,NULL);
              }
              if(testpid > 0) {
                     flag = 0;
                     int status=0;
                     wait(&status);
              }

      for(int i=0;i<NN;i++){
          if(newpar[i]!=NULL) free(newpar[i]);
          newpar[i]=NULL;
      }
       n=0;
      }

   tmp[j] = buf[i];
   j++;
  
 }
 free(tmp);

}
    if(Dflag==1){
    //close(fd2);
    sem_destroy(&sem1);
    exit(0);
    }

}
   
}

int main(int argc, char* argv[],char* env[]){
    pid_t parpid;
   // signal(SIGALRM,sigalarm_handler);
    if((parpid=fork())<0) //--здесь мы пытаемся создать дочерний процесс главного процесса (масло масляное в прямом смысле)
    {                   //--точную копию исполняемой программы
        printf("\ncan't fork"); //--если нам по какойлибо причине это сделать не удается выходим с ошибкой.
        exit(1);                //--здесь, кто не совсем понял нужно обратится к man fork
    }

    if(parpid==0){
        printf("tst2 Process - %i\n ", getpid());
        setsid();
        Daemon(argv);
    }

    else if (parpid!=0) { //--если дочерний процесс уже существует{
        sleep(2);
        kill(parpid, SIGALRM);
       // sleep(2);
       // kill(parpid, SIGALRM);
        exit(0);       //--генерируем немедленный выход из программы(зачем нам еще одна копия программы)
    }


return 0;
}
