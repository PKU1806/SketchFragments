#include"param.h"

int get_delay_lev(unsigned delay){
    rep2(i, 0, 9){
        if(delay < delayLevThres[i]) return i;
    }
    return 9;
}

class buck{
public:
    double delta_t;
    double last_ts;
    buck(double lt = 0.0){
        delta_t = 0.0, last_ts = lt;
    }
    void insert(double ts){
        if(last_ts < 1e-5) {last_ts = ts; return;}
        double tmp = (ts - last_ts);
        delta_t = max(delta_t, tmp);
        last_ts = ts;
    }
    void init(double ts){
        last_ts = ts; delta_t = 0;
    }
    double query(){
        //cout << delta_t << endl;
        return delta_t;
    }
    ~buck(){ }
};

class row{
public:
    buck **bucket;
    BOBHash32 *bobhash;
    
    row(unsigned seed, double ts = 0.0){
        bucket = new buck*[Buck_Num_PerRow];
        rep2(i, 0, Buck_Num_PerRow){
            bucket[i] = new buck(ts);
        }
        bobhash = new BOBHash32((unsigned)seed);
    }
    ~row(){
        rep2(i, 0, Buck_Num_PerRow) if(bucket[i]) delete bucket[i];
        if(bobhash) delete bobhash;
        delete bucket;
    }
    
    int get_hash(const flow_t flow){
        return bobhash->run((char*)&flow, 13) % Buck_Num_PerRow;
    }
    void insert(const flow_t flow, const double timestamp){
        int pos = get_hash(flow);
        //if(bucket[pos]->last_ts < 1e-7) bucket[pos]->init(timestamp);
        //else bucket[pos]->insert(timestamp);
        bucket[pos]->insert(timestamp);
    }
    double query(const flow_t flow){
        int pos = get_hash(flow);
        //cout << "pos " << pos << endl;
        return bucket[pos]->query();
    }
};

class interval_Sketch{
public:
    row **Row;
    interval_Sketch(double ts = 0.0){
        Row = new row*[Row_Num];
        rep2(i, 0, Row_Num){
            Row[i] = new row((unsigned)i,ts);
        }
    }
    ~interval_Sketch(){
        rep2(i, 0, Row_Num) if(Row[i]) delete Row[i];
        if(Row) delete Row;
    }

    void insert(const flow_t flow, const double timestamp){
        rep2(i, 0, Row_Num){
            Row[i]->insert(flow, timestamp);
        }
    }
    double query(const flow_t flow, bool flag=0){
        double Min = 1e12;
        
        rep2(i, 0, Row_Num){
            double curr = Row[i]->query(flow);
            if(curr <= Min) Min = curr;
        }
        return Min;
    }
};