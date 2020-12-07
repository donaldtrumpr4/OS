#include "ctime"
#include "iostream"

#define THREAD_NUMBER 2
#define COUNT_SIZE 8308330
using namespace std;



int main ()
{
    long double time_start1,time1;
    time_start1 = clock();




    long double tmp = 0;
//Параллельный участок с параметрами: tmp будет просуммировано по завершении, а количество потоков равно THREAD_NUMBER
#pragma omp parallel reduction (+: tmp) num_threads(THREAD_NUMBER)
    {
        //Директива for с параметром динамического планирования блоками размера COUNT_SIZE
#pragma omp for schedule(dynamic, COUNT_SIZE)
        for (int j = 0;j < 100000000;j++)
        {
            tmp += 4 / (100000000 + (j + 0.5) * (j + 0.5) / 100000000);
        }
    };


    time1 = (clock() - time_start1);
    cout<<"Time taken("<<THREAD_NUMBER<<" threads): "<<time1<<" ms"<<endl;
    cout.precision(100000000);

    cout<<"PI = "<<tmp<<endl;













    return 0;
}