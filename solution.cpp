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
#include <cudaGL.h>

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
    // todo
    return false;
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

    int maxSuitableArea = 0;
    TRect result;

    // todo O(n^3)

    return false;
}
