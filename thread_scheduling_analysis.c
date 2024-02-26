 /*
 -------------------------------------
 Assignment 3
 File: fcfs.c
 -------------------------------------
 Author:  Connor Doidge and Jack Waslen
 ID:      210279850 and 210468930
 Email:   doid9850@mylaurier.ca and wasl8390@mylaurier.ca
 Version  2023-11-03
 -------------------------------------
 */

#include <sys/mman.h>
#include <sys/stat.h>       
#include <sys/types.h>   
#include <sys/wait.h>     
#include <fcntl.h>   
#include <unistd.h>
#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
struct threadInfo {
    int p_id;
    int arr_time;
    int burst_time;
    int completion;
    int waiting_time;
    int turn_around_time;
};
int turnAroundTime(int completion, int arrival){
    return completion - arrival;
}
int waitingTime(int turnAround, int burstTime){
    return turnAround - burstTime;
}
struct threadInfo getThread(int id, int arrival, int burst){
    struct threadInfo bufferThread;
    bufferThread.p_id = id;
    bufferThread.arr_time = arrival;
    bufferThread.burst_time = burst;
   
    return bufferThread;
}
int main(){
    FILE *fileObj = fopen("sample_in_schedule.txt", "r");
    if (fileObj == NULL){
        printf("Failed to Open File");
    }
    int completion = 0;
    int count = 0;
    struct threadInfo currentThread = {0, 0, 0, 0, 0, 0};
    struct threadInfo infoArray[128];
    int id, arrival, burst;
    while(fscanf(fileObj, "%d, %d, %d", &id, &arrival, &burst) == 3){
      
        completion += burst;
        currentThread = getThread(id, arrival, burst);
        currentThread.completion = completion;
        currentThread.turn_around_time = turnAroundTime(completion, arrival);
        currentThread.waiting_time = waitingTime(currentThread.turn_around_time, burst);
        infoArray[count] = currentThread;
        count += 1;
    }
    int index = 0;
    float waitingTime = 0;
    float turnAroundTime = 0;
    printf("Thread ID       Arrival Time       Burst Time       Completion Time  Turn-Around Time       Waiting Time\n");
    while(count > index){
        printf("%-2d                    ",infoArray[index].p_id);
        printf("%-2d                  ",infoArray[index].arr_time);
        printf("%-2d                    ",infoArray[index].burst_time);
        printf("%-2d               ",infoArray[index].completion);
        printf("%-2d                     ",infoArray[index].turn_around_time);
        printf("%-2d\n",infoArray[index].waiting_time);
        
        waitingTime += infoArray[index].waiting_time;
        turnAroundTime += infoArray[index].turn_around_time;
        index += 1;
    }
    float finalTurnAround = turnAroundTime/index;
    float finalWaitingTime = waitingTime /index;
    printf("The average waiting time: %.2f\n", finalWaitingTime);
    printf("The average turn-around time: %.2f", finalTurnAround);
}
