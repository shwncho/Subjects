#include<stdio.h>
#include<stdlib.h>

#define PROCESS_NUM 10



typedef struct PCB* PCBpointer;
typedef struct PCB{
    int pid;
    int priority;
    int arrival_time;
    int burst_time;
    int wt; // waiting time
    int rt; // response time
    int tt; // turnaround time
    int tq; // timequantum
    struct PCB* next;
}PCB;
struct PCB list[10];

typedef struct readyQueue{
    struct PCB* head;
    struct PCB* tail;
}readyQueue;



int ready_queue_num=0;
void read_process_list(const char* file_name){
    int i;
    int pid, priority, arrival_time,burst_time;
    FILE* fp;

    fp = fopen(file_name, "r");
    if(fp==NULL){
        printf("\nFile Could not be opened");
        return;
    }
    else{
        for(int i=0; i<PROCESS_NUM; i++){
            fscanf(fp, "%d %d %d %d", &list[i].pid,&list[i].priority,&list[i].arrival_time,&list[i].burst_time);
            list[i].wt=0;
            list[i].rt=-1;
            list[i].tt=0;
            list[i].tq=0;
            list[i].next=NULL;
        }
    }
    fclose(fp);
}


// job queue에 프로세스들 넣기
PCBpointer jobQueue[PROCESS_NUM];

PCBpointer runningProcess=NULL;
PCBpointer prevRunningProcess=NULL;

void init_job_queue(){
    int i;
    for(i=0; i<PROCESS_NUM; i++){
        jobQueue[i]=NULL;
    }
}

void insert_job_queue(){
    
    for(int i=0; i<PROCESS_NUM; i++){
        jobQueue[i]=(PCBpointer)malloc(sizeof(struct PCB));
        jobQueue[i]->pid=list[i].pid;
        jobQueue[i]->priority=list[i].priority;
        jobQueue[i]->arrival_time=list[i].arrival_time;
        jobQueue[i]->burst_time=list[i].burst_time;
        jobQueue[i]->wt=list[i].wt;
        jobQueue[i]->rt=list[i].rt;
        jobQueue[i]->tt=list[i].tt;
        jobQueue[i]->tq=list[i].tq;
        jobQueue[i]->next=NULL;

    }
}


//도착순으로 job queue에 있는 프로세스들 정렬
void sortByat(int n){
    PCBpointer t;
    //정렬
    for(int i=0; i<n-1; i++){
        int minIdx=i;
        for(int j=i+1; j<n; j++){
            if(jobQueue[j]->arrival_time<jobQueue[minIdx]->arrival_time){
                minIdx=j;
            }
        }
        t=jobQueue[i];
        jobQueue[i]=jobQueue[minIdx];
        jobQueue[minIdx]=t;
    }
}

//각 알고리즘마다 ready queue에서의 활동이 달라지므로 이것을 jobqueue에서 부터 보장해주기 위해 clone을 이용
PCBpointer cloneJobQueue[PROCESS_NUM];
int clone_job_queue_num=0;
void insert_clone_job_queue(){
    for(int i=0; i<PROCESS_NUM; i++){
        cloneJobQueue[i]=(PCBpointer)malloc(sizeof(struct PCB));
        cloneJobQueue[i]->pid=jobQueue[i]->pid;
        cloneJobQueue[i]->priority=jobQueue[i]->priority;
        cloneJobQueue[i]->arrival_time=jobQueue[i]->arrival_time;
        cloneJobQueue[i]->burst_time=jobQueue[i]->burst_time;
        cloneJobQueue[i]->wt=jobQueue[i]->wt;
        cloneJobQueue[i]->rt=jobQueue[i]->rt;
        cloneJobQueue[i]->tt=jobQueue[i]->tt;
        cloneJobQueue[i]->tq=jobQueue[i]->tq;
        cloneJobQueue[i]->next=NULL;
        clone_job_queue_num++;
    }
}


void FCFS(){
    float avgwt=0.0;
    float avgrt=0.0;
    float avgtt=0.0;

    int sumwt=0,sumrt=0,sumtt=0;
    int idle=0;

    int time=0;
    int i=0; //index

    readyQueue rq;
    rq.head=rq.tail=NULL;
    PCBpointer t=NULL;
    PCBpointer ptr=NULL;

    printf("Scheduling : FCFS\n");
    printf("=================================================\n");

    while(!(clone_job_queue_num==0 && ready_queue_num==0 && runningProcess==NULL)){
        //job queue에서 arriveTime이 time과 일치하면 readyQueue로 넣어주기
            //ready queue num이 0이면 PCB를 readyQueue의 head,tail로 연결
            // ready queue num이 0이 아니면 새로운 process를 PCB의 next로 추가, tail에도 추가 
            //job queue num --, ready queue num ++

        //Defense infinite loop 
        if(time>=100)   break;
        //도착시간이 같은 프로세스가 있을 수 있으므로 while문
        while(cloneJobQueue[i]->arrival_time==time){
            printf("<time %d> [new arrival] process %d\n",time,cloneJobQueue[i]->pid);
            
            if(ready_queue_num==0){
                rq.head=cloneJobQueue[i];
                rq.tail=cloneJobQueue[i];
            }
            else{
                
                rq.tail->next=cloneJobQueue[i];
                rq.tail=cloneJobQueue[i];
            }
            i++;
            clone_job_queue_num--;
            ready_queue_num++;
        }

        //runningProcess없다면 그리고 ready queue에 프로세스 있다면
            //->ready queue에 있는거 running으로 보내주기
            //running으로 가는 프로세스의 next를 ready queue의 head로 보내주고 next=null
            //ready queue num -- , new arrival 출력, process is running 출력
            //burstTime --, turnaroundtime++
            //넣어줄게 없으면 system is idle 출력, idle++
        
        
        if(runningProcess==NULL){
            if(rq.head!=NULL){
                runningProcess=rq.head;
                if(runningProcess->rt==-1)   runningProcess->rt=runningProcess->wt;
                if(prevRunningProcess==NULL)    prevRunningProcess=runningProcess;
            
                t=rq.head;
                rq.head=t->next;
                t->next=NULL;

                //rq.head=rq.head->next;

                if((prevRunningProcess!=NULL && runningProcess!=NULL)){
                    if(prevRunningProcess!=runningProcess){
                        printf("------------------------------------  (Context-Switch)\n");
                    }
                }

                printf("<time %d> process %d is running\n",time,runningProcess->pid);
                
                ready_queue_num--;
                runningProcess->burst_time--;
                runningProcess->tt++;   //여기에서 tt는 running state일 때의 시간

                //burst time이 1인 process가 있을 수 있으므로
                if(runningProcess->burst_time==0){
                    runningProcess=NULL;
                }

                
            }
            else{
                printf("<time %d> ---- system is idle ----\n",time);
                idle++;

                prevRunningProcess=NULL;
            }

            
        }

        //runningProcess 기존에 있다면
            // prorocess is running 출력
            // runningProcess의 burstTime --, turnaroundtime ++
            // 만약 runningProcess의 burstTime이 0이 되면 runningProcess=NULL, 

        else if(runningProcess!=NULL){
            printf("<time %d> process %d is running\n",time,runningProcess->pid);
            runningProcess->burst_time--;
            runningProcess->tt++;   //여기에서 tt는 running state일 때의 시간
            
            if(runningProcess->burst_time==0){
                runningProcess=NULL;
            }
        }

        //readyQueue에(프로세스가 있다면) 있는 프로세스들 watingTime++
        if(rq.head!=NULL){
            ptr=rq.head;
            while(ptr!=NULL){
                ptr->wt++;
                ptr=ptr->next;
            }
        }

        //time++
        time++;
    }
    printf("<time %d> all processes finish\n",time);
    printf("=================================================\n");

    for(int j=0; j<PROCESS_NUM; j++){
        sumwt+=cloneJobQueue[j]->wt;
        sumrt+=cloneJobQueue[j]->rt;
        sumtt+=cloneJobQueue[j]->tt + cloneJobQueue[j]->wt; //tt는 running state일 때만 더해줬으므로 ready queue에 있을 때도 지금 더해줌
    }


    avgwt=(float)sumwt/PROCESS_NUM;
    avgrt=(float)sumrt/PROCESS_NUM;
    avgtt=(float)sumtt/PROCESS_NUM;


    printf("Average cpu usage : %.2lf%%\n",((time-idle)/(float)time)*100);
    printf("Average waiting time : %.1lf\n",avgwt);
    printf("Average response time : %.1lf\n",avgrt);
    printf("Average turnaround time : %.1lf\n",avgtt);



}
//Round Robin
//가정: 프로세스가 도착하자마자 running 할 수 없다
void RR(int timequantum){
    float avgwt=0.0;
    float avgrt=0.0;
    float avgtt=0.0;

    int sumwt=0,sumrt=0,sumtt=0;
    int idle=0;

    int time=0;
    int i=0; //index

    ready_queue_num=0;

    readyQueue rq;
    rq.head=rq.tail=NULL;
    PCBpointer t=NULL;  //PCBpointer를 받기위한 임시 저장소
    PCBpointer t2=NULL; //PCBpointer를 받기위한 임시 저장소2
    PCBpointer ptr=NULL; //readyQueue의 wating time을 증가시키는데 사용되는 포인터
    PCBpointer runningProcess=NULL;
    PCBpointer prevRunningProcess=NULL;

    printf("\nScheduling : RR\n");
    printf("=================================================\n");

    while(!(clone_job_queue_num==0 && ready_queue_num==0 && runningProcess==NULL)){
        //job queue에서 arriveTime이 time과 일치하면 readyQueue로 넣어주기
            //ready queue num이 0이면 PCB를 readyQueue의 head,tail로 연결
            // ready queue num이 0이 아니면 새로운 process를 PCB의 next로 추가, tail에도 추가 
            //job queue num --, ready queue num ++
        
        //Defense infinite loop
        if(time>=100)    break;

        // 출력해서 보기위한
        // if(time>=20){
        //     printf("rq.tail->pid%d\n",rq.tail->pid);
        //     printf("ready_queue num%d\n",ready_queue_num);
        // }

        //도착시간이 같은 프로세스가 있을 수 있으므로 while문
        while(cloneJobQueue[i]->arrival_time==time){
            printf("<time %d> [new arrival] process %d\n",time,cloneJobQueue[i]->pid);
            
            if(ready_queue_num==0){
                rq.head=cloneJobQueue[i];
                rq.tail=cloneJobQueue[i];
            }
            else{
                //cloneJobQueue[i-1]->next=cloneJobQueue[i];
                rq.tail->next=cloneJobQueue[i];
                rq.tail=cloneJobQueue[i];
            }
            i++;
            clone_job_queue_num--;
            ready_queue_num++;
        }

        

        //runningProcess없다면 그리고 ready queue에 프로세스 있다면
            //->ready queue에 있는거 running으로 보내주기
            //running으로 가는 프로세스의 next를 ready queue의 head로 보내주고 next=null
            //ready queue num -- , new arrival 출력, process is running 출력
            //burstTime --, turnaroundtime++
            //넣어줄게 없으면 system is idle 출력, idle++
        
        
        if(runningProcess==NULL){
            if(rq.head!=NULL){
                runningProcess=rq.head;
                if(runningProcess->rt==-1)   runningProcess->rt=runningProcess->wt;
                if(prevRunningProcess==NULL)    prevRunningProcess=runningProcess;
            
                t=rq.head;
                rq.head=t->next;
                t->next=NULL;


                if((prevRunningProcess!=NULL && runningProcess!=NULL)){
                    if(prevRunningProcess!=runningProcess){
                        printf("------------------------------------ 1(Context-Switch)\n");
                    }
                }

                printf("<time %d> process %d is running\n",time,runningProcess->pid);
                
                ready_queue_num--;
                runningProcess->burst_time--;
                runningProcess->tt++;   //여기에서 tt는 running state일 때의 시간
                
                if(runningProcess->burst_time==0){
                    runningProcess=NULL;
                }

                //running process의 burst time이 아직 남아있을 때
                else{
                    runningProcess->tq++;
                    //runningProcess에서 timeQuantum시간이 다 됐을경우
                    if(runningProcess->tq%timequantum==0){
                        if(rq.head!=NULL){
                    
                            runningProcess->tq=0;

                            rq.tail->next=runningProcess;
                            rq.tail=runningProcess;

                            prevRunningProcess=runningProcess;

                            runningProcess=rq.head;

                            //running process로 넘어간 process의 next를 초기화
                            t2=rq.head;
                            rq.head=t2->next;
                            t2->next=NULL;

                    
                        printf("------------------------------------  2(Context-Switch)\n");

                        }

                        //ready queue에 아무것도 없을경우 계속 running

                
                    }
                }

                
            }
            else{
                printf("<time %d> ---- system is idle ----\n",time);
                idle++;

                prevRunningProcess=NULL;
            }

            
        }

        //runningProcess 기존에 있다면
            // prorocess is running 출력
            // runningProcess의 burstTime --, turnaroundtime ++
            // 만약 runningProcess의 burstTime이 0이 되면 runningProcess=NULL, 

        else if(runningProcess!=NULL){
           
            runningProcess->tq++;
            //runningProcess에서 timeQuantum시간이 다 됐을경우

            if(runningProcess->tq%timequantum==0){
                if(rq.head!=NULL){
                    printf("<time %d> process %d is running\n",time,runningProcess->pid);    
                    time++;
                    runningProcess->burst_time--;
                    runningProcess->tt++;   //여기에서 tt는 running state일 때의 시간

            
                    if(runningProcess->burst_time==0){
                        runningProcess=NULL;
                        //readyQueue에(프로세스가 있다면) 있는 프로세스들 watingTime++
                        if(rq.head!=NULL){
                            ptr=rq.head;
                            while(ptr!=NULL){
                                ptr->wt++;
                                ptr=ptr->next;
                            }
                        }
                        //runningProcess의 burstTime이 끝나면 readyqueue조정을 안해도된다.                
                        continue;
                    }
                    printf("------------------------------------  tq(Context-Switch)\n");

                    //readyQueue에(프로세스가 있다면) 있는 프로세스들 watingTime++
                    if(rq.head!=NULL){
                        ptr=rq.head;
                        while(ptr!=NULL){
                            ptr->wt++;
                            ptr=ptr->next;
                        }
                    }                
                    runningProcess->tq=0;

                    rq.tail->next=runningProcess;
                    rq.tail=runningProcess;

                    prevRunningProcess=runningProcess;

                    runningProcess=rq.head;
                    
                    //running process로 넘어간 process의 next를 초기화
                    t2=rq.head;
                    rq.head=t2->next;
                    t2->next=NULL;
                   
                    continue;
                   
                }

                //ready queue에 아무것도 없을경우 계속 running

                
            }
            printf("<time %d> process %d is running\n",time,runningProcess->pid);
            runningProcess->burst_time--;
            runningProcess->tt++;   //여기에서 tt는 running state일 때의 시간

            
            if(runningProcess->burst_time==0){
                runningProcess=NULL;
            }

            
        }

        //readyQueue에(프로세스가 있다면) 있는 프로세스들 watingTime++
        if(rq.head!=NULL){
            ptr=rq.head;
            while(ptr!=NULL){
                ptr->wt++;
                ptr=ptr->next;
            }
        }

        //time++
        time++;
    }
    printf("<time %d> all processes finish\n",time);
    printf("=================================================\n");



    for(int j=0; j<PROCESS_NUM; j++){
        sumwt+=cloneJobQueue[j]->wt;
        sumrt+=cloneJobQueue[j]->rt;
        sumtt+=cloneJobQueue[j]->tt + cloneJobQueue[j]->wt; //tt는 running state일 때만 더해줬으므로 ready queue에 있을 때도 지금 더해줌
    }


    avgwt=(float)sumwt/PROCESS_NUM;
    avgrt=(float)sumrt/PROCESS_NUM;
    avgtt=(float)sumtt/PROCESS_NUM;


    printf("Average cpu usage : %.2lf%%\n",((time-idle)/(float)time)*100);
    printf("Average waiting time : %.1lf\n",avgwt);
    printf("Average response time : %.1lf\n",avgrt);
    printf("Average turnaround time : %.1lf\n",avgtt);


}

void Priority(float alpha){
    //우선순위 같을경우 도착시간 먼저온애 부터
    //도착시간 같을경우 burst time 짧은애 먼저

    //프로세스 도착할 때마다 priority 비교, 다르면 재정렬

    //도착한 프로세스 없으면 aging에 의한 priority 비교

    //정렬할려 할 때 레디큐의 head가 null인지 체크
    
}

//알고리즘 수행 뒤 clone_job_queue는 초기화, ready queue는 비우기, runningProcess도 free 해주기

void clear_clone_job_queue(){
    for(int i=0; i<PROCESS_NUM; i++){
        free(cloneJobQueue[i]);
        cloneJobQueue[i]=NULL;
    }
}

void clear_job_queue(){
    for(int i=0; i<PROCESS_NUM; i++){
        free(jobQueue[i]);
        jobQueue[i]=NULL;
    }
}
int main(int argc, char *argv[]){

    read_process_list("input1.dat");
    insert_job_queue();
    sortByat(PROCESS_NUM);
    insert_clone_job_queue();
    FCFS();

    clear_clone_job_queue();
    insert_clone_job_queue();


    RR(3);
    
    






    //모든 알고리즘 끝낸 뒤
    //job_queue free
    //ready queue free
    //running process도 free


    return 0;
}




