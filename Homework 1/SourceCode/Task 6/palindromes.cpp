#include <bits/stdc++.h>
#include <time.h>
#include <sys/time.h>
using namespace std;

#define pb push_back

struct thread_data{
    int start, end, tid, palindromes = 0, semordnilaps = 0;  
    vector<string> palindromes_vector, semordnilaps_vector;
};

/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized ){
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}
double start_time, end_time; 


int workers = 0;
vector<string>words;
//if string a < b => return -1 , if a==b return 0, if a > b return 1; 
int compare_strings(const string& a, const string& b)
{
    int len_min = min(a.length(), b.length());

    for (int i = 0; i < len_min; i++){
        if (a[i] < b[i])  return -1;
        if (a[i] > b[i])  return 1;
    }
    if(a.length() == b.length())    return 0;
    if(a.length() < b.length())     return -1;
    return 1;
}


//returns index of the string s in words vector or -1 if s is not found in words
int binsearch(int start, int end, string& s){

    while(start + 1 != end){
        int mid = (start  + end) / 2;
        if(compare_strings(words[mid], s) == 0) return mid;
        (compare_strings(words[mid], s) < 0 )? start = mid : end = mid; 
    }

    if(compare_strings(words[end], s) == 0 )    return end;
    if(compare_strings(words[start], s) == 0 )    return start;
    return -1;
}

void* thread_fun (void* arg){
    thread_data* data = (thread_data*) arg;
    data->palindromes = 0;
    data->semordnilaps = 0;
    for (int i = data->start; i < data->end; i++){
        //calculate the reverse
        string reverse = words[i];
        for (int j = 0; j < reverse.size() / 2; j++) 
        swap(reverse[j], reverse[reverse.size() - 1 - j]); 

        //check if palindrome
        if(compare_strings(reverse, words[i]) == 0){
            data->palindromes_vector.pb(reverse);
            data->palindromes++;
            continue;
        }

        //look for semordnilaps - fast
        if(binsearch(0,words.size()-1, reverse) != -1)  
        {
            data->semordnilaps_vector.pb(words[i]);
            data->semordnilaps++;
        } 

        //slow version
        // for (string s : words){
        //     if(compare_strings(s, reverse) == 0){
        //         data->semordnilaps++;
        //         break;
        //     }
        // }
    }
    return NULL;
}


int main(int argc, char *argv[]){
    // workers = 5;
    if (argc < 1)   workers = 1;
    else    workers = atoi(argv[1]);
    pthread_t tid[workers];
    thread_data thrd_data[workers];
    ifstream file("words");
    if (!file) {
        cerr << "Error opening file\n";
        return 1;
    }
    string line;
    while (getline(file, line)) {
        for (char c:line)
        {
            if(c >= 'A' && c <= 'Z') c-='A';
        } 
        words.pb(line);
    }

    sort(words.begin(), words.end());
    start_time = read_timer();
    for (int i = 0; i < workers; i++){
        thrd_data[i].start = (words.size()/workers) * i;
        thrd_data[i].end = (i == workers - 1)? words.size()-1 : (words.size()/workers) * (i+1);
        thrd_data[i].tid = i;
        if (pthread_create(&tid[i], NULL, thread_fun, &thrd_data[i]) != 0) 
            cerr << "error creating thread " << i << "\n";
    }

    int total_palindromes = 0, total_semordnilaps = 0;
    for (int i = 0; i < workers; i++){
        pthread_join(tid[i], NULL);

        total_palindromes+= thrd_data[i].palindromes;
        total_semordnilaps+= thrd_data[i].semordnilaps;
    }

    end_time = read_timer();
    /* print results */
    ofstream out_file("output.txt");
    out_file<<"The execution time is "<< end_time - start_time <<"sec\n\n";
    out_file<<"total_palindromes: "<<total_palindromes<<"\ntotal_semordnilaps: "<< total_semordnilaps<< "\n\n";
    
    for (int i = 0; i < workers; i++){
        out_file<<"worker "<< i<<":\n";
        out_file<<"number of palindromes: "<<thrd_data[i].palindromes<<"\nnumber of semordnilaps: "<<thrd_data[i].semordnilaps<<"\n\n";
    }
    out_file<<"\nPalindrome list:\n";
    int counter = 0;
    for (int i = 0; i < workers; i++){
        for (string s:thrd_data[i].palindromes_vector){
            out_file<<s<<", ";
            counter++;
            if(counter >= 10 ){
                counter = 0;
                out_file<<"\n";
            }
        }
        
    }

    out_file<<"\n\nSemordnilaps list:\n";
    counter = 0;
    for (int i = 0; i < workers; i++){
        for (string s:thrd_data[i].semordnilaps_vector){
            out_file<<s<<", ";
            counter++;
            if(counter >= 10 ){
                counter = 0;
                out_file<<"\n";
            }
        }
        
    }
    
    out_file.close();    
    

}