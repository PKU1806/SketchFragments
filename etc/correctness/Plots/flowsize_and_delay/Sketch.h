#ifndef SKETCH
#define SKETCH
#include"param.h"

int get_delay_lev(unsigned delay){
    rep2(i, 0, Lev_Num-1){
        if(delay < delayLevThres[i]) return i;
    }
    return Lev_Num-1;
}

class buck{
public:
    unsigned *Bin;
    buck(){
        Bin = new unsigned[Lev_Num];
        rep2(i, 0, Lev_Num) Bin[i] = 0;
    }
    void insert(unsigned delay){
        int delayLev = get_delay_lev(delay);
        Bin[delayLev] += 1;
    }
    unsigned query(unsigned delay){
        int lev = get_delay_lev(delay);
        return Bin[lev];
    }
    ~buck(){
        if(Bin)delete[] Bin;
    }
};

class row{
public:
    buck **bucket;
    BOBHash32 *bobhash;
    bool *mask;
    
    row(unsigned seed){
        bucket = new buck*[Buck_Num_PerRow];
        mask = new bool[Buck_Num_PerRow];
        rep2(i, 0, Buck_Num_PerRow){
            bucket[i] = new buck();
            int randnum = rand()%100;
            if(randnum < 100*collect_rate) mask[i] = 1;
            else mask[i] = 0;
        }
        bobhash = new BOBHash32(seed);

    }
    ~row(){
        rep2(i, 0, Buck_Num_PerRow) if(bucket[i]) delete bucket[i];
        if(bobhash) delete bobhash;
        delete bucket;
    }
    
    int get_hash(const flow_t flow){
        return bobhash->run((char*)&flow, 13) % Buck_Num_PerRow;
    }
    void insert(const flow_t flow, const unsigned delay){
        int pos = get_hash(flow);
        bucket[pos]->insert(delay);
    }
    int query(const flow_t flow, const unsigned delay){
        int pos = get_hash(flow);
        //cout << "pos " << pos << endl;
        if(mask[pos])return (int)bucket[pos]->query(delay);
        else return -1; 
    }
};

class halfCUSketch{
public:
    row **Row;
    halfCUSketch(){
        Row = new row*[Row_Num];
        rep2(i, 0, Row_Num){
            Row[i] = new row((unsigned)i);
        }
    }
    ~halfCUSketch(){
        rep2(i, 0, Row_Num) if(Row[i]) delete Row[i];
        if(Row) delete Row;
    }

    void insert(const flow_t flow, const unsigned delay){
        unsigned Min = 0x5fffffff;
        rep2(i, 0, Row_Num){
            unsigned curr = Row[i]->query(flow, delay);
            //cout << "curr " << curr << endl;
            if(curr <= Min){
                Row[i]->insert(flow, delay);
                Min = curr;
            }
        }
    }
    int query(const flow_t flow, const unsigned delay){
        int Min = 0x1fffffff;
        bool flag = 0;
        rep2(i, 0, Row_Num){
            int curr = Row[i]->query(flow, delay);
            if(curr != -1){
                flag = 1;
                if(curr <= Min) Min = curr;
            }
        }
        if(flag) return Min;
        else return -1;
    }
    unsigned query_lev(const flow_t flow, const int lev){
        unsigned Min = 0x5fffffff;
        bool flag = 0;
        rep2(i, 0, Row_Num){
            int pos = Row[i]->get_hash(flow);
            bool status = Row[i]->mask[pos];
            if(status){
                int curr = Row[i]->bucket[pos]->Bin[lev];
                if(curr <= Min) Min = curr;
                flag = 1;
            }
            
        }
        if(flag) return Min;
        else return -1;
    }
};

class CMSketch{
public:
    row **Row;
    CMSketch(){
        Row = new row*[Row_Num];
        rep2(i, 0, Row_Num){
            Row[i] = new row((unsigned)i);
        }
    }
    ~CMSketch(){
        rep2(i, 0, Row_Num) if(Row[i]) delete Row[i];
        if(Row) delete Row;
    }

    void insert(const flow_t flow, const unsigned delay){
        rep2(i, 0, Row_Num){
            Row[i]->insert(flow, delay);
        }
    }
    unsigned query(const flow_t flow, const unsigned delay){
        unsigned Min = 0x5fffffff;
        rep2(i, 0, Row_Num){
            unsigned curr = Row[i]->query(flow, delay);
            if(curr <= Min) Min = curr;
        }
        return Min;
    }
    unsigned query_lev(const flow_t flow, const int lev){
        unsigned Min = 0x5fffffff;
        rep2(i, 0, Row_Num){
            int pos = Row[i]->get_hash(flow);
            unsigned curr = Row[i]->bucket[pos]->Bin[lev];
            if(curr <= Min) Min = curr;
        }
        return Min;
    }
};

class CUSketch{
public:
    row **Row;
    CUSketch(){
        Row = new row*[Row_Num];
        rep2(i, 0, Row_Num){
            Row[i] = new row((unsigned)i);
        }
    }
    ~CUSketch(){
        rep2(i, 0, Row_Num) if(Row[i]) delete Row[i];
        if(Row) delete Row;
    }

    void insert(const flow_t flow, const unsigned delay){
        unsigned Min = 0x5fffffff;
        rep2(i, 0, Row_Num){
            unsigned curr = Row[i]->query(flow, delay);
            //cout << "curr " << curr << endl;
            if(curr < Min){
                Min = curr;
            }
        }
        rep2(i, 0, Row_Num){
            unsigned curr = Row[i]->query(flow, delay);
            if(curr == Min) Row[i]->insert(flow, delay);
        }
    }
    unsigned query(const flow_t flow, const unsigned delay){
        unsigned Min = 0x5fffffff;
        rep2(i, 0, Row_Num){
            unsigned curr = Row[i]->query(flow, delay);
            if(curr <= Min) Min = curr;
        }
        return Min;
    }
    unsigned query_lev(const flow_t flow, const int lev){
        unsigned Min = 0x5fffffff;
        rep2(i, 0, Row_Num){
            int pos = Row[i]->get_hash(flow);
            unsigned curr = Row[i]->bucket[pos]->Bin[lev];
            if(curr <= Min) Min = curr;
        }
        return Min;
    }
};
#endif