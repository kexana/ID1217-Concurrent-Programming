#include<bits/stdc++.h>
#include<semaphore.h>
#include<omp.h>
using namespace std;
#define MAX_CONSECUTIVE_VISITS 50   

sem_t state_mutex, g_queue[2], fifo_queue, cout_sem;
int people_inside = 0;
int waiting[2] = {0,0};
bool bathroom_closed = 0;
bool wc_break = 0;
int consecutive_visits = 0;
bool gender_inside = 1;

// 0-woman  1-man
void bathoroomer(bool gender)
{
    sem_wait(&fifo_queue);
    sem_wait(&state_mutex);
    if(people_inside == 0)  gender_inside = gender;
    
    if(gender_inside == gender && !bathroom_closed)
    { 
        //entering wc

        people_inside++;
        consecutive_visits++;
        sem_wait(&cout_sem);
        cout<<"Thread: "<< omp_get_thread_num()<<" gender: "<<(gender? ("man"):("woman")) <<" entering the bathroom; "<<(gender_inside?("man wc"):("woman wc"))<<"; people inside: "<<people_inside<<"\n";
        sem_post(&cout_sem);    
        if(consecutive_visits > MAX_CONSECUTIVE_VISITS && waiting[!gender] > 0){
            
            sem_wait(&cout_sem); 
            cout<<"bathroom closed Thread: "<<omp_get_thread_num()<<"\n";
            sem_post(&cout_sem);    
            bathroom_closed = 1;
        }
        sem_post(&state_mutex);
        sem_post(&fifo_queue);
    }
    else    
    {
        waiting[gender]++;
        sem_post(&state_mutex);
        sem_post(&fifo_queue);

        sem_wait(&g_queue[gender]);
        sem_wait(&state_mutex);
        waiting[gender]--;
        people_inside++;
        sem_wait(&cout_sem);
        cout<<"Thread: "<< omp_get_thread_num()<<" gender: "<<(gender? ("man"):("woman")) <<" entering the bathroom after waiting in the queue; "<<(gender_inside?("man wc"):("woman wc"))<<"; people inside: "<<people_inside<<"\n";
        sem_post(&cout_sem);  
        consecutive_visits++;
        if(consecutive_visits > MAX_CONSECUTIVE_VISITS && waiting[!gender] > 0){
            
            sem_wait(&cout_sem); 
            cout<<"bathroom closed Thread: "<<omp_get_thread_num()<<"\n";
            sem_post(&cout_sem);    
            bathroom_closed = 1;
        }

        sem_post(&state_mutex);
    }

    //doing the bussines
    sem_wait(&cout_sem);
    cout<<"Thread: "<< omp_get_thread_num()<<" takes a "<<((rand()%10 <2) ? ("poo"):("pee"))<<" \n";
    sem_post(&cout_sem);
    //
    usleep(rand()%1500);
    //
    //exiting the wc
    sem_wait(&state_mutex);
    people_inside--;
    sem_wait(&cout_sem);
    cout<<"Thread: "<< omp_get_thread_num()<<" gender: "<<(gender? ("man"):("woman")) <<" exiting the bathroom; "<<"people inside: "<<people_inside<<"\n";
    sem_post(&cout_sem);    
    if(people_inside == 0)
    {
        if(waiting[!gender] > 0)
        {
            gender_inside = !gender;
            sem_wait(&cout_sem);  
            cout<<"Thread: "<< omp_get_thread_num()<<" changing wc gender to "<<((!gender)? "man":"woman")<<"\n";
            sem_post(&cout_sem);  
            consecutive_visits = 0;
            bathroom_closed = 0;
            for (int i = 0; i < min(MAX_CONSECUTIVE_VISITS,waiting[!gender]); i++)
            {
                sem_post(&g_queue[!gender]);
            }
        }
        else if(waiting[gender] > 0)
        {
            consecutive_visits = 0;
            bathroom_closed = 0;
            for (int i = 0; i < min(MAX_CONSECUTIVE_VISITS,waiting[gender]); i++)
            {
                sem_post(&g_queue[gender]);
            }
        }
        else
        {
            consecutive_visits = 0;
            bathroom_closed = 0;
        }
    }
    sem_post(&state_mutex);


}
void thread_fun(bool gender)
{
    while (1)
    {
        usleep(rand()%10000);
        bathoroomer(gender);
    }
    
}


int main()
{
    
    sem_init(&state_mutex,0,1);
    sem_init(&fifo_queue,0,1);
    sem_init(&g_queue[0],0,0);
    sem_init(&g_queue[1],0,0);
    sem_init(&cout_sem, 0, 1);
    omp_set_num_threads(1000);
    #pragma omp parallel
    {
        thread_fun(rand()%2);   
    }

    sem_destroy(&state_mutex);
    sem_destroy(&fifo_queue);
    sem_destroy(&g_queue[0]);
    sem_destroy(&g_queue[1]);
    sem_destroy(&cout_sem);

}