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

#define EPSILON 10e-5

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

    //the rest. no conditionals, yay!
    for (int i = 1; i < actualSize; ++i) {
        for (int j = 1; j < actualSize; ++j) {
            cache[i][j] = values[i - 1][j - 1] + cache[i - 1][j] + cache[i][j - 1] - cache[i - 1][j - 1];
        }
    }

    int maxSuitableArea = 0, totalCost, area;

    //O(n^4) search
    for (int x = 1; x < actualSize; ++x) {
        for (int y = 1; y < actualSize; ++y) {
            for (int h = 1; h < x; ++h) {
                for (int w = 1; w < y; ++w) {

                    totalCost = values[x][y] - values[h - 1][w] - values[h][w - 1] + values[h - 1][w - 1];

                    if (totalCost < maxCost && (area = (y - h) * (x - w)) > maxSuitableArea) {
                        res->m_H = y - h;
                        res->m_W = x - w;
                        res->m_X = x - 1;
                        res->m_Y = y - 1;

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

    //building the cache
    for (int i = 0; i < size; ++i) {
        cache[i] = new bool[size];
        for (int j = 0; j < size; ++j) {
            cache[i][j] = maxCrime - values[i][j] > EPSILON;
        }
    }

    /*int maxSuitableArea = 0;
    TRect result;*/

    //TODO O(n^3)

    //freeing of cache allocated resources
    for (int i = 0; i < size; ++i) {
        delete[] cache[i];
    }

    delete[] cache;

    return false;
}
