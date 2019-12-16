#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <curses.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

//proc file system
#define PROC_PATH "/proc"
#define STAT_FILE "/proc/stat"
#define MEM_FILE "/proc/meminfo"
#define UPT_FILE "/proc/uptime"

typedef struct mem_form{// proc/meminfo_memory info struct
        unsigned int mtotal;//memory total size
        unsigned int mavail;//available memory
        unsigned int stotal;//swap memory total size
        unsigned int sfree;//free swap memory
        double musage;//memory usage
        double susage;//swap memory usage
};

typedef struct cpu_form{// proc/stat_cpu format
        unsigned int usr;
        unsigned int nice;
        unsigned int sys;
        unsigned int idle;
        double usage;
};


typedef struct totalInfo{//header info struct 
        double cpu;//CPU 
        double mem;//Memory
        double swp;//Swap memory
        int task;//tasks
        time_t upt;//uptime
};

//global variable
struct mem_form m;//mem_structure
struct totalInfo t;//total_headerInfo

//update function
void update_totalInfo();//total information
time_t update_upt();
int update_task();
void update_mem();
double update_cpu();

//print total information
void print_totalInfo();

