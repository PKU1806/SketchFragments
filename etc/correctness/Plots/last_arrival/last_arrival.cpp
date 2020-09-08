//#include"Interval_old.h"
#include <bits/stdc++.h>
#include "param.h"
#include"Interval.h"
#include"packet.h"
using namespace std;

const int N = 1000000;
//const int N = 1000;

map<flow_t, vector<double>> GT;
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

int calcu(ofstream &stlog){
    srand(time(0));
    //interval_Sketch* sketch_old;
    MinMaxSketch* sketch;
    //sketch_old = new interval_Sketch(0);
    sketch = new MinMaxSketch(0);
    /*
    rep2(i, 0, mistake_string.size()){
        mistake *= 10;
        mistake += mistake_string[i] - '0';
    }*/
    //print_uint128(mistake); cout << endl;
    ifstream fin(datapath, std::ios::binary);
    timestamp_t k1 = 0;
    flow_t k2_128 = 0;
    BOBHash32 *delay_hash; delay_hash = new BOBHash32(ABS(rand()%10));
    cout << 1 << endl;
    double GTMAX = 0;
    fin.read((char*)&k2_128, 13);
    fin.read((char*)&k1, sizeof(timestamp_t));
    if(flowmap.find(k2_128) == flowmap.end()){
        flowmap.insert(make_pair(k2_128, 1));
    }
    else{
        flowmap[k2_128] += 1;
    }
    //sketch = new MinMaxSketch(0);
    
    map<flow_t, timestamp_t> sketch_flow_last_timestamp;
    map<flow_t, timestamp_t> sketch_flow_max_interval;
    double aae = 0;
    double correct_cnt = 0;
    double allcnt = 0;
    rep2(i, 1, N){
        if(i %(N/10) == 0) std::cout << "Loading... "<< i/(N/100) << '%' << endl;
        k2_128 = 0, k1 = 0;
        fin.read((char*)&k2_128, 13);
        fin.read((char*)&k1, 8);
        //if(k2_128 != mistake) continue;
        if(flowmap.find(k2_128) == flowmap.end()){
            flowmap.insert(make_pair(k2_128, 1));
        }
        else{
            flowmap[k2_128] += 1;
        }

        double sketch_last_ts = sketch->last_arrival(k2_128);
        sketch->insert(k2_128, k1);
        //sketch_old->insert(k2_128,k1);
        //initialize ground truth
        double gt_last_ts = 0;
        if(sketch_flow_last_timestamp.find(k2_128)==sketch_flow_last_timestamp.end()){
            sketch_flow_last_timestamp.insert(make_pair(k2_128, k1));
            gt_last_ts = 0;
        }
        else{
            gt_last_ts = sketch_flow_last_timestamp[k2_128];
        }
        if(ABS(gt_last_ts)>1e-9){
            aae += ABS(gt_last_ts-sketch_last_ts);
            if(ABS(gt_last_ts-sketch_last_ts) < 1e-9) correct_cnt += 1;
            allcnt += 1;
        }

        if(sketch_flow_max_interval.find(k2_128)==sketch_flow_max_interval.end()){
            sketch_flow_max_interval.insert(make_pair(k2_128, 0));
        }
        double gt_interval = ABS(k1 - sketch_flow_last_timestamp[k2_128]);
        sketch_flow_max_interval[k2_128] = max(sketch_flow_max_interval[k2_128], gt_interval);
        sketch_flow_last_timestamp[k2_128] = k1;
        //if(gt_interval>GTMAX) GTMAX = gt_interval;
    }
    aae /= allcnt;
    correct_cnt /= allcnt;
    stlog << Row_Num*Buck_Num_PerRow*2*8/1024 << ',' << correct_cnt << ',' << aae << endl;
    /*
    double are = 0;
    double are_old = 0;
    double cnt = 0.0;
    double correct_cnt = 0.0;
    double wrong_cnt = 0.0;
    
    int topk = 1000;
    int falsecnt = 0;
    vector<pair<flow_t,unsigned>> sorttmp;
    map<flow_t, unsigned>::iterator sortit = flowmap.begin();
    while(sortit != flowmap.end()){
        sorttmp.push_back(*sortit);
        sortit++;
    }
    sort(sorttmp.begin(),sorttmp.end(),Comp);
    
    rep2(j, 0, topk){
        flow_t flowname = sorttmp[j].first;
        timestamp_t max_interval = sketch->query(flowname);
        timestamp_t gt_max_interval = sketch_flow_max_interval[flowname];
        //timestamp_t old_max_interval = sketch_old->query(flowname);
        //cout << max_interval << ',' << gt_max_interval << endl;
        //are_old += (ABS(gt_max_interval-old_max_interval))/(gt_max_interval);
        are += (ABS(gt_max_interval-max_interval))/(gt_max_interval);
        if(gt_max_interval>GTMAX) GTMAX = gt_max_interval;
    }
    
    are /= (topk);
    //are_old /= (topk);
    cout << "flow number is " << flowmap.size() << endl;
    cout << "bucket number per row is " << Buck_Num_PerRow << endl;
    cout << "row number is " << Row_Num << endl;
    cout << "are is     " << are << endl;
    //cout << "are_old is " << are_old << endl;
    cout << GTMAX << endl;
    stlog << (double)Buck_Num_PerRow*Row_Num*2*8/1024 << ',' << are << endl;
    //if(sketch_old) delete sketch_old;
    */
    if(sketch) delete sketch;
    if(delay_hash) delete delay_hash;
    return 0;
}

int main(){
    ofstream stlog("./row6_lastarrival_memory.csv", std::ios::out | std::ios::trunc);
    double basic_buck_num = 2048;
    Row_Num = 6;
    stlog << "Memory(kB), Correct Rate, AAE" << endl;
    while(basic_buck_num <= 1024*1024*4){
        Buck_Num_PerRow = (int)((double)basic_buck_num*3/(double)Row_Num);
        cout << Buck_Num_PerRow << endl;
        calcu(stlog);
        basic_buck_num *= 2;
    }
    return 0;
}