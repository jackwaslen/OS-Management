
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

#define NULL_FLAG 255
typedef struct Memory {
    int freeMemoryAddress;
    unsigned char *memoryRegion;
    int memorySize;
} Memory;

typedef struct taskCounter{
    int process;
    int time;
} taskCounter;

typedef struct taskInfo{
    int process;
    int start;
    int end;
} taskInfo;


int firstFit(char *process, int allocation, Memory memoryArray, int allocationSize);
int compaction(Memory memoryArray);
int release(Memory memoryArray, char* process);
int status(Memory memoryArray);

void setUpMemory(Memory *memory, int allocationSize){
    memory->freeMemoryAddress = 0;
    memory->memoryRegion = malloc(allocationSize);
    memory->memorySize = allocationSize;
}

int main(int argc, char *argv[]){
    // initial size amount via command line, ./allocation 1000000
    // program enters loop, exit to stop
    // user enters commands
    // commands - RQ, RL, C, Status, Exit
    char userInput[128];
    char command[64] = "n/a";
    char process[64] = "n/a";
    int allocation = -1;
    char algorithim[64] = "n/a";
    int allocationSize = atoi(argv[1]);
    

    // Global Variables
    Memory  memoryArray;
    setUpMemory(&memoryArray, allocationSize);



    // set up memory, 255 is null flag
    for (int j = 0; j < allocationSize; j++){
        memoryArray.memoryRegion[j] = 255;
    }
    printf("Here, the FIRST FIT approach has been implemented and the allocated %d bytes of memory \n", allocationSize);
    while (strcmp(command, "Exit") != 0){
        printf("allocator>");
        //sscanf(" %[^\n]", userInput);
        fgets(userInput, 4*1000, stdin);

        int parse = sscanf(userInput, "%s %s %d %s", command, process, &allocation, algorithim);
      
        if (strcmp(command, "RQ") == 0){
    
            firstFit(process, allocation, memoryArray, allocationSize);
        }
        if (strcmp(command, "C") == 0 ){
            compaction(memoryArray);
        }
        if (strcmp(command, "RL") == 0 ){
            release(memoryArray, process);
        }
         if (strcmp(command, "Status") == 0 ){
            //release(memoryArray, process);
            status(memoryArray);
        }
    }

    return 0;
}


int status(Memory memoryArray){
taskInfo tasks[64];
int count = 0;
int allocated = 0;
taskInfo currentTask;
taskInfo task;
task.process = memoryArray.memoryRegion[0];
task.start = 0;
currentTask = task;

if (task.process != 255){
    allocated += 1;
}


 for (int i= 1; i <=  memoryArray.memorySize; i++){
    int memId = memoryArray.memoryRegion[i];
    //printf("I: %d, rawValue:%d, currentTask: %d\n",i, memId, currentTask.process);
    if (memId != 255 && memoryArray.memorySize != i){
        // current task, save current task
        // current hole, save address of current hole
       // printf("I: %d, rawValue:%d, currentTask: %d\n",i, memId, currentTask.process);
        allocated += 1;
    }  
    if (memId != currentTask.process && memoryArray.memorySize != i){
        currentTask.end = i-1;
        tasks[count] = currentTask;
        count++;
        taskInfo task;
        task.process = memId;
        task.start = i;
        //printf("Saved Task: %d, start: %d, end: %d \n", task.process, task.start, task.end);
        currentTask = task;
   }
  
     
   // }  
 }
 currentTask.end = memoryArray.memorySize-1;
 tasks[count] = currentTask;
 //printf("Saved Task: %d, start: %d, end: %d \n", task.process, task.start, task.end);
count++;

 printf("Partitions [Allocated memory = %d]\n", allocated);
 for (int j = 0; j < count; j++){
    taskInfo task = tasks[j];
    if (task.process != 255){
        printf("Address [%d:%d] Process P%d\n", task.start, task.end, task.process);
    }

 }
 printf("\n");
 printf("Holes [Free memory = %d]:\n",  memoryArray.memorySize-allocated);
  for (int j = 0; j <= count; j++){
    taskInfo task = tasks[j];
    if (task.process == 255){
        printf("Address [%d:%d] len = %d\n", task.start, task.end, task.end - task.start +1);
    }

 }
}
int release(Memory memoryArray, char* process){
    printf("Releasing Memory for process %s\n", process);
     int processNumber = atoi(&process[1]);
     for (int i= 0; i <  memoryArray.memorySize; i++){
        if (memoryArray.memoryRegion[i] == processNumber){
            memoryArray.memoryRegion[i] = 0;
        }
     }
    printf("Successfully released memory for process %s\n", process);
}

int compaction(Memory memoryArray){
    // Loop over Memory Array, Count used sections.
    // Move first section to 0, second section directly after it.

    // if 0 
    // if filled,
    // currenTask same - > increment currentTasks time
    // currentTask diff -> save old task, set currentTask to new task time = 1
    taskCounter tasks[64];
    int count = 0;
    int currentTask;
    if (memoryArray.memoryRegion[0] == 255){
        currentTask = 255;
    } else {
        currentTask = memoryArray.memoryRegion[0];
    }
    
    int time = 0;
    for (int i= 0; i <=  memoryArray.memorySize; i++){
       // printf("I: %d, currentTask: %d\n",i, currentTask);
        if (memoryArray.memoryRegion[i] != 0){
            if (memoryArray.memoryRegion[i] != currentTask){
                if (currentTask != 255){
                    taskCounter task;
                    task.process = currentTask;
                    task.time = time;
                    tasks[count] = task;
                   // printf("Saved Task: %d, start: %d\n", task.process, task.time);
                    count += 1;
                }
                currentTask = memoryArray.memoryRegion[i];
                time = 0;
            }
            
        }
        time++;
       
    }
    
    // for each task, set task to each time period, rest to 0.
    taskCounter task = tasks[0];
    int taskIndex = 1;
    int counter = 1;
    int taskTime = task.time;
    for (int j = 0; j < memoryArray.memorySize; j++){
       if (taskTime == 0 && task.process != 255){
        //switch tasks
            if (counter < count){
                task = tasks[counter];
                taskTime = task.time;
                counter++;
            } else {
                taskCounter defaultTask;
                defaultTask.process = 255;
                task = defaultTask;
            }
            
       }
       memoryArray.memoryRegion[j] = task.process;
       taskTime--;
       
    }
    
    printf("Compaction process is successful!\n");
    return 0;
}

int firstFit(char *process, int allocation, Memory memoryArray, int allocationSize){
    int freeMemory = 0;
    int processNumber = atoi(&process[1]);
    int success = -1;
    //printf("memoryRegion Size: %d",(unsigned int)memoryRegion[0]);
    for (int i = 0; i < allocationSize; i++){
     
        if (memoryArray.memoryRegion[i] == NULL_FLAG){
            // memory is free
            if (freeMemory == 0){
                memoryArray.freeMemoryAddress = i;
            }
            freeMemory += 1;
           
            if (freeMemory >= allocation){
                int bufferSize;
                if (memoryArray.freeMemoryAddress == 0){
                    bufferSize = memoryArray.freeMemoryAddress + allocation;

                } else{
                   bufferSize= memoryArray.freeMemoryAddress + allocation;
                }
                for (int k = memoryArray.freeMemoryAddress; k < bufferSize; k++){
                    // fill memory
                   
                    memoryArray.memoryRegion[k] = processNumber;
                }
                success = 1;
                break;
            }
        } else{
            // memory is not free

            freeMemory = 0;
      
        }
    }
    if (success == 1){
printf("Successfully allocated %d to process %s\n", allocation, process);
    } else {
        printf("No hole of sufficient size!\n");
    }
    
  
}
