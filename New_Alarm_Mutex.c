/*
 * New_Alarm_Mutex.c
 *Group Member
Yiming Shao	215159932
Yixi Zhao	214936298
Youngin Ko	214876924
Weinan Wu	212662011
Guohao Ouyang	214447817
 */
#include <pthread.h>
#include <time.h>
#include "errors.h"
#include <string.h>
#include <stdio.h>

typedef struct alarm_tag {
        struct alarm_tag    *link;
        int seconds;
        time_t time;            /* seconds from EPOCH */
        char message[128];
        int alarm_id;
        int display_id;
} alarm_t;

typedef struct display_info {
        int d_1;
        int d_2;
        int d_3;
        int count_1;
        int count_2;
        int count_3;
        int cap1;
        int cap2;
        int cap3;
        int wake_up_total;
        int id1;
        int id2;
        int id3;
}display_i;



pthread_mutex_t alarm_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;

display_i *display_info = NULL;
alarm_t *alarm_list = NULL;
alarm_t *current_alarm = NULL;
alarm_t *change;


void alarm_insert (alarm_t *alarm)
{
        int status;
        alarm_t **last, *next;

        last = &alarm_list;
        next = *last;
        while (next != NULL) {
                if (next->time >= alarm->time) {
                        alarm->link = next;
                        *last = alarm;
                        break;
                }
                last = &next->link;
                next = next->link;
        }

        if (next == NULL) {
                *last = alarm;
                alarm->link = NULL;
        }
}

int find_best_fitting(display_i *display_info)
{
        int c1 = display_info->cap1;
        int c2 = display_info->cap2;
        int c3 = display_info->cap3;
        if(c1<=c2 && c1<=c3) {
                return 1;
        } else if(c2 <= c1 && c2 <= c3) {
                return 2;
        } else {
                return 3;
        }
}

/*
 * The alarm thread's start routine.
 */
void *alarm_thread (void *arg)
{
        alarm_t *alarm;
        int sleep_time;
        time_t now;
        int status;
        int expired;
        int best_fitting;

        struct timespec cond_time;

        /*
         * Loop forever, processing commands. The alarm thread will
         * be disintegrated when the process exits.
         */
        while (1) {
                status = pthread_mutex_lock (&alarm_mutex);
                if (status != 0) {
                        err_abort (status, "Lock mutex");
                }
                alarm = alarm_list;

                if (alarm == NULL) {
                        sleep_time = 1;
                }
                else {
                        alarm_list = alarm->link;
                        current_alarm = alarm;
                        now = time (NULL);
                        if (alarm->time <= now) {
                                sleep_time = 0;
                        }
                        else{
                                sleep_time = alarm->time - now;
                        }

                        if (display_info->wake_up_total <3) {
                                if(display_info->wake_up_total==2) {
                                        display_info->d_3=1;
                                        display_info->cap3=1;
                                        display_info->wake_up_total++;
                                        status = pthread_cond_signal(&cond3);
                                        printf("Alarm Thread Created New Display Display Alarm Thread <%d> For Alarm(<%d> at <%d>: <%s>)\n",
                                               display_info->id3,alarm->alarm_id, time(NULL),alarm->message);
                                        display_info->count_3 = display_info->count_3+1;
                                }else if(display_info->wake_up_total==1) {
                                        display_info->d_2=1;
                                        display_info->cap2=1;
                                        display_info->wake_up_total++;
                                        status = pthread_cond_signal(&cond2);
                                        printf("Alarm Thread Created New Display Display Alarm Thread <%d> For Alarm(<%d> at <%d>: <%s>)\n",
                                               display_info->id2,alarm->alarm_id, time(NULL),alarm->message);
                                        display_info->count_2 = display_info->count_2+1;
                                }else{
                                        display_info->d_1=1;
                                        display_info->cap1=1;
                                        display_info->wake_up_total++;
                                        status = pthread_cond_signal(&cond1);
                                        printf("Alarm Thread Created New Display Display Alarm Thread <%d> For Alarm(<%d> at <%d>: <%s>)\n",
                                               display_info->id1,alarm->alarm_id, time(NULL),alarm->message);
                                        display_info->count_1 = display_info->count_1+1;
                                }

                        }else{
                                best_fitting = find_best_fitting(display_info);
                                if(best_fitting == 1)
                                {
                                        printf("Alarm Thread Display Alarm Thread <%d> Assigned to Display Alarm (<%d> at <%d>: <%s>)\n",
                                               display_info->id1, alarm->alarm_id, time(NULL), alarm->message);
                                        display_info->cap1 = display_info->cap1+1;
                                        display_info->count_1 = display_info->count_1+1;
                                        status = pthread_cond_signal(&cond2);

                                }else if(best_fitting == 2) {
                                        printf("Alarm Thread Display Alarm Thread <%d> Assigned to Display Alarm (<%d> at <%d>: <%s>)\n",
                                               display_info->id2, alarm->alarm_id, time(NULL), alarm->message);
                                        display_info->cap2 = display_info->cap2+1;
                                        display_info->count_2 = display_info->count_2+1;
                                        status = pthread_cond_signal(&cond2);

                                }else{
                                        printf("Alarm Thread Display Alarm Thread <%d> Assigned to Display Alarm (<%d> at <%d>: <%s>)\n",
                                               display_info->id3, alarm->alarm_id, time(NULL), alarm->message);
                                        display_info->cap3= display_info->cap3+1;
                                        display_info->count_3 = display_info->count_3+1;
                                        status = pthread_cond_signal(&cond1);
                                }
                        }
                        if (status != 0) {
                                err_abort (status, "Signal cond");
                        }

                }

                // status = pthread_mutex_unlock (&alarm_mutex);

                status = pthread_mutex_unlock(&alarm_mutex);
                if(status != 0) {
                        err_abort(status, "Unlock mutex");
                }

                sleep (sleep_time);
        }
}


void *display_alarm_thread1(void *args){

        int status;
        alarm_t *alarm;
        int d_id;

        d_id = pthread_self();
        display_info->id1 = d_id;
        while(1)
        {

                status = pthread_mutex_lock(&alarm_mutex);
                if (status != 0)
                {
                        err_abort(status, "Lock mutex");
                }

                status = pthread_cond_wait(&cond1, &alarm_mutex);
                if (status != 0)
                {
                        err_abort(status, "Wait on cond");
                }
                alarm = current_alarm;

                while(alarm->time > time (NULL))
                {
                        printf("Alarm(<%d> Printed by Alarm Display Thread <%d> at <%d>: <%s>)\n",
                               alarm->alarm_id, pthread_self(), alarm->time - time (NULL), alarm->message);
                        sleep(5);
                }

                printf("Alarm Thread Removed Alarm(<%d> at <%d>: <%s>)\n",
                       alarm->alarm_id, time(NULL), alarm->message);
                status = pthread_mutex_unlock(&alarm_mutex);
                if (status != 0)
                {
                        err_abort(status, "unlock mutex");
                }
                free(alarm);
                display_info->count_1=display_info->count_1-1;
                if(display_info->count_1==0) {
                        printf("Alarm Thread Terminated Display Thread <%d> at <%d>\n", pthread_self(), time(NULL));
                        display_info->wake_up_total=display_info->wake_up_total-1;
                }
        }

}

void *display_alarm_thread2(void *args){

        int status;
        alarm_t *alarm;
        int d_id;
        d_id = pthread_self();
        display_info->id1 = d_id;

        while(1)
        {
                status = pthread_mutex_lock(&alarm_mutex);
                if (status != 0)
                {
                        err_abort(status, "Lock mutex");
                }

                status = pthread_cond_wait(&cond2, &alarm_mutex);
                if (status != 0)
                {
                        err_abort(status, "Wait on cond");
                }
                alarm = current_alarm;

                while(alarm->time > time (NULL))
                {
                        printf("Alarm(<%d> Printed by Alarm Display Thread <%d> at <%d>: <%s>)\n",
                               alarm->alarm_id, pthread_self(), alarm->time - time (NULL), alarm->message);
                        sleep(5);
                }

                printf("Alarm Thread Removed Alarm(<%d> at <%d>: <%s>)\n",
                       alarm->alarm_id, time(NULL), alarm->message);
                status = pthread_mutex_unlock(&alarm_mutex);
                if (status != 0)
                {
                        err_abort(status, "unlock mutex");
                }
                free(alarm);

                display_info->count_2=display_info->count_2-1;
                if(display_info->count_2==0) {
                        printf("Alarm Thread Terminated Display Thread <%d> at <%d>\n", pthread_self(), time(NULL));
                        display_info->wake_up_total=display_info->wake_up_total-1;
                }
        }

}

void *display_alarm_thread3(void *args){

        int status;
        alarm_t *alarm;
        int d_id;
        d_id = pthread_self();
        display_info->id1 = d_id;

        while(1)
        {
                status = pthread_mutex_lock(&alarm_mutex);
                if (status != 0)
                {
                        err_abort(status, "Lock mutex");
                }

                status = pthread_cond_wait(&cond3, &alarm_mutex);
                if (status != 0)
                {
                        err_abort(status, "Wait on cond");
                }
                alarm = current_alarm;

                while(alarm->time > time (NULL))
                {
                        printf("Alarm(<%d> Printed by Alarm Display Thread <%d> at <%d>: <%s>)\n",
                               alarm->alarm_id, pthread_self(), alarm->time - time (NULL), alarm->message);
                        sleep(5);
                }

                printf("Alarm Thread Removed Alarm(<%d> at <%d>: <%s>)\n",
                       alarm->alarm_id, time(NULL), alarm->message);
                status = pthread_mutex_unlock(&alarm_mutex);
                if (status != 0)
                {
                        err_abort(status, "unlock mutex");
                }
                free(alarm);

                display_info->count_3=display_info->count_3-1;
                if(display_info->count_3==0) {
                        printf("Alarm Thread Terminated Display Thread <%d> at <%d>\n", pthread_self(), time(NULL));
                        display_info->wake_up_total=display_info->wake_up_total-1;
                }
        }

}


void *display_change_thread(void *args)
{
    int status;
    int sleep_time;
    time_t now;
    alarm_t *alarm = change;
    while (1) {
        status = pthread_mutex_lock(&alarm_mutex);
        if (status != 0)
            err_abort (status, "Lock mutex");
        if(alarm!=NULL)
        {
            printf("Display Thread<%d> Start to Print Changed Message at <%d>: <%s>\n", pthread_self(), time(NULL), alarm->message);
            free(change);
        }
        status = pthread_mutex_unlock (&alarm_mutex);
        if (status != 0)
            err_abort (status, "Unlock mutex");
        pthread_exit(NULL);
    }
}

int main (int argc, char *argv[])
{
        int status;
        char line[128];
        char Req[20];
        alarm_t *alarm, **last, *next;
        time_t now;

        pthread_t thread;
        pthread_t d1_thread;
        pthread_t d2_thread;
        pthread_t d3_thread;
        pthread_t dc_thread;  // display thread to print change

        display_info = (display_i*)malloc(sizeof(display_i));
        if (display_info == NULL)
                errno_abort ("Allocate Display_info");
        status = pthread_create (
                &thread, NULL, alarm_thread, NULL);
        if (status != 0)
        {
                err_abort (status, "Create alarm thread");
        }
        status = pthread_create (
                &d1_thread, NULL, display_alarm_thread1, NULL);
        if (status != 0)
        {
                err_abort (status, "Create display1 thread");
        }
        status = pthread_create (
                &d2_thread, NULL, display_alarm_thread2, NULL);
        if (status != 0)
        {
                err_abort (status, "Create display2 thread");
        }
        status = pthread_create (
                &d3_thread, NULL, display_alarm_thread3, NULL);
        if (status != 0)
        {
                err_abort (status, "Create display3 thread");
        }


        display_info->wake_up_total = 0;
        display_info->d_1= 0;
        display_info->d_2 = 0;
        display_info->d_3 = 0;
        display_info->cap1 = 0;
        display_info->cap2 = 0;
        display_info->cap3 = 0;
        display_info->count_1=0;
        display_info->count_2=0;
        display_info->count_3=0;
        while(1) {
                printf("<Alarm> " );

                if (fgets (line, sizeof (line), stdin) == NULL) exit (0);

                if (strlen (line) <= 1) continue;
                alarm = (alarm_t*)malloc (sizeof (alarm_t));
                if (alarm == NULL)
                        errno_abort ("Allocate alarm");
                if(sscanf(line, "%20[^(] (%d) %d %128[^\n]", Req, &alarm->alarm_id,  &alarm->seconds, alarm->message)<4)
                {
                        fprintf(stderr, "Bad command\n");
                        free(alarm);
                }else {
                        if(strcmp(Req, "Start_Alarm")==0) {
                                status = pthread_mutex_lock(&alarm_mutex);
                                if(status!=0) {
                                        err_abort(status,"Lock mutex");
                                }
                                alarm->time = time(NULL) + alarm->seconds;
                                last = &alarm_list;
                                next = *last;

                                alarm_insert(alarm);
                                printf("Alarm(<%d>) inserted by Main Thread<%d> Into Alarm List at <%d>: <%s>\n",
                                       alarm->alarm_id, pthread_self(), time(NULL), alarm->message);
                                status = pthread_mutex_unlock (&alarm_mutex);
                                if (status != 0) {
                                        err_abort (status, "Unlock mutex");
                                }

                        }else if(strcmp(Req, "Change_Alarm")==0) {
                                change = (alarm_t*)malloc (sizeof (alarm_t));
                                status = pthread_mutex_lock(&alarm_mutex);
                                if(status!=0) {
                                        err_abort(status,"Lock mutex");
                                }
                                alarm->time = time(NULL) + alarm->seconds;
                                last = &alarm_list;
                                next = *last;
                                int sig_change =0;
                                while (next != NULL) {
                                        if (next->alarm_id == alarm->alarm_id) {
                                                printf("Make changing\n");
                                            status = pthread_create (
                                                    &dc_thread, NULL, display_change_thread, change);
                                            if (status != 0)
                                            {
                                                err_abort (status, "Create display_change_info thread");
                                            }
                                                next->time = alarm->time;
                                                strcpy(next->message,alarm->message);
                                                next->seconds = alarm->seconds;
                                                change = alarm;
                                                sig_change =1;
                                                break;
                                        }
                                        last = &next->link;
                                        next = next->link;
                                }

                                if (sig_change==0) {
                                        printf("%s\n", "No such alarm");
                                }

                                status = pthread_mutex_unlock (&alarm_mutex);
                                if (status != 0)
                                        err_abort (status, "Unlock mutex");

                        }else{
                                printf("%s\n","Bad command\n");
                                free(alarm);

                        }
                }

        }

}
