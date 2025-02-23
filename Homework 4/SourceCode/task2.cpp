#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

class RepairStation
{
private:
    condition_variable cv;
    mutex mtx;
    int repair_type[3];
    int all;

    void enter_station(char type)
    {
        type -= 'a';
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this, type]
                { return (repair_type[type] > 0 && all > 0); });
        repair_type[type]--;
        all--;
        cout << "vechicle " << char('a' + type) << " " << " entered the station. Thread: " << omp_get_thread_num() << " free space overall after entering: " << all << " vechicle specific free space: " << repair_type[type] << "\n";
    }
    void exit_station(char type)
    {
        type -= 'a';
        unique_lock<mutex> lock(mtx);
        repair_type[type]++;
        all++;
        cout << "vechicle " << char('a' + type) << " " << " exited the station. Thread: " << omp_get_thread_num() << " free space overall: " << all << " vechicle specific free space: " << repair_type[type] << "\n";
        cv.notify_all();
        // Fairness is not guaranteed since cv.notify_all() wakes all threads, 
        // but the OS scheduler decides which thread proceeds first, possibly causing starvation.

    }

public:
    RepairStation(int _a, int _b, int _c, int _all)
    {
        repair_type[0] = _a;
        repair_type[1] = _b;
        repair_type[2] = _c;
        all = _all;
    }
    ~RepairStation() {}

    void use_station(char type)
    {
        enter_station(type);
        cout << "Bzzt... Bzzzzt Thread: " << omp_get_thread_num() << "\n";
        usleep(rand() % 3000 + 1);
        cout << "Thread: " << omp_get_thread_num() << " Repaired\n";
        exit_station(type);
    }
};

RepairStation station(10, 3, 5, 9);
void thrd_fun(char type)
{
    int x = 15;
    while (x--)
    {
        usleep(rand() % 3000 + 1);
        station.use_station(type);
    }
}
int main()
{
    omp_set_num_threads(20);
#pragma omp parallel
    {
        thrd_fun('a' + rand() % 3);
    }
}
