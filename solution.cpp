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
            if (maxCrime > values[i - 1][j]) {
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
    for (int i = 0; i <= size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (cache[i][j] == 0)
                printf("      ");
            else
                printf("%5d ", cache[i][j]);
        }
        puts("");
    }
*/

    //O(n^2) search
    for (int i = 1; i <= size; ++i) {

    }

    //freeing of cache allocated resources
    for (int i = 0; i < size; ++i) {
        delete[] cache[i];
    }

    delete[] cache;

    return maxSuitableArea != 0;
}