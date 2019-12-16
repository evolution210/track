ELEC462001_SYSTEM_PROGRAMMING(FALL_2019)
PROFESSOR Young-Kyoon Suh
Team Project : Implement Linux Built-in Command 'top'
Problem description
*****************************************************************
Authorized by
2013049101 황인승
2017118012 예진화
2013002085 최기락
*****************************************************************

 Htop 을 목적으로 top을 구현한 프로세스 관리 프로그램입니다.

1. 프로그램 실행 개념

1.1. 프로그램을 실행하면 시스템에 존재하는 프로세스들을 불러와 그들의 PID와 CPU, 메모리 사용량 계산등에 필요한
전체 시스템의 UPTIME등의 정보를 저장합니다. 

1.2. 받아온 PID를 기반으로 PROC filesystem중 프로세스들의 정보가 들어있는 /proc/PID/status를 사용하여 프로세스의 유저, Priority, Nice value, 
CPU 사용량등의 정보를 MakeStat함수를 사용해 저장합니다.

1.3 받아온 전체 시스템과 프로세스의 정보들을 1초 단위로 업데이트 해서 출력합니다.


2. 구현 기능

	HelpPage	: 기존 TOP/HTOP 커멘드의 help page처럼 프로그램의 기본적인 설명과 구현한 기능들을 설명하는 페이지 입니다.

	정렬	: 프로그램 최초 실행시에는 PID 순으로 프로세스들이 정렬되어 있는데 각각 PID, TIME, CPU사용량, 메모리사용량
		  순으로 프로세스들을 재정렬 할 수 있습니다.

	KILL	: 커서가 위치한 프로세스를 KILL 합니다.

	Renicing	: 커서가 위치한 프로세스의 Priority 와 Nice value의 값을 1 상승 및 감소할 수 있습니다.

	Quit	: 프로그램을 종료합니다.


3. 주요 함수 설명

	MakeStat	: Directory이름을 받아와서 /proc/PID/status를 사용해 프로세스의 정보들을 입력받아 저장하고 사용하지 않는/불필요한 정보들은
		  dummy 변수들에 저장합니다.
	
	setup	: Thread를 사용하여 정보를 정렬하고 출력합니다. 

	cpu_usage: 프로세스가 사용한 전체 시간을 구하고 그 값을 프로세스의 uptime으로 나누어 CPU 사용률을 계산합니다.

	compare	: 프로그램에서 출력하는 프로세스들을 정렬 기능의 원하는 정렬 기준을 입력받아 정렬합니다.

	update_totalInfo :  단순히 프로그램 실행 시점의 정보가 아니라 실시간으로 변하는 프로세스들의 정보를 업데이트 하기위해 필요한
			CUP, 메모리, uptime 값들을 지속적으로 업데이트 해줍니다. 

4. 실행 방법

	clean : Object file 제거

	make : track 실행 프로그램 생성
	
	./track : 프로그램 실행
