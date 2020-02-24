#Group Member
 #Yiming Shao	215159932
 #Yixi Zhao	214936298
 #Youngin Ko	214876924
 #Weinan Wu	212662011
 #Guohao Ouyang	214447817


all: new_alarm_mutex.c
	cc -o new_alarm_mutex new_alarm_mutex.c -D_POSIX_PTHREAD_SEMANTICS -lpthread