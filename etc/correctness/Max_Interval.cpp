#include"Interval.h"
#include"packet.h"

using namespace std;

const int N = 100000;
//const int N = 1000;
string mistake_string("510116804101083034681783880495");

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

int main(){
    //srand(time(0));
    interval_Sketch** sketch;
    //sketch = new interval_Sketch();
    rep2(i, 0, mistake_string.size()){
        mistake *= 10;
        mistake += mistake_string[i] - '0';
    }
    //print_uint128(mistake); cout << endl;
    ifstream fin(datapath, std::ios::binary);
    timestamp_t k1 = 0;
    flow_t k2_128 = 0;
    BOBHash32 *delay_hash; delay_hash = new BOBHash32(ABS(rand()%10));
    packet_in pack_in = packet_in();
    
    vector<double> curr_max_int;
    vector<double> gt_max_int;
    vector<map<flow_t, double>> gt_ts;

    fin.read((char*)&k2_128, 13);
    fin.read((char*)&k1, sizeof(timestamp_t));
    pack_in.push_back( pair<flow_t, double>(k2_128, k1) );
    curr_max_int.push_back(0.0);
    gt_max_int.push_back(0.0);
    if(flowmap.find(k2_128) == flowmap.end()){
        flowmap.insert(make_pair(k2_128, 1));
    }
    else{
        flowmap[k2_128] += 1;
    }
    sketch = new interval_Sketch*[10];
    for(int i = 0; i < 10; i++){
        sketch[i] = new interval_Sketch(0);
        gt_ts.push_back(map<flow_t, double>());
        gt_ts[i].insert(make_pair(k2_128, k1));
    }


    rep2(i, 1, N){
        if(i %(N/10) == 0) std::cout << "Loading... "<< i/(N/100) << '%' << endl;
        k2_128 = 0, k1 = 0;
        fin.read((char*)&k2_128, 13);
        fin.read((char*)&k1, 8);
        //if(k2_128 != mistake) continue;
        pack_in.push_back( make_pair(k2_128, k1) );
        curr_max_int.push_back(0.0);
        gt_max_int.push_back(0.0);
        if(flowmap.find(k2_128) == flowmap.end()){
            flowmap.insert(make_pair(k2_128, 1));
        }
        else{
            flowmap[k2_128] += 1;
        }
    }

    packet_full full_packet = get_packet(pack_in);

    vector<map<flow_t, timestamp_t>> sketch_flow_last_timestamp;
    vector<map<flow_t, timestamp_t>> sketch_flow_max_interval;

    for(int i = 0; i < 10; i++){
        sketch_flow_last_timestamp.push_back(map<flow_t, timestamp_t>());
        sketch_flow_max_interval.push_back(map<flow_t, timestamp_t>());
        int len = full_packet[i].second.size();
        packet_in this_switch = full_packet[i].second;
        vector<int> ind = full_packet[i].first;
        for(int j = 0; j < len; j++){
            double interval;
            interval = ABS((this_switch[j].second - sketch[i]->query(this_switch[j].first, false)));
            
            sketch[i]->insert(this_switch[j].first, this_switch[j].second);
            
            //initialize ground truth
            if(sketch_flow_last_timestamp[i].find(this_switch[j].first)==sketch_flow_last_timestamp[i].end()){
                sketch_flow_last_timestamp[i].insert(make_pair(this_switch[j].first, this_switch[j].second));
            }
            if(sketch_flow_max_interval[i].find(this_switch[j].first)==sketch_flow_max_interval[i].end()){
                sketch_flow_max_interval[i].insert(make_pair(this_switch[j].first, 0));
            }
            //cout << 1 << endl;
            if(gt_ts[i].find(this_switch[j].first)==gt_ts[i].end()){
                gt_ts[i].insert(make_pair(this_switch[j].first, this_switch[j].second));
            }
            
            double gt_interval = ABS((this_switch[j].second - gt_ts[i][this_switch[j].first]));

            gt_interval = ABS(this_switch[j].second - sketch_flow_last_timestamp[i][this_switch[j].first]);
            
            if(ABS(gt_interval)>1e-7){
                curr_max_int[ind[j]] = max(curr_max_int[ind[j]], interval);
                gt_max_int[ind[j]] = max(gt_max_int[ind[j]], gt_interval);
            }
            else{
                curr_max_int[ind[j]] = max(curr_max_int[ind[j]], 0.0);
                gt_max_int[ind[j]] = max(gt_max_int[ind[j]], 0.0);
            }
            //update ground truth
            sketch_flow_max_interval[i][this_switch[j].first] = max(sketch_flow_max_interval[i][this_switch[j].first], gt_interval);

            gt_ts[i][this_switch[j].first] = this_switch[j].second;
            sketch_flow_last_timestamp[i][this_switch[j].first] = this_switch[j].second;
        }
    }
    
    double are = 0;
    double cnt = 0.0;
    double correct_cnt = 0.0;
    double wrong_cnt = 0.0;
    /*
    rep2(i, 0, pack_in.size()){
        double algo = curr_max_int[i], gt_int = gt_max_int[i];
        //cout << 1 << endl;
        if(ABS(gt_int) > 1e-7) are += ABS((gt_int-algo))/ABS(gt_int), cnt += 1.0;
        if(ABS(gt_int) > 1e-7 && ABS((gt_int-algo)) < 1e-7) correct_cnt += 1.0;
        if(ABS(gt_int) > 1e-7 && algo > gt_int) {
            wrong_cnt += 1.0;
            cout << i << ", " << curr_max_int[i] << ", " << fixed << gt_max_int[i] << endl;
            print_uint128(pack_in[i].first); cout << ", " <<fixed<< pack_in[i].second << endl;
        }
    }
    */
    int topk = 100;
    int falsecnt = 0;
    //map<flow_t, unsigned> ordered_flowmap;
    vector<pair<flow_t,unsigned>> sorttmp;
    map<flow_t, unsigned>::iterator sortit = flowmap.begin();
    while(sortit != flowmap.end()){
        sorttmp.push_back(*sortit);
        sortit++;
    }
    sort(sorttmp.begin(),sorttmp.end(),Comp);
    rep2(i, 0, 10){

        rep2(j, 0, topk){
            flow_t flowname = sorttmp[j].first;
            timestamp_t max_interval = sketch[i]->query(flowname);
            timestamp_t gt_max_interval = sketch_flow_max_interval[i][flowname];
            if(max_interval > gt_max_interval && i==0) cout << max_interval << ',' << gt_max_interval << endl;
                 
        
            are += (ABS(gt_max_interval-max_interval))/(gt_max_interval);

        }
    }
    //cout << flowmap[]
    cout << falsecnt << endl;
    are /= (topk*10);
    //if(ABS(cnt) >1e-7) are /= cnt;
    cout << "flow number is " << flowmap.size() << endl;
    cout << "bucket number per row is " << Buck_Num_PerRow << endl;
    cout << "row number is " << Row_Num << endl;
    //cout << "valid packet number is "<< cnt << endl;
    //cout << "correct packet number is " << correct_cnt << endl;
    //cout << "correct rate is " << correct_cnt/cnt << endl;
    cout << "are is " << are << endl;
    return 0;
}