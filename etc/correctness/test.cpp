#include"Sketch.h"
#include"distribution.h"

using namespace std;

const int N = 1000000;
gtmap GT;

int main(){
    halfCUSketch* sketch;
    sketch = new halfCUSketch();
    ifstream fin(datapath, std::ios::binary);
    timestamp_t k1;
    flow_t k2_128;
    rep2(i, 0, N){
        if(i %(N/10) == 0) cout << i/(N/100) << '%' << endl;

        fin.read((char*)&k2_128, 13);
        fin.read((char*)&k1, sizeof(timestamp_t));

        unsigned delay = get_ChiSquare(5);
        int delay_lev = get_delay_lev(delay);
        sketch->insert(k2_128, delay);
        if(GT.find(k2_128) == GT.end()){
            GT.insert(make_pair(k2_128, vector<unsigned>(begin(Zeros), end(Zeros))));
            GT[k2_128][delay_lev] += 1;
        }
    }
}