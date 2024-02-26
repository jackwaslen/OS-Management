/*
 File: Process_Management.c
 -------------------------------------
 Author: Jack Waslen
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

int childProcess(int bufferSize, const char *memName){
    // 1. Establish the shared memory object
    // 2. Output contents of shared memory
    // 3, Remove shared memory
    // 4. Program exits when all sequences equal to start_numbers printed on console
    char *shared_mem_base;
    printf("Child Process: The generated collatz sequence is ");
    int shared_mem_value = shm_open(memName, O_CREAT | O_RDWR, 0666);
    if (shared_mem_value == -1){
        printf("Oh no! Failed to create shared Memory! %s\n", strerror(errno));
        exit(1);
    }
    //configure size
    ftruncate(shared_mem_value, bufferSize);
    // map shared memory segment to address space of process
    shared_mem_base = mmap(0, bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, shared_mem_value, 0);
    if (shared_mem_base == MAP_FAILED){
         printf("Oh no! Failed to map shared Memory! %s\n", strerror(errno));
   
       exit(1);
    }

    printf("%s ", shared_mem_base);
  

    // removing shared memory
     if (munmap(shared_mem_base, bufferSize) == -1){
        printf("Unmapped failed! %s\n", strerror(errno));
        exit(1);
    }
    if (close(shared_mem_value) == -1){
        printf("Failed to close %s\n", strerror(errno));
        exit(1);
    }

    if (shm_unlink(memName) == -1) {
        printf("Failed to Unlink %s: %s\n", memName, strerror(errno));
    exit(1);
  }
  printf("\n");
    return 0;
}

int parentProcess(int number){
    // read a list of starting numbers ["start_numbers.txt"], store in array.
    // For each number, parent process creates a collatz sequence with shared-memory object, print sequence, complete itself.
    // For each number, 1. Established shared memory object (shm_open(), ftruncate(), mmap())
    // 2. Create collatz sequence for number
    // 3. Save sequence to shared memory object
    // 4. create child process, wait for it to terminate
    // 5. Once operations are done, delete shared mem object with munmap, shm_unlink
   
    // Establish shared memory object
    const int bufferSize = 4096;
   
    const char *memName = "/SharedMemory";
    char *shared_mem_base;

    // create shared memory segment
    
    int shared_mem_value = shm_open(memName, O_CREAT | O_RDWR, 0666);
    if (shared_mem_value == -1){
        printf("Oh no! Failed to create shared Memory! %s\n", strerror(errno));
        exit(1);
    }
    //configure size
    ftruncate(shared_mem_value, bufferSize);
    // map shared memory segment to address space of process
    shared_mem_base = mmap(0, bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, shared_mem_value, 0);
    if (shared_mem_base == MAP_FAILED){
         printf("Oh no! Failed to map shared Memory! %s\n", strerror(errno));
       // close and shm_unlink? -> what example program says
       exit(1);
    }
    printf("Parent Process: The positive integer read from file is %d \n", number);
    // Collatz Sequence

    char buffer[64];
    char *ptr = shared_mem_base;
    ptr += sprintf(ptr, "%d ", number);
    while (number != 1){
       if (number % 2 == 0){
        number = number / 2;
    } else {
        number = number*3 + 1;
    }

    ptr += sprintf(ptr, "%d ", number);

    }


    return 0;

}

int main(){
    const char *fileName = "start_numbers.txt";
    int inputNumbers[100];
    int numbersRead = 0;
    const int bufferSize = 4096;
    const char *memName = "/SharedMemory";
    FILE *fileObject = fopen(fileName, "r");
    if (fileObject == NULL){
        printf("Failed to open file!");
    }
    while (fscanf(fileObject, "%d", &inputNumbers[numbersRead]) == 1 && numbersRead < 100){
        numbersRead += 1;
    }
    int finished = 0;
    for (int i = 0; i < numbersRead; i++){
        pid_t pid = fork();
        if (pid < 0){
            printf("Oh no! Failed to fork processes! %s\n", strerror(errno));
            exit(1);
        }
        else if (pid > 0){
       
            parentProcess(inputNumbers[i]);
            wait(&pid); 
            finished = 1;
           
        } else {
           
            childProcess(bufferSize, memName);
            exit(0);
            
        }
        wait(&pid);

           
    }
    fclose(fileObject);
    
    return 0;
}
