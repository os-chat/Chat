#include <fcntl.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <chrono>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <inttypes.h>
#include <sys/time.h>

uint64_t timeSinceEpochMillisec()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}
void ti()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    unsigned long long millisecondsSinceEpoch =
        (unsigned long long)(tv.tv_sec) * 1000 +
        (unsigned long long)(tv.tv_usec) / 1000;

    printf("%llu\n", millisecondsSinceEpoch);
    //   printf("[%"PRIu64"]\n", timeSinceEpochMillisec());
    fflush(stdout);
}
void *myThreadFun(void *vargp)
{
    int i = *((int *)&vargp);
    printf("[%d] entro\n", i);
    ti();
    printf("\n");
    sleep(8.213918);
    ti();
    printf("\n");
}

int main()
{
    int i;
    pthread_t tid;

    // Let us create three threads
    for (i = 0; i < 3; i++)
        pthread_create(&tid, NULL, myThreadFun, (void *) i);

    pthread_exit(NULL);
    return 0;
}
