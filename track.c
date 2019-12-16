#include "track.h"
#include "header.h"


void falarm(int signum){//siganl handling
	signal(SIGALRM,falarm);
	clear();
	set_terminal();
	MakeArr();

	//sort and print information thread
	pthread_create(&header,NULL,header_print,NULL);
	pthread_create(&body,NULL,body_print,NULL);
	pthread_join(header,NULL);
	pthread_join(body,NULL);

	refresh();
}


int set_ticker(int n_msecs){//timer setting 
	struct itimerval new_timeset;
	long n_sec,n_usecs;
	
	//compute time
	n_sec = n_msecs / 1000;
	n_usecs = (n_msecs % 1000) * 1000L;
	
	//register timer
	new_timeset.it_interval.tv_sec = n_sec;
	new_timeset.it_interval.tv_usec = n_usecs;
	new_timeset.it_value.tv_sec = n_sec;
	new_timeset.it_value.tv_usec = n_usecs;

	return setitimer(ITIMER_REAL,&new_timeset,NULL);
}

void signal_setup(void){//initial signal setting
	struct sigaction timesig, scrollsig;
	timesig.sa_handler = falarm;
	sigemptyset(&timesig.sa_mask);
	timesig.sa_flags = 0;

	if(sigaction(SIGALRM,&timesig,NULL)==-1)
		perror("sigaction : SIGALRM");

	set_ticker(1000);//update cycle time
}

void setup(void){//initial screen setting
	set_terminal();//timer setting
	MakeArr(); //read informations
	keypad(stdscr,TRUE);//keypad on

	//sort and print information thread
	pthread_create(&header,NULL,header_print,NULL);
	pthread_create(&body,NULL,body_print,NULL);
	pthread_join(header,NULL);
	pthread_join(body,NULL);

	//initial sorting status 
	s_status.sort_switch = false;//sort off
	s_status.mode = 'x';//default sort mode
}

void* header_print(void* f){//thread function
	update_totalInfo();//update total information
	pthread_mutex_lock(&lock);//lock
	print_totalInfo();//print total information
	pthread_mutex_unlock(&lock);//unlock
}

void* body_print(void* f){//thread function
	sort_stat();//sort process information
	pthread_mutex_lock(&lock);//lock
	print_sorted_procInfo();//print process information
	pthread_mutex_unlock(&lock);//unlock
}

void print_F1(void){//HELP feature
        //description
        signal(SIGALRM,SIG_IGN);
        clear();
        standout();
        printw("Authorized by : Hwang, Inseung ; Ye, Jinhwa ; Choi, Girak\n");
        standend();
        printw("\n");
        printw("Help Page\n");
        printw("\n");
        standout();
        printw("This utility(TOP/HTOP) is to display information about CPU and memory utilization.\n");
        printw("Try this program to monitor CPU and Memory usage in run time.\n");
        standend();
        printw("\n");
        printw("CPU[ ] = CPU usage of the whole system\n");
        printw("MEM[ ] = Memory usage of the whole system\n");
        printw("SWP[ ] = Swap memory percentage of the whole system\n");
        printw("Tasks = Total tasks in action\n");
        printw("\n");
        printw("[Feature Description]\n");
        printw("F1 : Help Page\n");
        printw("F2 : Sorting\n");
        printw("\tP : Sort by PID\n");
        printw("\tM : Sort by Memory Percentage\n");
        printw("\tC : Sort by CPU Percentage\n");
        printw("\tT : Sort by Time\n");
        printw("\tQ : Cancel\n");
        printw("F3 : Kill\n");
        printw("\ty : kill\n");
        printw("\tn : cancel\n");
        printw("F4 : Change nice/priority\n");
        printw("\tF7 : minus nice/priority\n");
        printw("\tF8 : plus nice/priority\n");
        printw("\tQ : cancel\n");
        printw("F5 : Quit\n");
        standout();
        printw("\nPress any key to return\n");
        standend();

        if(getch()){
                clear();
                signal(SIGALRM,falarm);
                return;
        }
}



void print_F2(void){//sorting feature
        clear();
        s_status.sort_switch = true;//sorting on

        //sort and print information thread
        pthread_create(&header,NULL,header_print,NULL);
        pthread_create(&body,NULL,body_print,NULL);
        pthread_join(header,NULL);
        pthread_join(body,NULL);


        switch(getch()){
                case 'p'://PID sorting
                        s_status.mode = 'p';//sorting mode
                        s_status.sort_switch = false;//sorting off
                        break;

                case 'm'://MEM sorting
                        s_status.mode = 'm';
                        s_status.sort_switch = false;
                        break;

                case 'c'://CPU sorting
                        s_status.mode = 'c';
                        s_status.sort_switch = false;
                        break;

                case 't'://TIME sorting
                        s_status.mode = 't';
                        s_status.sort_switch = false;
                        break;

                case 'n'://NICE sorting
                        s_status.mode = 'n';
                        s_status.sort_switch = false;
                        break;

                case 'q'://ESC sorting
                        s_status.sort_switch = false;
                        return;

                default :
                        break;
        }
}

void kill_F3(int index){//kill feature function
        clear();//window clear
        s_status.kill_switch = true;//kill_F3 on
        menu_ind = prow; //save selected cusor's information

        //print inforamtion
        print_totalInfo();
        print_sorted_procInfo();

        switch(getch()){//input about kill
                case 'Y':
                case 'y':
                        kill(pstat_arr[index].stat->PID,SIGQUIT);//send signal  
                        s_status.kill_switch = false;//kill off
                        break;
                case 'N':
                case 'n':
                        s_status.kill_switch = false;//kill off
                        break;
                default:
                        kill_F3(index);
        }
}

void print_F4(int index){//renicing feature
        clear();
	menu_ind = prow; //save selected cusor's information

        s_status.renice_switch = true;//renicing on
	int rn;

	//print info
	print_totalInfo();
        print_sorted_procInfo();	
	while(s_status.renice_switch){	
        switch(getch()){
                case KEY_F(7)://renicing -
			rn = pstat_arr[index].stat->NICE; //read current cusor process's priority
			if(rn > -19){
				 rn --;
				setpriority(PRIO_PROCESS,pstat_arr[index].stat->PID,rn); //decrease priority
			}
                        continue;
                case KEY_F(8)://renicing +
			rn = pstat_arr[index].stat->NICE;
			if(rn < 20){
				 rn ++;
                                 setpriority(PRIO_PROCESS,pstat_arr[index].stat->PID,rn); //increase priority
			}
			continue;
		case 'q'://ESC 
			s_status.renice_switch = false;
			return;
		default :
			break;
	}
	}
}

void main(int argc, char* argv[]){
	int menu=0;
	signal_setup();//timer, signal setup

	initscr();//init window
	clear();

	setup();//total setup
	refresh();

	while((menu=getch())!= KEY_F(5)){//KEY_F5 is QUIT
		switch(menu){
			case KEY_UP://key up
				if(prow > nrow){ //to hightrite make a pointer index (prow+nrow)
					prow--;//at last page
				}
				else if(prow > 0){//before toucing last page
					nrow--;
					prow--;
				}
				break;
			case KEY_DOWN://key down
				if(nrow < t.task-frow+8){//before touching last page
					nrow++;
					prow++;
				}
				else if(prow < arrindex-1){//at last page
					prow++;
				}
				break;
			case KEY_F(1)://Help F1
				print_F1();
				break;
			case KEY_F(2)://Sorting F2
				print_F2();
				break;
			case KEY_F(3)://Kill F3
				kill_F3(prow);
				break;
			case KEY_F(4)://Renicing F4
                                print_F4(prow);
                                break;
		}

		//sort and print information thread
		pthread_create(&header,NULL,header_print,NULL);
		pthread_create(&body,NULL,body_print,NULL);
		pthread_join(header,NULL);
		pthread_join(body,NULL);

	
	}
	freedom();//total free
	endwin();//close window
}


void MakeArr(void){
	arrindex=0; //set array
	DIR * dir;
	struct dirent *entry;

	if((dir = opendir(PROC_PATH))==NULL)
		return;
	else{
		while((entry = readdir(dir)) != NULL){
			if(atoi(entry->d_name) != 0){
				In_Dir(entry->d_name); //read stat&cmdline in each process file
			}
		}
		closedir(dir);
	}
}

DIR* In_Dir(char * dirname){
	int dn = atoi(dirname);
	MakeUname(dirname); //read process owner name
	MakeStat(dirname); //read most informations
	MakeCmd(dirname); //read commnad line
	stat_arr[arrindex].MEM_PER=process_usage(t.mem,dn);
	stat_arr[arrindex].CPU_PER=cpu_usage(t.upt, stat_arr[arrindex].UTIME,stat_arr[arrindex].STIME,stat_arr[arrindex].CUTIME,stat_arr[arrindex].CSTIME,stat_arr[arrindex].TIME);
	stat_arr[arrindex].PROC_T =proc_t(t.upt,stat_arr[arrindex].TIME);
	arrindex++;
}

void MakeUname(char * dirname){
	FILE *statf;
	struct stat info;
	int dirnum = atoi(dirname);
	char dirbuf[1024];
	char buffer[1024];
	int buflen=0;
	sprintf(dirbuf,"/proc/%d",dirnum);

	if(stat(dirbuf,&info)==-1)
		return;
	else{
		strcpy(buffer, uid_to_name(info.st_uid));//save chaged uname from uid in buffer
		if((buflen = strlen(buffer))<=0);
		else{ //add to array only if has cmd line
			Eliminate(buffer,'(',')');
			if(stat_arr[arrindex].USR==NULL)
				stat_arr[arrindex].USR = (char*)malloc(sizeof(char)*(buflen+1));
			if(strlen(stat_arr[arrindex].USR) <= buflen)
				stat_arr[arrindex].USR = (char*)realloc(stat_arr[arrindex].USR,
						sizeof(char)*(buflen+1));//size up realloc
			strcpy(stat_arr[arrindex].USR,buffer);//save in stat_arr structure
		}
	}
}

char* uid_to_name(uid_t uid){//uid number to process name
        struct passwd *pw_ptr;
        static char numstr[256];

        if((pw_ptr = getpwuid(uid)) == NULL){
                sprintf(numstr,"%d", uid);
                return numstr;
        }
        else{
                return pw_ptr->pw_name;
        }

}



void MakeStat(char* dirname){
	FILE *statf;
	int dirnum = atoi(dirname);
	char statbuf[1024];
	int buflen=0;
	char buffer[1024];
	unsigned long long st;
	char dummy1[128],dummy2[128],dummy3[128],dummy4[128],dummy5[128],dummy6[128],dummy7[128],dummy8[128],dummy9[128],dummy10[128],dummy11[128],dummy12[128],dummy13[128],dummy14[128],dummy15[128],dummy16[128],dummy17[128];

	sprintf(statbuf,"/proc/%d/stat",dirnum);
	statf = fopen(statbuf,"r"); //oepn "/proc/<PID>/stat"
	if(statf == NULL)
		return;
	else{
		fscanf(statf,"%d %s %c %d %s %s %s %s %s %s %s %s %s %lu %lu %ld %ld %ld %ld %s %s %lu %s %lu "
				,&stat_arr[arrindex].PID //PID
				,buffer //filename
				,&stat_arr[arrindex].STAT //stat character
				,&stat_arr[arrindex].PPID //PPID
				,dummy1 //pgrp
				,dummy2 //session
				,dummy3 //tty_nr
				,dummy4 //tpgid
				,dummy5 //flags
				,dummy6 //minflt
				,dummy7 //cminflt
				,dummy8 //majflt
				,dummy9 //cmajflt
				,&stat_arr[arrindex].UTIME //utime
				,&stat_arr[arrindex].STIME //stime
				,&stat_arr[arrindex].CUTIME //cutime
				,&stat_arr[arrindex].CSTIME //cstime add four of this = (time_now - starting_time)
				,&stat_arr[arrindex].PRI //priority
				,&stat_arr[arrindex].NICE //nice
				,dummy14 //num threads
				,dummy15 //itrealvalue
				,&stat_arr[arrindex].TIME //starttime
				,dummy16
				,&stat_arr[arrindex].MEM // vsize
				);
		Eliminate(buffer,'(',')');
		if((buflen = strlen(buffer))<=0);
		else{ //add to array only if has cmd line
			if(stat_arr[arrindex].NAME==NULL)
				stat_arr[arrindex].NAME = (char*)malloc(sizeof(char)*(buflen+1));
			if(strlen(stat_arr[arrindex].NAME) <= buflen)
				stat_arr[arrindex].NAME = (char*)realloc(stat_arr[arrindex].NAME,
						sizeof(char)*(buflen+1));//size up realloc
			strcpy(stat_arr[arrindex].NAME,buffer); //save informations in stat_arr structure
		}
		if(statf)
			fclose(statf);
		statf=NULL;
	}

}

void MakeCmd(char* dirname){//save command line
	FILE* cmdlf;
	int dirnum = atoi(dirname);
	char cmdlbuf[1024];
	char buffer[1024];
	int buflen=0;
	sprintf(cmdlbuf,"/proc/%d/cmdline",dirnum);
	if(cmdlf = fopen(cmdlbuf,"r")){ //open "/proc/<PID>/cmdline"
		fscanf(cmdlf,"%s",buffer);
		if((buflen = strlen(buffer))<=0);//if cmdline is null stop
		else{
			Eliminate(buffer,'(',')');
			if(stat_arr[arrindex].CMD==NULL)
				stat_arr[arrindex].CMD = (char*)malloc(sizeof(char)*(buflen+1)); //make char array to save cmdline
			if(strlen(stat_arr[arrindex].CMD) <= buflen)
				stat_arr[arrindex].CMD = (char*)realloc(stat_arr[arrindex].CMD,
						sizeof(char)*(buflen+1));//size up realloc
			strcpy(stat_arr[arrindex].CMD,buffer);//savei command line in stat_arr structure
		}
		fclose(cmdlf);
		cmdlf=NULL;
	}
}

double process_usage(double total_mem,int i){//calculate process memory
	char line[1024];
	double percentage=0;
	unsigned long nVmRSS = 0;

	sprintf(line, "/proc/%d/status", i);
	FILE* fp=fopen(line, "r");
	if(fp == NULL)
		return 0;

	while(fgets(line, 1024, fp) != NULL){
		if(strstr(line, "VmRSS")){
			char tmp[32];
			char size[32];
			sscanf(line, "%s%s", tmp,size);
			nVmRSS = atoi(size);
			break;
		}
	}
	fclose(fp);
	percentage=100*(nVmRSS/total_mem);

	return percentage;
}

double cpu_usage(time_t upt, unsigned long UTIME, unsigned long STIME, long CUTIME, long CSTIME, unsigned long STARTTIME){
	unsigned long seconds;
	double cpu_usage, total_time;
	total_time = UTIME + STIME + CUTIME + CSTIME;
	//the total time spent in this process = CPU time spent in user code + CPU time 
	//spent in kernel code + waited for children's cpu time in user code + waited for children's cpu time in kernel code.
	seconds = upt - (STARTTIME/100);
	//transform the total elapsed time in clock ticks into seconds.
	cpu_usage = 100 * ((total_time/100)/seconds);
	//percentage of cpu usage.
	return cpu_usage;
}

long proc_t(time_t upt, unsigned long STARTTIME){
	long seconds;
	if(STARTTIME != 0)
		seconds = upt - (STARTTIME/100);
	//cpu uptime - the starttime of each processes/100; /100 <- is to transform clock ticks to seconds.
	else
		seconds = 0;
	return seconds;
}

void print_time_beautiful(long time){
	int min,hour,sec;
	sec = time;
	min=sec/60;
	hour=min/60;
	sec=sec%60;
	min=min%60;
	printw("%.2d:%.2d:%.2d", hour,min,sec);
	//transform the time caculated in seconds to hour:min:sec.
}
void Eliminate(char* str, char ch1, char ch2){//increase readability by removing some char
	for(;*str!='\0';str++){
		if(*str == ch1 | *str == ch2){
			strcpy(str,str+1);
			str--;
		}
	}
}

void set_terminal(void){//read terminal size
	struct winsize w;
	ioctl(STDOUT_FILENO,TIOCGWINSZ, &w);
	frow = w.ws_row;
	fcol = w.ws_col;
}

void print_sorted_procInfo(){//print sorted proc infomation
	int row = 7;
	for(int it=nrow;it<frow+nrow-8;it++){ 
		//print form 8th to last line
		//about sorted pstat_arr from how many moved down cusor to cusor + size of terminal 
		move(row++,1);
		if(it == prow)//highlite current selecting row in table
			standout();
		printw("%-8d",pstat_arr[it].stat->PID);//else normal print 
		printw("%-20s",pstat_arr[it].stat->USR);
		printw("%-8d",pstat_arr[it].stat->PPID);
		printw("%-8c",pstat_arr[it].stat->STAT);
		printw("%-8ld",pstat_arr[it].stat->PRI);
		printw("%-10ld",pstat_arr[it].stat->NICE);
		print_time_beautiful(pstat_arr[it].stat->PROC_T);	
		printw("  %.2lf%%  ", pstat_arr[it].stat->CPU_PER);
		printw("%-8lu",pstat_arr[it].stat->MEM);
		printw("  %.2lf%%  ",pstat_arr[it].stat->MEM_PER);
		printw("%-36s",pstat_arr[it].stat->NAME);
		printw("%-20s",pstat_arr[it].stat->CMD);
		if(it == prow)
			standend();
	}
	if(s_status.sort_switch){//sort menu bar
		move(frow-1,1);
		standout();
		printw("P : PID   M : MEM   T : TIME   N : NICE   C : CPU   Q : canel");
		standend();
	}
	else if(s_status.kill_switch){//kill menu bar 
		move(frow-1,1);
        standout();
        printw("Do you really want to kill %s? [y/n]",pstat_arr[menu_ind].stat->NAME);
        standend();
	}
	else if(s_status.renice_switch){//renice menu bar
		move(frow-1,1);
	standout();
        printw("Up nice[F8] Down nice[F7] this %s? Quit[q]",pstat_arr[menu_ind].stat->NAME);
        standend();
	}
	else{//normal menu bar
		move(frow-1,1);
		standout();
		printw("[F1]Help  [F2]Sort  [F3]Kill  [F4]Priority  [F5]Quit                                 ");
		standend();
	}
}



int compare(const void *a, const void *b){//increasing quick sort
	struct pstatus p1 = *(struct pstatus *)a;
	struct pstatus p2 = *(struct pstatus *)b;

	switch(s_status.mode){//sorting mode
		case 'p'://PID sorting
			if(p1.stat->PID > p2.stat->PID)
				return 1;
			else if(p1.stat->PID < p2.stat->PID)
				return -1;
			else return 0;
			break;

		case 'm'://Mem sorting
			if(p1.stat->MEM_PER > p2.stat->MEM_PER)
				return -1;
			else if(p1.stat->MEM_PER < p2.stat->MEM_PER)
				return 1;
			else return 0;
			break;

		case 't'://TIME sorting
			if(p1.stat->TIME > p2.stat->TIME)
				return -1;
			else if(p1.stat->TIME < p2.stat->TIME)
				return 1;
			else return 0;
			break;

		case 'n'://NICE sorting
			if(p1.stat->NICE > p2.stat->NICE)
				return -1;
			else if(p1.stat->NICE < p2.stat->NICE)
				return 1;
			else return 0;
			break;

		case 'c'://CPU sorting
                        if(p1.stat->CPU_PER > p2.stat->CPU_PER)
                                return -1;
                        else if(p1.stat->CPU_PER < p2.stat->CPU_PER)
                                return 1;
                        else return 0;
                        break;

		default:
			return 0;
			break;

	}
}

void sort_stat(void){//print part sorting
	for(int i=0;i<arrindex; i++){
		pstat_arr[i].stat = &stat_arr[arrindex-i-1];
	}

	qsort(pstat_arr,arrindex,sizeof(struct pstatus),compare);
}


void freedom(void){//total free function
	for(int it=0;it<arrindex;it++){
		if(stat_arr[it].USR)
			free(stat_arr[it].USR);
		stat_arr[it].USR=NULL;
		if(stat_arr[it].CMD)
			free(stat_arr[it].CMD);
		stat_arr[it].CMD=NULL;
		if(stat_arr[it].NAME)
			free(stat_arr[it].NAME);
		stat_arr[it].NAME=NULL;
	}
}
