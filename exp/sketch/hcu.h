#ifndef HCUS_HEADER
#define HCUS_HEADER

#include "sketch.h"
#include "BOBHash32.h"

class HCUSketch : public Sketch
{
private:
	int size, num_hash, row_size;

	int *cnt, *fp;
	BOBHash32 *hash;

public:
	HCUSketch(int size, int num_hash);
	~HCUSketch();
	void init();
	void insert(int v);
	int query_freq(int v);
	void status();
};

#endif