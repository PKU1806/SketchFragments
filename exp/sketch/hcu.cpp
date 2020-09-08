#include "hcu.h"
#include "utils.h"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <climits>

HCUSketch::HCUSketch(int size, int num_hash) :
size(size), num_hash(num_hash)
{
	sprintf(name, "Half CUSketch");

	if (size <= 0 || num_hash <= 0)
	{
		panic("SIZE & NUM_HASH must be POSITIVE integers.");
	}
	cnt = new int[size];
	fp = new int[size];
	hash = new BOBHash32[num_hash];

	row_size = size / num_hash;
}

HCUSketch::~HCUSketch()
{
	if (cnt)
		delete [] cnt;
	if (fp)
		delete [] fp;
	if (hash)
		delete [] hash;
}

void
HCUSketch::init()
{
	memset(cnt, 0, size * sizeof(int));
	memset(fp, 0, size * sizeof(int));

	for (int i = 0; i < num_hash; ++i)
	{
		hash[i].initialize(i + 1); //rand() % MAX_PRIME32);
	}
}

void
HCUSketch::status()
{
	printf("bucket: %d   hash: %d\n", size, num_hash);
}

void
HCUSketch::insert(int v)
{
	int minp = INT_MAX;
	int i = 0, base = 0;

	for (i = 0, base = 0; i < num_hash; ++i, base += row_size)
	{
		int pos = hash[i].run((char*)&v, sizeof(int)) % row_size + base;
		if (cnt[pos] <= minp)
		{
			minp = cnt[pos];
			cnt[pos]++;
		}
	}	
}

int
HCUSketch::query_freq(int v)
{
	int ans = INT_MAX;
	int i = 0, base = 0;

	for (i = 0, base = 0; i < num_hash; ++i, base += row_size)
	{
		int pos = hash[i].run((char*)&v, sizeof(int)) % row_size + base;
		ans = min(ans, cnt[pos]);
	}

	return ans;
}

