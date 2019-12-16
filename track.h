#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<pwd.h>
#include<signal.h>
#include<curses.h>
#include<time.h>
#include<sys/time.h>
#include<string.h>
#include<sys/ioctl.h>
#include<pthread.h>
#include<sys/resource.h>

struct status{//process status structure
        int PID;
        int PPID;
        char *USR;
        char STAT;
        long CPU;
        unsigned long MEM;
        double MEM_PER;
	unsigned long UTIME;
	unsigned long STIME;
	long CUTIME;
	long CSTIME;
	double CPU_PER;
        long PRI;
        long NICE;
        unsigned long TIME;
	long PROC_T;
        char *CMD;
        char *NAME;
}status;

struct pstatus{//sorted status
        struct status* stat;
}pstatus;

struct _status{//sorting,  kill, renicing mode record
	bool sort_switch;//sorting on/off
	char mode;//sorting mode
	bool kill_switch;//kill on/off
	bool renice_switch;//renicing on/off
};

//glocal variable 
int arrindex=0; //data array index
struct status stat_arr[4096]; //data table 
struct pstatus pstat_arr[4096]; //sorted data table pointer array
struct _status s_status;//feature mode record
int frow, fcol ; //terminal full row and column
int nrow = 0; //present row and column arrindex number for print
double CPP=0;
int prow=0; //cursor for printing
int menu_ind=0; //to save selected row index for menu
pthread_t header;//header control thread_total info
pthread_t body;//body control thread_process info

//setting function
void setup(void);//initial screen setting
void signal_setup(void);//initial signal setting
void set_terminal(void); //read size of terminal

//updating process info function
void MakeArr(void);
DIR* In_Dir(char * dirname);
void MakeUname(char *);
void MakeCmd(char* dirname);
char* uid_to_name(uid_t uid);
void freedom(void);
void MakeStat(char* dirname);
void Eliminate(char *str, char ch1, char ch2);
double process_usage(double total_mem,int i);
double cpu_usage(time_t upt, unsigned long UTIME, unsigned long STIME, long CUTIME, long CSTIME, unsigned long STARTTIME);
long proc_t(time_t upt, unsigned long STARTTIME);

//print function
void print_procInfo(void);
void print_sorted_procInfo(void);

//thread function
void* body_print(void *f);
void* header_print(void *f);

//feature function
void sort_stat(void);
int compare(const void *a, const void *b);//qsort compare function
void print_F1(void); //help
void print_F2(void); //sort
void print_F3(int); //kill
void print_F4(int index); //renice


pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;//thread lock

