#include"Sketch.h"
#include"distribution.h"

using namespace std;

const int N = 1000000;
gtmap GT;
map<flow_t, uint32_t> flowmap;

bool Cmp_By_Value(const pair<flow_t, uint32_t> & lhs, const pair<flow_t, uint32_t> & rhs){
    return lhs.second > rhs.second;
}

int print_uint128(__uint128_t n) {
  if (n == 0)  return printf("0");

  char str[40] = {0}; // log10(1 << 128) + '\0'
  char *s = str + sizeof(str) - 1; // start at the end
  while (n != 0) {
    if (s == str) return -1; // never happens

    *--s = "0123456789"[n % 10]; // save last digit
    n /= 10;                     // drop it
  }
  return printf("%s", s);
}

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

        unsigned delay = get_Union(0,100);
        int delay_lev = get_delay_lev(delay);
        //cout << delay <<',' << delay_lev << endl;
        sketch->insert(k2_128, delay);
        if(flowmap.find(k2_128) == flowmap.end()){
            flowmap.insert(make_pair(k2_128, 1));
        }
        else{
            flowmap[k2_128] += 1;
        }
        if(GT.find(k2_128) == GT.end()){
            GT.insert(make_pair(k2_128, vector<unsigned>(begin(Zeros), end(Zeros))));
            GT[k2_128][delay_lev] += 1;
        }
        else{
            GT[k2_128][delay_lev] += 1;
        }
    }
    vector<pair<flow_t, uint32_t> > keyvalue(flowmap.begin(), flowmap.end());
    sort(keyvalue.begin(), keyvalue.end(), Cmp_By_Value);
    cout << "flow number is " << keyvalue.size() << endl;
    rep2(i, 0, 10){
        //print_uint128(keyvalue[i].first);
        //cout << ", "  << keyvalue[i].second << endl;
        cout << "flow size is " << keyvalue[i].second << endl;
        cout << "half-CU result is:         ";
        rep2(j, 0, Lev_Num){
            cout << sketch->query_lev(keyvalue[i].first, j) << ", ";
        }
        cout << endl;
        cout << "ground truth result is:    ";
        rep2(j, 0, Lev_Num){
            cout << GT[keyvalue[i].first][j] << ", ";
        }
        cout << endl;
    }
}