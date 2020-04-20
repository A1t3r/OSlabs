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

sem_t sem1;
int flag = 0;
int Dflag = 0;

void sigalarm_handler(int fd){
flag=1;
}

void term_handler(int fd){
Dflag=1;
}

int Daemon(char*path[]){
    signal(SIGALRM,sigalarm_handler);
    signal(SIGTERM,term_handler);
    int fd;
    char buf[256]={};
    fd = open(path[1],O_CREAT|O_RDWR,S_IRWXU);
   if(read(fd, buf, sizeof(buf)-1)==-1) exit(9);
    int tmpsize = strlen(buf);
// разбиени строки на подстроки с сохранением массива строк в динамической памяти + cоздание массива массивов строк для работы с несколькими командами
    int cnt=0;
    int cnnt=0;
    int mcnt=8;
    for(int i =0;i< sizeof(buf);i++) {
        if (buf[i] == '\n') {
            cnt++;
            cnnt=0;
        }
        if (buf[i] == ' '){
            cnnt++;
            if (mcnt<cnnt) mcnt=cnnt;}
    }
    mcnt++;
    cnt++;
    char** newtoexec=(char**)calloc((cnt),sizeof(char*));
    for(int i=0;i<cnt;i++){
        newtoexec[i]=(char*)calloc(32,sizeof(char));
    }
    char*** newpar=(char***)calloc((cnt),sizeof(char**));
    for(int i=0;i<cnt;i++){
        newpar[i]=(char**)calloc((mcnt),sizeof(char*));
        for(int j=0;j<mcnt;j++){
            newpar[i][j]=(char*)calloc(32,sizeof(char));
            newpar[i][j]=NULL;
        }
    }

    int k=0;
    int n =0;
    int j =0;
    int frst=0;
    newpar[n][0]="test";
    char* tmp = (char*)calloc(32,sizeof(char));
    for(int i =0;i< sizeof(buf)-1;i++) {
        if (buf[i] == '\n') {
            if (frst == 0) {
                k++;
                tmp[j] = '\0';
                newtoexec[n] = tmp;
            }
            else {
                tmp[j] = '\0';
                k++;
                newpar[n][k] = tmp;
            }
            tmp = (char *) calloc(32 , sizeof(char));
            n++;
            newpar[n][0]="test";
            k=0;
            j = 0;
            frst=0;
            continue;
        }
        if (buf[i] == ' ') {
            if (frst == 1) {
                k++;
                tmp[j] = '\0';
                newpar[n][k] = tmp;
                tmp = (char *) calloc(32 , sizeof(char));
                j = 0;
                continue;
            } else {
                tmp[j] = '\0';
                newtoexec[n] = tmp;
                tmp = (char *) calloc(32 , sizeof(char));
                j = 0;
                frst = 1;
                continue;
            }
        }
        tmp[j] = buf[i];
        j++;
    }
    tmp[j] = '\0';
    if(frst==0){
        newtoexec[n]=tmp;
    }
    else{
        newpar[n][k+1]=tmp;
        newpar[n][k+2]=NULL;
    }
    free(tmp);
/////////////////////////////////////////////////////////////////////////
    close(1);
    dup2(fd, 1);

  // const char *semname="/semname";
   sem_init(&sem1, 0,0);
   sem_post(&sem1);
  // sem1 = sem_open(semname,O_CREAT);//|O_EXCL);
  // sem_wait(&sem1);

    int fd2 = open("UPdemLOG.txt",O_CREAT|O_RDWR,S_IRWXU);

for(;;) {
    if (flag == 1) {
        
        for (int i=0;i<cnt-1;i++){
 
             pid_t testpid;
             testpid = fork();
             if(testpid==0) {
                     sem_wait(&sem1);
                    write(fd2,"\n NOW WORKING: \n",13);
                    write(fd2,newtoexec[i],strlen(newtoexec[i])); 
                     sem_post(&sem1);
                     execve(newtoexec[i], newpar[i],NULL);
              }
              if(testpid > 0) {
                     flag = 0;
              }

          }
      }
    if(Dflag==1){
     close(fd);
     close(fd2);
     sem_destroy(&sem1);
     exit(0);
}
}
    //close(fd);
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
