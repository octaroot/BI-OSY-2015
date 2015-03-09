/*
 * Test sequential functions, validate results
 */
#include "solution.cpp"

const int RES_SIZE = 50;
const int COST_SIZE = 10;
const int CRIME_SIZE = 10;

struct TCostRes
 {
   int m_CostMax;
   int m_Area;
 };

struct TCrimeRes
 {
   double m_CrimeMax;
   int m_Area;
 };

static int g_CostMat[COST_SIZE * COST_SIZE] =
 {
   824,  757,  896,  255,  189,  404,  277,  914,  564,   18,
   -85,   41,   61,   30,  981,  436,  548,  569,  398,  176,
   470,  326,  197,  764,  -52,  456,   77,  846,  -59,  369,
   148,  874,  173,  285,  433,  460,  338,  879,  163,  559,
   643,  361,  791,  118,  251,  915,  576,  330,  283,  590,
   413,  916,   29,  575,  549,  728,  354,   64,  463,   19,
    -6,  413,  945,  619,  687,  185,  501,    9,  -26,   13,
   -52,  -74,   62,  733,  568,  549,  -40,  428,  -74,  333,
   494,  -71,  965,  843,  960,  817, -162,  503,   19,  826,
   259,  498,  621,  429,  170,  468,  659, -132,  734,  777
 };

static double g_CrimeMat[CRIME_SIZE * CRIME_SIZE] =
 {
  244.75, 186.50, 226.00, 245.75, 167.50, 125.50,  43.00, 207.75, 222.50,  21.50,
  163.25,  63.75, 158.25,  59.00, 175.75,  80.75,  83.75,  59.75,  20.75, 159.00,
   57.75, 163.50, 128.75, 242.75,  71.75, 137.50, 180.50,  30.50, 119.00, 149.00,
  236.00, 114.00,  85.50, 212.25, 110.00,   3.25,  87.75, 150.50, 208.50,  60.25,
  169.50, 122.00, 121.75,  77.75, 178.50,  47.50, 156.25,  12.50, 104.75, 174.75,
  169.25, 160.25,  88.25,  48.00, 153.25, 157.50, 183.25,  83.75, 185.75,  52.50,
  230.25, 171.75, 164.00,  66.00, 134.25,  24.00,  66.75, 219.50, 172.25,  25.50,
   30.00,  91.75, 145.00, 149.25, 167.25,  73.75, 194.50,  73.75,  84.00,  49.25,
  246.00,   3.25, 207.25,  84.50,  49.00, 110.50, 239.75, 229.75, 191.75, 175.50,
   32.25, 172.00,  97.25, 194.00, 235.75, 229.25, 215.75,  52.75, 198.75, 138.00
 };

static TCostRes g_CostRes [] =
 {
   { -234, 0 }, { -214, 0 }, { -156, 2 }, { -59, 3 }, { 76, 4 },
   { 251, 4 }, { 465, 4 }, { 718, 6 }, { 1009, 7 }, { 1340, 9 },
   { 1709, 9 }, { 2117, 12 }, { 2564, 12 }, { 3050, 12 }, { 3575, 16 },
   { 4139, 16 }, { 4741, 18 }, { 5383, 20 }, { 6063, 21 }, { 6782, 24 },
   { 7540, 24 }, { 8337, 28 }, { 9173, 28 }, { 10047, 32 }, { 10961, 32 },
   { 11913, 36 }, { 12905, 36 }, { 13935, 40 }, { 15004, 40 }, { 16112, 42 },
   { 17258, 45 }, { 18444, 48 }, { 19668, 50 }, { 20932, 54 }, { 22234, 56 },
   { 23575, 63 }, { 24955, 64 }, { 26374, 70 }, { 27832, 70 }, { 29328, 72 },
   { 30864, 80 }, { 32438, 81 }, { 34051, 81 }, { 35704, 90 }, { 37395, 90 },
   { 39124, 90 }, { 40893, 100 }, { 42701, 100 }, { 44547, 100 }, { 46433, 100 }
 };

static TCrimeRes g_CrimeRes [] =
 {
   { 0.00, 0 }, { 35.00, 1 }, { 49.50, 2 }, { 60.75, 3 }, { 70.25, 3 },
   { 78.50, 3 }, { 86.00, 4 }, { 92.75, 4 }, { 99.25, 4 }, { 105.25, 4 },
   { 111.00, 4 }, { 116.50, 4 }, { 121.50, 4 }, { 126.50, 4 }, { 131.25, 4 },
   { 136.00, 4 }, { 140.50, 5 }, { 144.75, 5 }, { 149.00, 5 }, { 153.00, 5 },
   { 157.00, 6 }, { 161.00, 9 }, { 164.75, 10 }, { 168.50, 12 }, { 172.00, 15 },
   { 175.50, 15 }, { 179.00, 20 }, { 182.50, 20 }, { 185.75, 21 }, { 189.25, 21 },
   { 192.25, 21 }, { 195.50, 24 }, { 198.75, 24 }, { 201.75, 24 }, { 204.75, 24 },
   { 207.75, 25 }, { 210.75, 30 }, { 213.75, 30 }, { 216.50, 30 }, { 219.25, 30 },
   { 222.25, 45 }, { 225.00, 48 }, { 227.75, 48 }, { 230.25, 48 }, { 233.00, 48 },
   { 235.50, 48 }, { 238.25, 50 }, { 240.75, 63 }, { 243.25, 81 }, { 246.00, 100 }
 };
//-------------------------------------------------------------------------------------------------
void               costTest                                ( void )
 {
   int * mat[COST_SIZE];

   for ( int i = 0; i < COST_SIZE; i ++ )
    mat[i] = g_CostMat + i * COST_SIZE;

   for ( int i = 0; i < RES_SIZE; i ++ )
    {
      TRect r;
      if ( FindByCost ( mat, COST_SIZE, g_CostRes [i] . m_CostMax, &r ) )
       {
         if ( g_CostRes[i] . m_Area != r . m_W * r . m_H )
          printf ( "Bad (%d). Expected %d, got %d (%d x %d) @ (%d, %d)\n", g_CostRes[i].m_CostMax , g_CostRes[i] . m_Area, r . m_W * r . m_H, r . m_W, r . m_H, r.m_X, r.m_Y);
         else
             printf("Good (%d = %dm^2)\n", g_CostRes[i].m_CostMax, g_CostRes[i].m_Area);
       }
      else
       {
         if ( g_CostRes[i] . m_Area != 0 )
             printf ( "Bad (%d). Expected %d, got false\n", g_CostRes[i].m_CostMax , g_CostRes[i] . m_Area);
           else
             printf("Good (%d = %dm^2)\n", g_CostRes[i].m_CostMax, g_CostRes[i].m_Area);
       }
    }
 }
//-------------------------------------------------------------------------------------------------
void               crimeTest                               ( void )
 {
   double * mat[CRIME_SIZE];

   for ( int i = 0; i < CRIME_SIZE; i ++ )
    mat[i] = g_CrimeMat + i * CRIME_SIZE;

   for ( int i = 0; i < RES_SIZE; i ++ )
    {
      TRect r;
      if ( FindByCrime ( mat, CRIME_SIZE, g_CrimeRes [i] . m_CrimeMax, &r ) )
      {
          if ( g_CrimeRes[i] . m_Area != r . m_W * r . m_H )
              printf ( "Bad (%lf). Expected %d, got %d (%d x %d) @ (%d, %d)\n", g_CrimeRes[i].m_CrimeMax , g_CrimeRes[i] . m_Area, r . m_W * r . m_H, r . m_W, r . m_H, r.m_X, r.m_Y);
          else
              printf("Good (%lf = %dm^2)\n", g_CrimeRes[i].m_CrimeMax, g_CrimeRes[i].m_Area);
      }
      else
      {
          if ( g_CrimeRes[i] . m_Area != 0 )
              printf ( "Bad (%lf). Expected %d, got false\n", g_CrimeRes[i].m_CrimeMax , g_CrimeRes[i] . m_Area);
          else
              printf("Good (%lf = %dm^2)\n", g_CrimeRes[i].m_CrimeMax, g_CrimeRes[i].m_Area);
      }
    }
 }
//-------------------------------------------------------------------------------------------------
int                main                                    ( void )
 {
   //costTest ();
   crimeTest ();

   return 0;
 }
