#include"param.h"

int get_delay_lev(unsigned delay){
    rep2(i, 0, 9){
        if(delay < delayLevThres[i]) return i;
    }
    return 9;
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
    
    row(){
        bucket = new buck*[Buck_Num_PerRow];
        rep2(i, 0, Buck_Num_PerRow){
            bucket[i] = new buck();
        }
        bobhash = new BOBHash32(unsigned(rand()));
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
    unsigned query(const flow_t flow, const unsigned delay){
        int pos = get_hash(flow);
        return bucket[pos]->query(delay);
    }
};

class halfCUSketch{
public:
    row **Row;
    halfCUSketch(){
        Row = new row*[Row_Num];
        rep2(i, 0, Row_Num){
            Row[i] = new row();
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
            if(curr <= Min){
                Row[i]->insert(flow, delay);
                Min = curr;
            }
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