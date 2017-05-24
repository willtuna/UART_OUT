#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void get_current_time(void );
int main(int argc, char* argv[]){

    for(int i=0 ; i < 20 ; ++i){
        get_current_time();
        sleep(2);
    }

    return 0;   
}
void get_current_time(void){
    const int GMT8 = 8;
    // create a struct of time which contains the year date hour minute second format
    struct tm *tm_ptr;

    // time_t is for fetch the current time from GMT Jan 1 , 1970
    time_t current_time;
    // this function would return the time calculated from 1970
    time(&current_time); 

    // Convert the time_t to struct tm format
    tm_ptr = gmtime(&current_time);

    printf("Raw Time is %ld", current_time);
    printf("gmtime gives:\n");
    printf("date: %04d / %02d / %02d \n", tm_ptr->tm_year+1900, tm_ptr->tm_mon+1, tm_ptr->tm_mday);
    printf("time: %02d Hr %02d Min %02d Sec \n", tm_ptr->tm_hour + GMT8, tm_ptr->tm_min , tm_ptr -> tm_sec);
}
