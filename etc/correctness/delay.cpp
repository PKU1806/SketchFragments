#include"half-CUSketch.h"
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
    srand(time(0));
    halfCUSketch* sketch;
    sketch = new halfCUSketch();
    ifstream fin(datapath, std::ios::binary);
    timestamp_t k1;
    flow_t k2_128;
    BOBHash32 *delay_hash; delay_hash = new BOBHash32(ABS(rand()%10));

    rep2(i, 0, N){
        if(i %(N/10) == 0) cout << "Loading... "<< i/(N/100) << '%' << endl;
        fin.read((char*)&k2_128, 13);
        fin.read((char*)&k1, sizeof(timestamp_t));
        unsigned delay;
        if(delay_hash->run((char*)&k2_128, 13)%2) delay = get_ChiSquare(1, 3);
        else delay = get_Union(0, 100);
        int delay_lev = get_delay_lev(delay);
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
    unsigned flow_num = keyvalue.size();
    
    cout << "********* statistical analysis *********" << endl;  
    double aae_sum = 0;
    double are_sum = 0;
    int aae_num = flow_num;
    int are_num = 100;
    rep2(i, 0, aae_num){
        rep2(j, 0, Lev_Num){
            aae_sum += (double)ABS((sketch->query_lev(keyvalue[i].first, j) - GT[keyvalue[i].first][j]));
        }
    }
    rep2(i, 0, are_num){
        rep2(j, 0, Lev_Num){
            are_sum += ((double)ABS((sketch->query_lev(keyvalue[i].first, j) - GT[keyvalue[i].first][j]))+1.0) / (GT[keyvalue[i].first][j]+1.0);
        }
    }
    aae_sum = aae_sum / (double)(aae_num*Lev_Num);
    are_sum = are_sum / (double)(are_num*Lev_Num);
    cout << "aae:                       " << aae_sum << " of top " << aae_num << " flows" << endl;
    cout << "are:                       " << are_sum << " of top " << are_num << " flows" << endl;
    
    cout << "Computing AAE and ARE..." << endl;
    ofstream stlog("./statistic.csv", std::ios::out | std::ios::trunc);
    for(int num = 1; num <= 3001; num += 10){
        aae_num = num, are_num = num;
        aae_sum = 0, are_sum = 0;
        rep2(i, 0, aae_num){
            rep2(j, 0, Lev_Num){
                aae_sum += (double)ABS((sketch->query_lev(keyvalue[i].first, j) - GT[keyvalue[i].first][j]));
            }
        }
        rep2(i, 0, are_num){
            rep2(j, 0, Lev_Num){
                are_sum += ((double)ABS((sketch->query_lev(keyvalue[i].first, j) - GT[keyvalue[i].first][j]))+1.0) / (GT[keyvalue[i].first][j]+1.0);
            }
        }
        aae_sum = aae_sum / (double)(aae_num*Lev_Num);
        are_sum = are_sum / (double)(are_num*Lev_Num);
        stlog << num << ',' << aae_sum << ',' << are_sum << endl;
    }
    stlog.close();


    cout << "Computing CDF..." << endl;
    ofstream cdflog("./cdf.csv", std::ios::out | std::ios::trunc);
#ifndef small
    int sta_num = 10;
    rep2(j, 0, sta_num){
        cdflog << "index,half-CU,GT,";
    }
    cdflog << endl;
    rep2(i, 0, Lev_Num){
        rep2(j, 0, sta_num){
            double allsum = (double)keyvalue[j].second;
            double tmp_currsum = 0, gt_currsum = 0;
            rep(k, 0, i){
                tmp_currsum += (double)sketch->query_lev(keyvalue[j].first, k);
                gt_currsum += (double)GT[keyvalue[j].first][k];
            }
            double algo_result = tmp_currsum / allsum;
            double gt_result = gt_currsum / allsum;
            cdflog << i << ',' << algo_result << ',' << gt_result << ',';
        }
        cdflog << endl;
    }
#else
    int ind[6] = {100, 101, 102, 1000, 1001, 1002};
    rep2(j, 0, 6){
        cdflog << "index,half-CU,GT,";
    }
    cdflog << endl;
    rep2(i, 0, Lev_Num){
        rep2(j, 0, 6){
            double allsum = (double)keyvalue[ind[j]].second;
            double tmp_allsum = 0;
            rep2(k, 0, Lev_Num) tmp_allsum += (double)sketch->query_lev(keyvalue[ind[j]].first, k);
            double tmp_currsum = 0, gt_currsum = 0;
            rep(k, 0, i){
                tmp_currsum += (double)sketch->query_lev(keyvalue[ind[j]].first, k);
                gt_currsum += (double)GT[keyvalue[ind[j]].first][k];
            }
            double algo_result = tmp_currsum / tmp_allsum;
            double gt_result = gt_currsum / allsum;
            cdflog << i << ',' << algo_result << ',' << gt_result << ',';
        }
        cdflog << endl;
    }
#endif
    cdflog.close();

    return 0;
}