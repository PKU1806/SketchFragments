#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <ctime>

using namespace std;

vector<pair<string, vector<string>>> up_link;
vector<pair<string, vector<string>>> down_link;
vector<string> done_server;
vector<string> server = {"h1", "h2", "h3", "h4", "h5", "h6", "h7", "h8", "h9", "h10", "h11", "h12", "h13", "h14", "h15", "h16"};
vector<pair<string, vector<int>>> fragments;
vector<string> aggregated_switch_array;
int server_num = 16;
int switch_num = 20;
int random_bound = 10;
int sketch_fragment_num = 64 * 3;

void init_up_link(vector<pair<string, vector<string>>> &link){
    link.push_back(make_pair("h1", vector<string> {"s13"}));
    link.push_back(make_pair("h2", vector<string> {"s13"}));
    link.push_back(make_pair("h3", vector<string> {"s14"}));
    link.push_back(make_pair("h4", vector<string> {"s14"}));
    link.push_back(make_pair("h5", vector<string> {"s15"}));
    link.push_back(make_pair("h6", vector<string> {"s15"}));
    link.push_back(make_pair("h7", vector<string> {"s16"}));
    link.push_back(make_pair("h8", vector<string> {"s16"}));
    link.push_back(make_pair("h9", vector<string> {"s17"}));
    link.push_back(make_pair("h10", vector<string> {"s17"}));
    link.push_back(make_pair("h11", vector<string> {"s18"}));
    link.push_back(make_pair("h12", vector<string> {"s18"}));
    link.push_back(make_pair("h13", vector<string> {"s19"}));
    link.push_back(make_pair("h14", vector<string> {"s19"}));
    link.push_back(make_pair("h15", vector<string> {"s20"}));
    link.push_back(make_pair("h16", vector<string> {"s20"}));
    link.push_back(make_pair("s13", vector<string> {"s5", "s6"}));
    link.push_back(make_pair("s14", vector<string> {"s5", "s6"}));
    link.push_back(make_pair("s15", vector<string> {"s7", "s8"}));
    link.push_back(make_pair("s16", vector<string> {"s7", "s8"}));
    link.push_back(make_pair("s17", vector<string> {"s9", "s10"}));
    link.push_back(make_pair("s18", vector<string> {"s9", "s10"}));
    link.push_back(make_pair("s19", vector<string> {"s11", "s12"}));
    link.push_back(make_pair("s20", vector<string> {"s11", "s12"}));
    link.push_back(make_pair("s5", vector<string> {"s1", "s2"}));
    link.push_back(make_pair("s6", vector<string> {"s3", "s4"}));
    link.push_back(make_pair("s7", vector<string> {"s1", "s2"}));
    link.push_back(make_pair("s8", vector<string> {"s3", "s4"}));
    link.push_back(make_pair("s9", vector<string> {"s1", "s2"}));
    link.push_back(make_pair("s10", vector<string> {"s3", "s4"}));
    link.push_back(make_pair("s11", vector<string> {"s1", "s2"}));
    link.push_back(make_pair("s12", vector<string> {"s3", "s4"}));
}

void init_down_link(vector<pair<string, vector<string>>> &link){
    link.push_back(make_pair("s1", vector<string> {"s5", "s7", "s9", "s11"}));
    link.push_back(make_pair("s2", vector<string> {"s5", "s7", "s9", "s11"}));
    link.push_back(make_pair("s3", vector<string> {"s6", "s8", "s10", "s12"}));
    link.push_back(make_pair("s4", vector<string> {"s6", "s8", "s10", "s12"}));
    link.push_back(make_pair("s5", vector<string> {"s13", "s14"}));
    link.push_back(make_pair("s6", vector<string> {"s13", "s14"}));
    link.push_back(make_pair("s7", vector<string> {"s15", "s16"}));
    link.push_back(make_pair("s8", vector<string> {"s15", "s16"}));
    link.push_back(make_pair("s9", vector<string> {"s17", "s18"}));
    link.push_back(make_pair("s10", vector<string> {"s17", "s18"}));
    link.push_back(make_pair("s11", vector<string> {"s19", "s20"}));
    link.push_back(make_pair("s12", vector<string> {"s19", "s20"}));
    link.push_back(make_pair("s13", vector<string> {"h1", "h2"}));
    link.push_back(make_pair("s14", vector<string> {"h3", "h4"}));
    link.push_back(make_pair("s15", vector<string> {"h5", "h6"}));
    link.push_back(make_pair("s16", vector<string> {"h7", "h8"}));
    link.push_back(make_pair("s17", vector<string> {"h9", "h10"}));
    link.push_back(make_pair("s18", vector<string> {"h11", "h12"}));
    link.push_back(make_pair("s19", vector<string> {"h13", "h14"}));
    link.push_back(make_pair("s20", vector<string> {"h15", "h16"}));

}

void init_fragments(vector<pair<string, vector<int>>> &arr){
    arr.push_back(make_pair("s1", vector<int> {}));
    arr.push_back(make_pair("s2", vector<int> {}));
    arr.push_back(make_pair("s3", vector<int> {}));
    arr.push_back(make_pair("s4", vector<int> {}));
    arr.push_back(make_pair("s5", vector<int> {}));
    arr.push_back(make_pair("s6", vector<int> {}));
    arr.push_back(make_pair("s7", vector<int> {}));
    arr.push_back(make_pair("s8", vector<int> {}));
    arr.push_back(make_pair("s9", vector<int> {}));
    arr.push_back(make_pair("s10", vector<int> {}));
    arr.push_back(make_pair("s11", vector<int> {}));
    arr.push_back(make_pair("s12", vector<int> {}));
    arr.push_back(make_pair("s13", vector<int> {}));
    arr.push_back(make_pair("s14", vector<int> {}));
    arr.push_back(make_pair("s15", vector<int> {}));
    arr.push_back(make_pair("s16", vector<int> {}));
    arr.push_back(make_pair("s17", vector<int> {}));
    arr.push_back(make_pair("s18", vector<int> {}));
    arr.push_back(make_pair("s19", vector<int> {}));
    arr.push_back(make_pair("s20", vector<int> {}));
}

void done_k_server(int done_num, vector<string> &done_server){
    int done_count = 0;
	while (done_count != done_num){
		int temp = rand() % server_num + 1;
		string str = "h" + to_string(temp);
		if (find(done_server.begin(), done_server.end(), str) == done_server.end()){
			done_server.push_back(str);
			done_count++;
		}
	}
}

string next_node(vector<pair<string, vector<string>>> link, string current_node, vector<string> &pass_node){
    string node;
    vector<string> next_node_vector;
    for(vector<pair<string, vector<string>>>::iterator it = link.begin(); it != link.end(); ++it){
        if((*it).first == current_node){
            next_node_vector = (*it).second;
            break;
        }
    }
    if(next_node_vector.size() == 1){
        return next_node_vector[0];
    }else{
        int index = rand() % next_node_vector.size();
        while(find(pass_node.begin(), pass_node.end(), next_node_vector[index]) != pass_node.end()){
            index = rand() % next_node_vector.size();
        }
        return next_node_vector[index];
    }
}

void aggregator(vector<pair<string, vector<int>>> &arr, vector<string> &aggregated_switch_array, string switch_id, int fragment_id, int &aggregated_sketch_num){
    for(vector<pair<string, vector<int>>>::iterator it = arr.begin(); it != arr.end(); ++it){
        //若该碎片还未收集过，则插入
        if((*it).first == switch_id && find((*it).second.begin(), (*it).second.end(), fragment_id) == (*it).second.end()){
            (*it).second.push_back(fragment_id);

			if((*it).second.size() == sketch_fragment_num){
				if(find(aggregated_switch_array.begin(), aggregated_switch_array.end(), switch_id) == aggregated_switch_array.end()){
					aggregated_switch_array.push_back(switch_id);
					aggregated_sketch_num++;
					cout<<"switch "<<switch_id<<" has been aggregated!"<<endl;
				}
			}

            break;
        }
    }
}

int main()
{
	int done_num;
	cout<<"Please input the done_server_num: ";
	cin>>done_num;

    int packet_num = 0;
    int aggregated_sketch_num = 0;

    // srand((unsigned)time(0));

    init_up_link(up_link);
    init_down_link(down_link);
    init_fragments(fragments);
    done_k_server(done_num, done_server);

    while(aggregated_sketch_num != switch_num){
        if(done_server.size() < server_num - 1 ){
            int direction_flag = 0; // 0 is up, 1 is down;
            int fragment_flag = 0; // 0 表示未携带碎片
            int fragment_id = 0;
            string fragment_switch_id;

            int send_server_index = rand() % server_num + 1;
            string send_server = "h" + to_string(send_server_index);
            while(find(done_server.begin(), done_server.end(), send_server) != done_server.end()){
                send_server_index = rand() % server_num + 1;
                send_server = "h" + to_string(send_server_index);
            }

            int count = 0; //跳数
            string current_node = send_server;
            vector<string> pass_node_array;
            pass_node_array.push_back(send_server);

            while(count <= 6){
                if(direction_flag == 0){
                    string NextNode = next_node(up_link, current_node, pass_node_array);

                    //若还未携带碎片
                    if(fragment_flag == 0){
                        int temp = rand() % random_bound;
                        //以1/random_bound概率携带碎片
                        if(temp == 0){
                            fragment_switch_id = NextNode;
                            fragment_id = rand() % sketch_fragment_num;
                            fragment_flag = 1;
                        }

                    }

                    count++;
                    //若已经上到最顶层，则下一跳强制向下
                    if(count >= 3){
                        direction_flag = 1;
                    }else{
                        //下一跳以50%的概率向下
                        if(rand() % 10 <= 4){
                            direction_flag = 1;
                        }
                    }

                    current_node = NextNode;

                }else{
                    string NextNode = next_node(down_link, current_node, pass_node_array);
                    //若还未到达服务器
                    if(find(server.begin(), server.end(), NextNode) == server.end()){
                        //若还未携带碎片
                        if(fragment_flag == 0){
                            int temp = rand() % random_bound;
                            //以1/random_bound概率携带碎片
                            if(temp == 0){
                                fragment_switch_id = NextNode;
                                fragment_id = rand() % sketch_fragment_num;
                                fragment_flag = 1;
                            }

                        }
                    }else{
                        //若到达的服务器未done掉且携带了碎片，则收集碎片
                        if(find(done_server.begin(), done_server.end(), NextNode) == done_server.end() && fragment_flag == 1){
                            aggregator(fragments, aggregated_switch_array, fragment_switch_id, fragment_id, aggregated_sketch_num);
                        }
                        break;
                    }

                    count++;
                    current_node = NextNode;
                }
            }
        }else{
            cout<<"Input error! Done_server_num must be smaller than server_num - 1!"<<endl;
            break;
        }

        packet_num++;
    }

    cout<<"It has sent "<<packet_num<<" packets!"<<endl;

    return 0;
}
