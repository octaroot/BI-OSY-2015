#ifndef __PROGTEST__

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <stack>
#include <deque>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

using namespace std;

struct TRect {
    int m_X;
    int m_Y;
    int m_W;
    int m_H;
};

struct TCostProblem {
    int **m_Values;
    int m_Size;
    int m_MaxCost;

    void         (*m_Done)(const TCostProblem *, const TRect *);
};

struct TCrimeProblem {
    double **m_Values;
    int m_Size;
    double m_MaxCrime;

    void         (*m_Done)(const TCrimeProblem *, const TRect *);
};

#endif /* __PROGTEST__ */

#define BUFFER_SIZE 100

struct TMyBuffer {
    const TCrimeProblem *crimeProblem;
    const TCostProblem *costProblem;

    TMyBuffer(TCrimeProblem const *crimeProblem, TCostProblem const *costProblem)
            : crimeProblem(crimeProblem), costProblem(costProblem) {
    }
};

struct TFunctionPointer
{
    const void *(*func)(void);
};

queue<TMyBuffer*> g_Buffer;

pthread_mutex_t g_MtxBuffer;
sem_t g_Full, g_Free;


bool FindByCost(int **values, int size, int maxCost, TRect *res) {

    const int actualSize = size + 1;

    //cache int table (SAT table)
    int **cache = new int *[actualSize];

    //building the cache
    //we fill the 0,y and x,0 with initial values to make the main O(n^2) do no conditionals

    //the [0,0]
    cache[0] = new int[actualSize];
    cache[0][0] = 0;

    //the [x,0] and [0,y]
    for (int i = 1; i < actualSize; ++i) {
        cache[i] = new int[actualSize];
        cache[i][0] = 0;
        cache[0][i] = 0;
    }

    //the rest. no conditionals, yay! O(n^2)
    for (int i = 1; i < actualSize; ++i) {
        for (int j = 1; j < actualSize; ++j) {
            cache[i][j] = values[i - 1][j - 1] + cache[i - 1][j] + cache[i][j - 1] - cache[i - 1][j - 1];
        }
    }

    //debug
/*
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            printf ("%5d ", values[i][j]);
        }
        puts("");
    }
*/

    int maxSuitableArea = 0, totalCost, area;

    //O(n^4) search
    for (int tl_row = 0; tl_row < actualSize; ++tl_row) {
        for (int tl_col = 0; tl_col < actualSize; ++tl_col) {
            for (int br_row = tl_row; br_row < actualSize; ++br_row) {
                for (int br_col = tl_col; br_col < actualSize; ++br_col) {

                    totalCost = cache[br_row][br_col] - cache[tl_row][br_col] - cache[br_row][tl_col] + cache[tl_row][tl_col];

                    if (totalCost <= maxCost && (area = (br_row - tl_row) * (br_col - tl_col)) > maxSuitableArea) {

                        res->m_H = br_row - tl_row;
                        res->m_W = br_col - tl_col;
                        res->m_X = tl_col;
                        res->m_Y = tl_row;

                        maxSuitableArea = area;
                    }
                }
            }
        }
    }

    //freeing of cache allocated resources
    for (int i = 0; i < actualSize; ++i) {
        delete[] cache[i];
    }

    delete[] cache;

    return maxSuitableArea != 0;
}

bool FindByCrime(double **values, int size, double maxCrime, TRect *res) {

    //cache histogram table
    int **cache = new int *[size + 1];

    //optimization trick - less conditionals :)
    cache[0] = new int[size];
    for (int i = 0; i < size; ++i) {
        cache[0][i] = 0;
    }

    //building the cache O(n^2)
    for (int i = 1; i <= size; ++i) {
        cache[i] = new int[size];
        for (int j = 0; j < size; ++j) {
            if (maxCrime >= values[i - 1][j]) {
                cache[i][j] = 1 + cache[i - 1][j];
            }
            else {
                cache[i][j] = 0;
            }
        }
    }

    int maxSuitableArea = 0;

    //debug
/*
    printf(" ");

    for (int i = 0; i < size; ++i) {
        printf("%4d", i);
    }

    puts("");

    for (int i = 1; i <= size; ++i) {
        printf("%d:", i - 1);
        for (int j = 0; j < size; ++j) {
            if (cache[i][j] == 0)
                printf("    ");
            else
                printf("%3d ", cache[i][j]);
        }
        puts("");
    }
*/

    //O(n^2) search

    int *maxAreas = new int[size];
    int tmpArea;
    int *left = new int[size];
    stack<int> stack1;

    /*
     * DISCLAIMER
     *
     * Maximum-area continuous rectangle in histogram algorithm used
     *
     * http://tech-queries.blogspot.cz/2011/03/maximum-area-rectangle-in-histogram.html
     */
    for (int i = 1; i <= size; ++i) {
        for (int j = 0; j < size; ++j) {
            while (!stack1.empty() && cache[i][j] <= cache[i][stack1.top()]) {
                stack1.pop();
            }
            left[j] = maxAreas[j] = j - 1 - (stack1.empty() ? -1 : stack1.top());
            stack1.push(j);
        }

        stack1 = stack<int>();

        for (int j = size - 1; j >= 0; --j) {
            while (!stack1.empty() && cache[i][j] <= cache[i][stack1.top()]) {
                stack1.pop();
            }
            maxAreas[j] += (stack1.empty() ? size : stack1.top()) - j - 1;
            stack1.push(j);
        }

        for (int j = 0; j < size; ++j) {
            if ((tmpArea = cache[i][j] * (maxAreas[j] + 1)) > maxSuitableArea) {
                maxSuitableArea = tmpArea;
                res->m_X = j - left[j];
                res->m_Y = i - cache[i][j];
                res->m_W = maxAreas[j] + 1;
                res->m_H = cache[i][j];
            }
        }
    }

    //freeing of cache allocated resources
    for (int i = 0; i <= size; ++i) {
        delete[] cache[i];
    }

    delete[] cache;
    delete[] maxAreas;
    delete[] left;

    return maxSuitableArea != 0;
}

void solveProblems() {
    while (1) {

        sem_wait(&g_Full);

        pthread_mutex_lock(&g_MtxBuffer);

        TMyBuffer * top = g_Buffer.front();

        const TCostProblem *costProblem = top->costProblem;
        const TCrimeProblem *crimeProblem = top->crimeProblem;

        if (costProblem == NULL && crimeProblem == NULL) {
            pthread_mutex_unlock(&g_MtxBuffer);
            sem_post(&g_Full);
            //puts ("zabijim vlakno po zarazce");
            return;
        }

        g_Buffer.pop();

        pthread_mutex_unlock(&g_MtxBuffer);

        delete top;

        TRect solution;

        if (costProblem) {

            if (FindByCost(costProblem->m_Values, costProblem->m_Size, costProblem->m_MaxCost, &solution)) {
                costProblem->m_Done(costProblem, &solution);
            }
            else {
                costProblem->m_Done(costProblem, NULL);
            }

        }
        else {
            if (FindByCrime(crimeProblem->m_Values, crimeProblem->m_Size, crimeProblem->m_MaxCrime, &solution)) {
                crimeProblem->m_Done(crimeProblem, &solution);
            }
            else {
                crimeProblem->m_Done(crimeProblem, NULL);
            }
        }

        sem_post(&g_Free);
    }
}

void generateCostProblems(const TFunctionPointer* f) {

    const TCostProblem * problem;

    while ((problem = (const TCostProblem *)f->func()) != NULL) {
        sem_wait(&g_Free);
        pthread_mutex_lock(&g_MtxBuffer);

        TMyBuffer * newProblem = new TMyBuffer(NULL, problem);

        g_Buffer.push(newProblem);

        pthread_mutex_unlock(&g_MtxBuffer);
        sem_post(&g_Full);
    }

    return;
}

void generateCrimeProblems(const TFunctionPointer* f) {
    const TCrimeProblem* problem;

    while ((problem = (const TCrimeProblem*)f->func()) != NULL) {
        sem_wait(&g_Free);
        pthread_mutex_lock(&g_MtxBuffer);

        TMyBuffer * newProblem = new TMyBuffer(problem, NULL);

        g_Buffer.push(newProblem);

        pthread_mutex_unlock(&g_MtxBuffer);
        sem_post(&g_Full);
    }

    return;
}

void MapAnalyzer(int threads, const TCostProblem *(*costFunc)(void), const TCrimeProblem *(*crimeFunc)(void)) {

    pthread_t *thrID = new pthread_t[2 + threads];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_mutex_init(&g_MtxBuffer, NULL);
    sem_init(&g_Free, 0, BUFFER_SIZE);
    sem_init(&g_Full, 0, 0);

    TFunctionPointer costFunctionPointer, crimeFunctionPointer;

    costFunctionPointer.func = (void const *(*)()) costFunc;
    crimeFunctionPointer.func = (void const *(*)()) crimeFunc;

    pthread_create(&thrID[threads], &attr, (void *(*)(void *)) generateCostProblems, &costFunctionPointer);
    pthread_create(&thrID[threads + 1], &attr, (void *(*)(void *)) generateCrimeProblems, &crimeFunctionPointer);

    for (int i = 0; i < threads; i++)
        pthread_create(&thrID[i], &attr, (void *(*)(void *)) solveProblems, NULL);

    pthread_attr_destroy(&attr);

    pthread_join(thrID[threads], NULL);
    pthread_join(thrID[threads + 1], NULL);

    sem_wait(&g_Free);
    pthread_mutex_lock(&g_MtxBuffer);

    TMyBuffer *zarazka = new TMyBuffer(NULL, NULL);

    g_Buffer.push(zarazka);

    pthread_mutex_unlock(&g_MtxBuffer);
    sem_post(&g_Full);

    for (int i = 0; i < threads; i++)
        pthread_join(thrID[i], NULL);

    g_Buffer.pop();
    delete zarazka;

    pthread_mutex_destroy(&g_MtxBuffer);
    sem_destroy(&g_Free);
    sem_destroy(&g_Full);

    delete[] thrID;

}