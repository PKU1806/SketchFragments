#include"param.h"

//typedef pair<flow_t, vector<double>> packet_in;
typedef vector<pair<flow_t, double> > packet_in;
typedef vector<pair< vector<int>, packet_in>> packet_full;
double get_delay(){
    int tmp = rand() % 10000;
    return tmp*0.000001;
}
bool Comp_pair(const tuple<flow_t, double, int> & a, const tuple<flow_t, double, int> & b){
    return get<1>(a) < get<1>(b);
}
auto get_packet(packet_in input){
    vector<flow_t> flow = vector<flow_t>();
    vector<double> ts0 = vector<double>();
    
    vector<vector<double>> ts_full = vector<vector<double>>();
    int len = input.size();

    for(int i = 0; i < len; i++){
        flow.push_back(input[i].first);
        ts0.push_back(input[i].second);
        vector<double> tmp = vector<double>();
        tmp.push_back(ts0[i]);
        ts_full.push_back(tmp);
        //ts_full[i][0] = ts0[i];
        for(int j = 1; j < 10; j++){
            //ts_full[i][j] = ts_full[i][j-1] + get_delay();
            ts_full[i].push_back(ts_full[i][j-1]+get_delay());
        }
    }

    packet_full result = packet_full();
    //result[i]表示第i个交换机收到的包，result[i][j]表示第i个交换机收到的第j个包
    for(int i = 0; i < 10; i++){
        //packet_in pack_switch = packet_in();
        vector<tuple<flow_t, double, int>> pack_switch = vector<tuple<flow_t, double, int>>();
        for(int j = 0; j < len; j++){
            //pack_switch.push_back(make_pair(flow[j], ts_full[j][i]));
            pack_switch.push_back(tuple<flow_t, double, int>(flow[j], ts_full[j][i], j));
        }
        sort(pack_switch.begin(), pack_switch.end(), Comp_pair);
        vector<int> ind;
        packet_in flow_time;

        for(int j = 0; j < len; j++){
            ind.push_back(get<2>(pack_switch[j]));
            flow_time.push_back(make_pair(get<0>(pack_switch[j]), get<1>(pack_switch[j]) ) );
        }
        result.push_back(make_pair(ind, flow_time));
    }
    return result;
}