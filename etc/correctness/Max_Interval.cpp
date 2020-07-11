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
        sketch[i] = new interval_Sketch(k1);
        gt_ts.push_back(map<flow_t, double>());
        gt_ts[i].insert(make_pair(k2_128, k1));
    }


    rep2(i, 1, N){
        if(i %(N/10) == 0) cout << "Loading... "<< i/(N/100) << '%' << endl;
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
    rep2(i, 0, pack_in.size()){
        //print_uint128(pack_in[i].first); cout << ", " << fixed << pack_in[i].second << endl;
    }
    //cout << "********************" << endl;
    packet_full full_packet = get_packet(pack_in);

    for(int i = 0; i < 10; i++){
        int len = full_packet[i].second.size();
        packet_in this_switch = full_packet[i].second;
        vector<int> ind = full_packet[i].first;
        for(int j = 0; j < len; j++){
            double interval;
            if(ind[j]==1) {//cout << "curr time "<< this_switch[j].second << endl; 
            
            interval = (this_switch[j].second - sketch[i]->query(this_switch[j].first, 0));}
            
            else {interval = ABS((this_switch[j].second - sketch[i]->query(this_switch[j].first, false)));}
            
            sketch[i]->insert(this_switch[j].first, this_switch[j].second);
            
            
            //if(ind[j]==1){
                //cout << "gt curr time " << gt_ts[i][this_switch[j].first] << endl;
            //}

            if(gt_ts[i].find(this_switch[j].first)==gt_ts[i].end()){
                gt_ts[i].insert(make_pair(this_switch[j].first, this_switch[j].second));
            }
            
            double gt_interval = ABS((this_switch[j].second - gt_ts[i][this_switch[j].first]));

            //if(ind[j]==1){
            //    cout << interval << ',' << gt_interval << endl;
            //}
            if(ABS(gt_interval)>1e-7){
                curr_max_int[ind[j]] = max(curr_max_int[ind[j]], interval);
                gt_max_int[ind[j]] = max(gt_max_int[ind[j]], gt_interval);
            }
            else{
                curr_max_int[ind[j]] = max(curr_max_int[ind[j]], 0.0);
                gt_max_int[ind[j]] = max(gt_max_int[ind[j]], 0.0);
            }
            gt_ts[i][this_switch[j].first] = this_switch[j].second;
        
        }
    }
    
    rep2(i, 0, N){
        //if(i%100==0)
        //cout << curr_max_int[i] << ',' << gt_max_int[i] << endl;
    }
    
    double are = 0;
    double cnt = 0.0;
    double correct_cnt = 0.0;
    double wrong_cnt = 0.0;
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
    if(ABS(cnt) >1e-7) are /= cnt;
    cout << "flow number is " << flowmap.size() << endl;
    cout << "bucket number per row is " << Buck_Num_PerRow << endl;
    cout << "row number is " << Row_Num << endl;
    cout << "valid packet number is "<< cnt << endl;
    cout << "correct packet number is " << correct_cnt << endl;
    cout << "correct rate is " << correct_cnt/cnt << endl;
    cout << "are is " << are << endl;
    /*
    vector<pair<flow_t, uint32_t> > keyvalue(flowmap.begin(), flowmap.end());
    sort(keyvalue.begin(), keyvalue.end(), Cmp_By_Value);
    cout << "flow number is " << keyvalue.size() << endl;
    rep2(i, 0, 10){
        //print_uint128(keyvalue[i].first);
        //cout << ", "  << keyvalue[i].second << endl;
        cout << "flow size is " << keyvalue[i].second << endl;
        cout << "Interval Sketch result is:    ";
        cout << sketch->query(keyvalue[i].first) << endl;
        cout << "ground truth result is:       ";
        cout << GT[keyvalue[i].first][0] << endl;
    }
    unsigned flow_num = keyvalue.size();
    *//*
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
*/
    return 0;
}