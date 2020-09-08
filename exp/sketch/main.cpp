#include "utils.h"
#include "BOBHash32.h"
#include "cm.h"
#include "cu.h"
#include "hcu.h"

#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <stdint.h>
using namespace std;

#include <boost/program_options.hpp>
using namespace boost::program_options;

#define ft first
#define sc second

vector<int> flow;
string file_name;

void ParseArg(int argc, char *argv[])
{
    options_description opts("Benchmark Options");

    opts.add_options()
/*      ("verbose,v", "print more info")
        ("debug,d", "print debug info")
        ("filename,f", value<string>()->required(), "file dir")*/
        ("help,h", "print help info")
        ;
    variables_map vm;
    
    store(parse_command_line(argc, argv, opts), vm);

    if(vm.count("help"))
    {
        cout << opts << endl;
        return;
    }
}

void LoadData_Mininet(char file[])
{
    BOBHash32 hash_id;
    hash_id.initialize(1122);
    // hash_id.initialize(rand()%MAX_PRIME32);
    
    ifstream is(file, ios::in);
    string buf;

    int cnt = 0;
    while (getline(is, buf))
    {
        flow.push_back(hash_id.run(buf.c_str(), buf.length()));
        cnt++;
    }

    cout << "Loading complete. " << cnt << endl;
}


void FreqEstTest(vector<Sketch*>& sk, vector<int>& flow, bool init_first = false)
{
    static map<int, int> flow_cnt;
    int n = flow.size(), n_sk = sk.size();
    
    flow_cnt.clear();
    for (int i = 0; i < n; ++i)
        flow_cnt[ flow[i] ] = 0;
    for (int i = 0; i < n; ++i)
        flow_cnt[ flow[i] ]++;

    for (int id = 0; id < n_sk; ++id)
    {
        if (init_first)
            sk[id]->init();

        for (int i = 0; i < n; ++i)
        {
            // if(id == 0)
            //     printf("insert %d\n", flow[i]);
            sk[id]->insert(flow[i]);
        }
    }

    printf("total_flow: %d\n", flow_cnt.size());

    // summary
    printf("Frequency Estimation Test\n");
    printf("-------------------------------------\n");

    fstream fout(file_name, ios::out | ios::app);
    for (int id = 0; id < n_sk; ++id)
    {
        printf("%d) %s\n", id, sk[id]->name);
        
        int sz = flow_cnt.size();
        double pr = 0, are = 0, aae = 0;

        for (auto item: flow_cnt)
        {
            int est = sk[id]->query_freq(item.ft);
            int err = abs(est - item.sc);
            // printf("%d %d\n", est, item.sc);
            // printf("  %3d: %11d\t%5d\t%5d\t%5d\t%5d\n", i, my_res[i].sc, my_res[i].ft,
            //                                  sk[id]->query_freq(my_res[i].sc),
            //                                  flow_cnt[my_res[i].sc], int(err));
            are += (double)err / item.sc;
            aae += (double)err;
            pr += (est == item.sc);

            // if (id == 0)
            // {
            //     int lo_est = ((VSketch*)sk[id])->query_freq_low(item.ft);
            //     if (lo_est > item.sc || est < item.sc)
            //     {
            //         printf("bad!\n");
            //     }
            //     fout << item.sc << ' ' << est << ' ' << lo_est << endl;
            // }
        }

        are /= sz;
        aae /= sz;
        pr /= sz;

        sk[id]->status();
        printf("Pr: %.3lf  ARE: %1.3lf(%.3lf)  AAE: %1.3lf(%.3lf)\n", pr, are, log10(are), aae, log10(aae));
        fout << pr << ' ' << are << ' ' << log10(are) << ' ';
        fout << aae << ' ' << log10(aae) << endl;
    }
}

int main(int argc, char *argv[])
{
    srand(2020);
    // parse args
    ParseArg(argc, argv);

    // load data
    LoadData_Mininet("../trace/s1_10k.dat");
    // LoadData_WebPage("data/webdocs_form00.dat");
    
    file_name = string("../log/log_fe_mininet10k");

    // test
    if (true)
    {
        for (int i = 2; i <= 11; ++i)
        {
            // memory - KB
            int mem = i * 100;//(i == 1 ? 50 : i*100);
            if (i == 0)
                mem = 50;

            vector<Sketch*> sk;
            HCUSketch *hcu = new HCUSketch(16 * i * 3, 3);
            CMSketch *cms = new CMSketch(16 * i * 3, 3);
            CUSketch *cus = new CUSketch(16 * i * 3, 3);

            sk.push_back(hcu);
            sk.push_back(cms);
            sk.push_back(cus);
            
            FreqEstTest(sk, flow, true);

            delete hcu;
        }
    }

    return 0;
}
