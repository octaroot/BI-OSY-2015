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

#define BUFFER_SIZE 70

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

    //cache table
    bool **cache = new bool *[size];

    //building the cache O(n^2)
    for (int i = 0; i < size; ++i) {
        cache[i] = new bool[size];
        for (int j = 0; j < size; ++j) {
            cache[i][j] = maxCrime >= values[i][j];
        }
    }

    int maxSuitableArea = 0;

    //O(n^3) search

    /*
     * DISCLAIMER
     *
     * Maximum-area continuous rectangle in histogram algorithm used
     *
     * http://www.seas.gwu.edu/~simhaweb/cs151/lectures/module6/module6.html
     */

    int * rowCache = new int[size];

    for (int row = 0; row < size; ++row) {

        //rebuild the row cache
        for (int col = 0; col < size; ++col) {
            rowCache[col] = 0;
            for (int rowInsideCol = row; rowInsideCol < size && cache[rowInsideCol][col]; ++rowInsideCol)
                rowCache[col]++;

        }

        for (int col = 0; col < size; ++col) {

            if (!cache[row][col]) continue;

            int currentRow, currentCol = col, rowMax;

            rowMax = size - 1;

            while (currentCol < size && cache[row][currentCol])
            {
                currentRow = row + rowCache[currentCol] - 1;
                if (currentRow < rowMax)
                    rowMax = currentRow;
                else
                    currentRow = rowMax;

                int tempArea;
                if ((tempArea = (currentCol - col + 1)*(currentRow - row + 1)) > maxSuitableArea)
                {
                    maxSuitableArea = tempArea;

                    res->m_Y = row;
                    res->m_X = col;
                    res->m_W = currentCol - col + 1;
                    res->m_H = currentRow - row + 1;

                }

                ++currentCol;
            }

        }

    }

    //freeing of cache allocated resources
    for (int i = 0; i < size; ++i) {
        delete[] cache[i];
    }

    delete[] rowCache;
    delete[] cache;

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
        sem_post(&g_Free);

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