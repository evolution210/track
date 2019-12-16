#include "header.h"

void update_totalInfo(){//total informatioon update
	t.cpu = update_cpu();
	update_mem();
	t.mem = m.musage;
	t.swp = m.susage;
	t.task = update_task();
	t.upt = update_upt();
}

double update_cpu(){//return cpu_usage
	FILE* fp;//proc/stat file pointer
	struct cpu_form c;//cpu_format struct

	fp = fopen(STAT_FILE,"r");//open /proc/stat
	if(fp == NULL)
		return 0;

	fscanf(fp,"%*s %u %u %u %u",&(c.usr),&(c.nice),&(c.sys),&(c.idle));//read cpu info
	c.usage = (c.idle)*100/((c.usr)+(c.nice)+(c.sys)+(c.idle));//compute cpu usage
	fclose(fp);

	return 100 - c.usage;
}

void update_mem(){//memory info update
	FILE* fp;
	char line[1024];

	fp = fopen(MEM_FILE,"r");//open /proc/meminfo
	if(fp == NULL)
		return;

	while(fgets(line,1024,fp)!=NULL){//parsing /proc/meminfo file
		//printf("%s\n",line);
		if(strstr(line,"MemTotal"))
			sscanf(line,"MemTotal: %u",&(m.mtotal));
		if(strstr(line,"MemAvailable"))
			sscanf(line,"MemAvailable: %u",&(m.mavail));
		if(strstr(line,"SwapTotal"))
			sscanf(line,"SwapTotal: %u",&(m.stotal));
		if(strstr(line,"SwapFree"))
			sscanf(line,"SwapFree: %u",&(m.sfree));
	}
	m.musage = (double)(m.mtotal - m.mavail);//compute memory usage
	m.susage = (double)(m.stotal - m.sfree);//compute swap memory usage

	fclose(fp);
}

int update_task(){//task info
	DIR* dir_ptr;
	struct dirent* direntp;
	int count = 0;

	if( (dir_ptr = opendir(PROC_PATH)) == NULL)//open /proc directory 
		return 0;
	
	while( (direntp = readdir(dir_ptr)) != NULL){//count processes
		if(atoi(direntp->d_name) != 0)
			count++;
	}

	closedir(dir_ptr);
	return count;//the number of process
}

time_t update_upt(){//read uptime
	FILE *fp;
	time_t uptime;
	struct tm *utime;

	fp = fopen(UPT_FILE,"r");//open /proc/uptime 
	if(fp == NULL)
		return uptime;

	fscanf(fp,"%ld",&uptime);
	fclose(fp);
	return uptime;
}
void print_totalInfo(){//print header total infomation
	time_t c_time = time(NULL);//current time
	int min,hour,sec;//uptime variable
	sec = t.upt;
	min=sec/60;
	hour=min/60;
	sec=sec%60;
	min=min%60;

	move(1,1);
	printw("CPU[ ");//CPU info
	for(int i=0; i<(t.cpu/5);i++)
		printw("|");
	move(1,30);
	printw(" %.1f%% ]\n",t.cpu);
	move(2,1);
	printw("MEM[ ");//Memory info
	for(int i=0;i<(t.mem*100/m.mtotal)/5;i++)
		printw("|");
	move(2,30);
	printw(" %.2fG/%.2fG ]",t.mem/1000000.0,m.mtotal/1000000.0);

	move(3,1);
	printw("SWP[ ");//Swap memory info
	for(int i=0;i<(t.swp*100/m.stotal)/5;i++)
		printw("|");
	move(3,30);
	printw(" %.1fM/%.1fM ]",t.swp/1000.0,m.stotal/1000.0);

	move(1,60);
	printw("Tasks : %d",t.task);//Task info
	move(2,60);
	printw("Uptime : %d:%d:%d", hour,min,sec);//Uptime info
	move(3,60);
	printw("Current time : %s",ctime(&c_time));//Current time info

	move(6,1);
	printw("%-8s %-20s %-8s %-8s %-5s %-8s %-12s %-4s %-8s %-2s %-36s %-20s","PID","USR","PPID","STAT","PRI","NICE","TIME","CPU","MEM","MEM_PER","NAME","CMD");//header bar

	refresh();
}

