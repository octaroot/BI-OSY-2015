#ifndef __PROGTEST__

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <stack>
#include <deque>

#ifdef linux
#include <pthread.h>
#include <semaphore.h>
#endif

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

void MapAnalyzer(int threads, const TCostProblem *(*costFunc)(void), const TCrimeProblem *(*crimeFunc)(void)) {
    // todo
}

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
    for (int i = 0; i < actualSize; ++i) {
        for (int j = 0; j < actualSize; ++j) {
            printf ("%5d ", cache[i][j]);
        }
        puts("");
    }
*/

    int maxSuitableArea = 0, totalCost, area;

    //O(n^4) search
    for (int row = 1; row < actualSize; ++row) {
        for (int col = 1; col < actualSize; ++col) {
            for (int h = 0; h < row; ++h) {
                for (int w = 0; w < col; ++w) {

                    totalCost = cache[row][col] - cache[row][w] - cache[h][col] + cache[h][w];

                    if (totalCost <= maxCost && (area = (row - h) * (col - w)) > maxSuitableArea) {

                        res->m_H = row - h;
                        res->m_W = col - w;
                        res->m_X = h;
                        res->m_Y = w;

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

    //cache bool table
    bool **cache = new bool *[size];

    //algo caches
    int ** dN = new int*[size], ** dS = new int*[size];

    //building the cache O(n^2)
    for (int i = 0; i < size; ++i) {
        cache[i] = new bool[size];
        dN[i] = new int[size];
        dS[i] = new int[size];
        for (int j = 0; j < size; ++j) {
            cache[i][j] = maxCrime >= values[i - 1][j];
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
    for (int i = 0; i < size; ++i) {
        dN[0][i] = cache[0][i] ? 0 : -1;
    }

    for (int i = 1; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (cache[i][j])
                dN[i][j] = dN[i - 1][j] + 1;
            else
                dN[i][j] = -1;
        }
    }

    for (int i = 0; i < size; ++i) {
        dS[size - 1][i] = cache[size - 1][i] ? 0 : -1;
    }

    for (int i = size - 2; i >= 0; --i) {
        for (int j = 0; j < size; ++j) {
            if (cache[i][j])
                dS[i][j] = dN[i + 1][j] + 1;
            else
                dS[i][j] = -1;
        }
    }

    TRect result;

    for (int i = size - 1; i >= 0; --i) {
        int maxS = size;
        for (int j = size - 1; j >= 0; --j) {
            ++maxS;
            if (cache[i][j] && (j == 0 || !cache[i][j - 1]))
            {
                int N = dN[i][j];
                int S = dS[i][j];
                int width = 1;
                while (j + width < size && cache[i][j + width])
                {
                    int nextN = dN[i][j + width], nextS = dS[i][j + width];
                    if (nextN < N || nextS < S)
                    {
                        if (S < maxS)
                        {
                            result.m_X = i - N;
                            result.m_Y = j;
                            result.m_W = width;
                            result.m_H = N + S + 1;
                            maxSuitableArea = result.m_W * result.m_H;
                        }
                        if (nextN < N) N = nextN;
                        if (nextS < S) S = nextS;
                    }
                    ++width;
                }
                if (S < maxS)
                {
                    result.m_X = i - N;
                    result.m_Y = j;
                    result.m_W = width;
                    result.m_H = N + S + 1;
                    maxSuitableArea = result.m_W * result.m_H;
                }
                maxS = 0;
            }
        }
    }

    //res = result;


    //freeing of cache allocated resources
    for (int i = 0; i < size; ++i) {
        delete[] cache[i];
    }

    delete[] cache;

    return maxSuitableArea != 0;
}