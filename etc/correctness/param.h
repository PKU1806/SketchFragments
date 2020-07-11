#ifndef Param
#define Param
#include<bits/stdc++.h>
#include"BOBHash32.h"

#define fi first
#define se second
#define rep(i,a,b) for(int (i)=(a);(i)<=(b);(i)++)
#define rep2(i,a,b) for(int (i)=(a);(i)<(b);(i)++)
#define END_FILE_NO 1
#define START_FILE_NO 0
#define KEY_LENGTH 8
#define ITEM_LENGTH 16
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define ABS(a) ((a)>0?(a):(-(a)))

#define Lev_Num 10
#define Buck_Num_PerRow 500
#define Row_Num 3

#define znq
#define small

#ifdef znq
#define datapath "/mnt/e/Research/2020SketchFragments/130000.dat"
#else
#define datapath "/usr/share/dataset/CAIDA2018/dataset/130000.dat"
#endif

typedef __uint128_t flow_t;
typedef double_t timestamp_t;
typedef map<__uint128_t, vector<unsigned> > gtmap;
__uint128_t mistake = 0;
//unsigned delayLevThres[Lev_Num] = {3, 10, 30, 70, 180, 400, 1000, 2000, 5000, 15000};
unsigned delayLevThres[Lev_Num] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
unsigned Zeros[Lev_Num] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#endif