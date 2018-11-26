//主节点
//(1)读取文件数据到二维的vector中
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <fstream>
#include <cstdlib>
#include <sys/time.h> 
#include "mpi.h"
using namespace std;
MPI_Status status;
int getPos(int x,int y)
{
	return (y-1)*1600+x;
}
void run(int index,vector<int> &local_x,vector<int> &local_y,vector<int> &local_speed,vector<int> &local_dirc){
	for(int i=0;i<local_speed[index];++i){
		switch(local_dirc[index]){
			case 0:
				if(local_y[index]==900){
					if(local_x[index]==1600){
						--local_x[index];
						local_dirc[index]=1;
					}
					else{
						--local_y[index];
						++local_x[index];
						local_dirc[index]=6;
					}
				}
				else{
					++local_y[index];
				}
				break;
			case 4:
				if(local_y[index]==1){
					if(local_x[index]==1){
						++local_x[index];
						local_dirc[index]=5;
					}
					else{
						++local_y[index];
						--local_x[index];
						local_dirc[index]=2;
					}
				}
				else{
					--local_y[index];
				}
				break;
			case 1:
				if(local_x[index]==1){
					if(local_y[index]==900){
						--local_y[index];
						local_dirc[index]=4;
					}
					else{
						++local_x[index];
						++local_y[index];
						local_dirc[index]=7;
					}
				}
				else{
					--local_x[index];
				}
				break;
			case 5:
				if(local_x[index]==1600){
					if(local_y[index]==1){
						++local_y[index];
						local_dirc[index]=0;
					}
					else{
						--local_x[index];
						--local_y[index];
						local_dirc[index]=3;
					}
				}
				else{
					++local_x[index];
				}
				break;
			case 2:
				if(local_x[index]==1||local_y[index]==900){
					++local_x[index];
					local_dirc[index]=5;
				}
				else{
					--local_x[index];
					++local_y[index];
				}
				break;
			case 3:
				if(local_x[index]==1||local_y[index]==1){
					++local_y[index];
					local_dirc[index]=0;
				}
				else{
					--local_x[index];
					--local_y[index];
				}
				break;
			case 7:
				if(local_x[index]==1600||local_y[index]==900){
					--local_y[index];
					local_dirc[index]=4;
				}
				else{
					++local_x[index];
					++local_y[index];
				}
				break;
			case 6:
				if(local_x[index]==1600||local_y[index]==1){
					--local_x[index];
					local_dirc[index]=1;
				}
				else{
					++local_x[index];
					--local_y[index];
				}
				break;
			default:
				break;
		}
	}	
}
int main(int argc,char **argv)
{
	int master,taskid,numtasks,numworkers,source,dest,count;
	MPI_Init(&argc,&argv);   // 初始化MPI环境
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);//标识各个MPI进程 ，告诉调用该函数进程的当前进程号
    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);//用来标识相应进程组中有多少个进程
	map<string,int> m;
    m["U"]=0;m["L"]=1;m["LU"]=2;m["LD"]=3;m["D"]=4;m["R"]=5;m["RD"]=6; m["RU"]=7;

	vector<int> v_x[10];
	vector<int> v_y[10];
	vector<int> v_speed[10];
	vector<int> v_dirc[10];

	numworkers=numtasks-1;
    master=numworkers;

    //声明local变量区域
    int local_size;
    int local_share_size;
    int share_x_temp[500001];
    int share_y_temp[500001];
    int share_speed_temp[500001];
    int share_dirc_temp[500001];
    map<int,vector<int>> state;
    vector<int> Loc_x,Loc_y,Loc_s,Loc_d;

    vector<int> f_loc_x,f_loc_y,f_loc_s,f_loc_d;
    //声明master变量区
    int share_size;
    vector<int> share_x_v[10];
	vector<int> share_y_v[10];
	vector<int> share_speed_v[10];
	vector<int> share_dirc_v[10];

	int share_x_from_master[500001];
	int share_y_from_master[500001];
	int share_s_from_master[500001];
	int share_d_from_master[500001];




    if(taskid==master)
	{
		ifstream input("input.txt");
		int tmpx,tmpy,tmps,T;
		string str;
		input >> T;
		int index[10]={0};
		while(input >> tmpx){
			input >> tmpy >> str >> tmps;
			int pos=(getPos(tmpx,tmpy)*numworkers-1)/(1600*900);			
			v_x[pos].push_back(tmpx);			
			v_y[pos].push_back(tmpy);
			v_speed[pos].push_back(tmps);
			v_dirc[pos].push_back(m[str]);
		}
		input.close();
		int t=0;
		while(t<T)
		{
		for(dest=0;dest<numworkers;dest++)
		{
			int size=v_x[dest].size();
			cout<<"1.1"<<endl;
		    int *buffer_x = new int[v_x[dest].size()];
		    if (!v_x[dest].empty())
		    {
		        memcpy(buffer_x, &v_x[dest][0], v_x[dest].size()*sizeof(int));
		    }
		    int *buffer_y = new int[v_y[dest].size()];
		    if (!v_y[dest].empty())
		    {
		        memcpy(buffer_y, &v_y[dest][0], v_y[dest].size()*sizeof(int));
		    }
		    int *buffer_speed = new int[v_speed[dest].size()];
		    if (!v_speed[dest].empty())
		    {
		        memcpy(buffer_speed, &v_speed[dest][0], v_speed[dest].size()*sizeof(int));
		    }
		    int *buffer_dirc = new int[v_dirc[dest].size()];
		    if (!v_dirc[dest].empty())
		    {
		        memcpy(buffer_dirc, &v_dirc[dest][0], v_dirc[dest].size()*sizeof(int));
		    }
		    cout<<"1.2"<<endl;
			MPI_Send(&size,1,MPI_INT,dest,0,MPI_COMM_WORLD);
			MPI_Send(&buffer_x[dest],size,MPI_INT,dest,1,MPI_COMM_WORLD);	
			MPI_Send(&buffer_y[dest],size,MPI_INT,dest,2,MPI_COMM_WORLD);
			MPI_Send(&buffer_speed[dest],size,MPI_INT,dest,3,MPI_COMM_WORLD);
			MPI_Send(&buffer_dirc[dest],size,MPI_INT,dest,4,MPI_COMM_WORLD);
			cout<<"1.3"<<endl;
		}
		cout<<"1 no preblom"<<endl;
		//获得传回的share变量并存入vector
		for(source=0;source<numworkers;source++)
		{
			int share_x[500001];
			int share_y[500001];
			int share_speed[500001];
			int share_dirc[500001];
			MPI_Recv(&share_size,1,MPI_INT,source,5,MPI_COMM_WORLD,&status);

			MPI_Recv(&share_x,share_size,MPI_INT,source,6,MPI_COMM_WORLD,&status);
			MPI_Recv(&share_y,share_size,MPI_INT,source,7,MPI_COMM_WORLD,&status);
			MPI_Recv(&share_speed,share_size,MPI_INT,source,8,MPI_COMM_WORLD,&status);
			MPI_Recv(&share_dirc,share_size,MPI_INT,source,9,MPI_COMM_WORLD,&status);
			for(int i=0;i<share_size;i++)
			{
				int pos=(getPos(share_x[i],share_y[i])*numworkers-1)/(1600*900);
				share_x_v[pos].push_back(share_x[i]);
				share_y_v[pos].push_back(share_y[i]);
				share_speed_v[pos].push_back(share_speed[i]);
				share_dirc_v[pos].push_back(share_dirc[i]);
			}
			//cout<<"source="<<source<<" "<<share_size<<endl;
		}

		//再把所有的Share值放回应该处理的进程中去
		for(dest=0;dest<numworkers;dest++)
		{
			int size=share_x_v[dest].size();
	        memcpy(share_x_from_master, &share_x_v[dest][0], size*sizeof(int));
	        memcpy(share_y_from_master, &share_y_v[dest][0], size*sizeof(int));
	        memcpy(share_s_from_master, &share_speed_v[dest][0], size*sizeof(int));
	        memcpy(share_d_from_master, &share_dirc_v[dest][0], size*sizeof(int));

			MPI_Send(&size,1,MPI_INT,dest,10,MPI_COMM_WORLD);
			MPI_Send(&share_x_from_master,size,MPI_INT,dest,11,MPI_COMM_WORLD);	
			MPI_Send(&share_y_from_master,size,MPI_INT,dest,12,MPI_COMM_WORLD);
			MPI_Send(&share_s_from_master,size,MPI_INT,dest,13,MPI_COMM_WORLD);
			MPI_Send(&share_d_from_master,size,MPI_INT,dest,14,MPI_COMM_WORLD);
		}
		for(int i=0;i<10;i++)
		{
			v_x[i].clear();
			v_y[i].clear();
			v_dirc[i].clear();
			v_speed[i].clear();
		}
		for(source=0;source<numworkers;source++)
		{
			int final_size;
			int final_x[500001];
			int final_y[500001];
			int final_speed[500001];
			int final_dirc[500001];
			MPI_Recv(&final_size,1,MPI_INT,source,15,MPI_COMM_WORLD,&status);

			MPI_Recv(&final_x,final_size,MPI_INT,source,16,MPI_COMM_WORLD,&status);
			MPI_Recv(&final_y,final_size,MPI_INT,source,17,MPI_COMM_WORLD,&status);
			MPI_Recv(&final_speed,final_size,MPI_INT,source,18,MPI_COMM_WORLD,&status);
			MPI_Recv(&final_dirc,final_size,MPI_INT,source,19,MPI_COMM_WORLD,&status);
			for(int i=0;i<final_size;i++)
			{
				int pos=(getPos(final_x[i],final_y[i])*numworkers-1)/(1600*900);
				v_x[pos].push_back(tmpx);			
				v_y[pos].push_back(tmpy);
				v_speed[pos].push_back(tmps);
				v_dirc[pos].push_back(m[str]);
			}
		}
		t++;
	}
	}
	else
	{
		int local_x_temp[500001];
	    int local_y_temp[500001];
	    int local_speed_temp[500001];
	    int local_dirc_temp[500001];
		MPI_Recv(&local_size,1,MPI_INT,master,0,MPI_COMM_WORLD,&status);
		MPI_Recv(&local_x_temp,local_size,MPI_INT,master,1,MPI_COMM_WORLD,&status);
		MPI_Recv(&local_y_temp,local_size,MPI_INT,master,2,MPI_COMM_WORLD,&status);
		MPI_Recv(&local_speed_temp,local_size,MPI_INT,master,3,MPI_COMM_WORLD,&status);
		MPI_Recv(&local_dirc_temp,local_size,MPI_INT,master,4,MPI_COMM_WORLD,&status);

		vector<int> local_x(local_x_temp,local_x_temp+local_size);
	    vector<int> local_y(local_y_temp,local_y_temp+local_size);
	    vector<int> local_speed(local_speed_temp,local_speed_temp+local_size);
	    vector<int> local_dirc(local_dirc_temp,local_dirc_temp+local_size);

	    vector<int> share_x;
	    vector<int> share_y;
	    vector<int> share_speed;
	    vector<int> share_dirc;
	    //cout<<"taskid="<<taskid<<" size="<<local_size<<endl;
	    for(int i=0;i<local_size;i++)
	    {
	    	int start=(getPos(local_x[i],local_y[i])*numworkers-1)/(1600*900);
			run(i,local_x,local_y,local_speed,local_dirc);
			int end=(getPos(local_x[i],local_y[i])*numworkers-1)/(1600*900);
			if(start!=end)
			{
				share_x.push_back(local_x[i]);
				share_y.push_back(local_y[i]);
				share_speed.push_back(local_speed[i]);
				share_dirc.push_back(local_dirc[i]);
				//delete origin data
			}
			else
			{
				Loc_x.push_back(local_x[i]);
				Loc_y.push_back(local_y[i]);
				Loc_s.push_back(local_speed[i]);
				Loc_d.push_back(local_dirc[i]);
			}
	    }
		//将共享变量返回至master结点重新分配进程
		int count=share_x.size();
        memcpy(share_x_temp,&share_x[0],count*sizeof(int));
        memcpy(share_y_temp, &share_y[0], count*sizeof(int));
        memcpy(share_speed_temp, &share_speed[0],count*sizeof(int));
        memcpy(share_dirc_temp, &share_dirc[0], count*sizeof(int));

	    MPI_Send(&count,1,MPI_INT,master,5,MPI_COMM_WORLD);
	    MPI_Send(&share_x_temp,count,MPI_INT,master,6,MPI_COMM_WORLD);
	    MPI_Send(&share_y_temp,count,MPI_INT,master,7,MPI_COMM_WORLD);
	    MPI_Send(&share_speed_temp,count,MPI_INT,master,8,MPI_COMM_WORLD);
	    MPI_Send(&share_dirc_temp,count,MPI_INT,master,9,MPI_COMM_WORLD);

	    int local_share_x_temp[500001];
	    int local_share_y_temp[500001];
	    int local_share_speed_temp[500001];
	    int local_share_dirc_temp[500001];
	    MPI_Recv(&local_share_size,1,MPI_INT,master,10,MPI_COMM_WORLD,&status);
		MPI_Recv(&local_share_x_temp,local_size,MPI_INT,master,11,MPI_COMM_WORLD,&status);
		MPI_Recv(&local_share_y_temp,local_size,MPI_INT,master,12,MPI_COMM_WORLD,&status);
		MPI_Recv(&local_share_speed_temp,local_size,MPI_INT,master,13,MPI_COMM_WORLD,&status);
		MPI_Recv(&local_share_dirc_temp,local_size,MPI_INT,master,14,MPI_COMM_WORLD,&status);
		//cout<<"taskid="<<taskid<<" before localsize="<<local_x.size()<<endl;
		for(int i=0;i<local_share_size;i++)
		{
			Loc_x.push_back(local_share_x_temp[i]);
			Loc_y.push_back(local_share_y_temp[i]);
			Loc_s.push_back(local_share_speed_temp[i]);
			Loc_d.push_back(local_share_dirc_temp[i]);
		}
		//cout<<"taskid="<<taskid<<" after localsize="<<local_x.size()<<endl;

		//开始删除数据,所有的数据都存在local_中
		
		int *index=new int[Loc_x.size()];
		int pre=0;
		for(int i=0;i<Loc_x.size();i++){
			int postion=Loc_x[i]+(Loc_y[i]-1)*1600;
			if(state.find(postion)==state.end()){
				vector<int> tmpvec{i};
				state.insert(make_pair(postion,tmpvec));
			}
			else{
				state[postion].push_back(i);
			}
			index[pre++]=postion;
		}
		for(int i=0;i<pre;i++)
        {
        	if(state[index[i]].size()>=2)
        	{
        		int minSpeed,minIndex;
	        	int secondSpeed,secondIndex;
	        	minSpeed=local_speed[state[index[i]][0]];
	        	minIndex=0;
	        	secondSpeed=local_speed[state[index[i]][0]];
	        	secondIndex=0;
	        	
	        	for(int j=1;j<state[index[i]].size();j++)
	        	{
	        		if(local_speed[state[index[i]][j]]<minSpeed)
	        		{
	        			minIndex=j;
	        		}

	        	}	
	        	for(int j=1;j<state[index[i]].size();j++)
	        	{
	        		if(j!=minIndex)
	        		{
	        			if(local_speed[state[index[i]][j]]<secondSpeed)
		        		{
		        			secondIndex=j;
		        		}
	        		}
	        	}
	        	
	        	if(local_speed[state[index[i]][minIndex]]!=local_speed[state[index[i]][secondIndex]])
	        	{
	        		f_loc_x.push_back(Loc_x[state[index[i]][minIndex]]);
	        		f_loc_y.push_back(Loc_y[state[index[i]][minIndex]]);
	        		f_loc_s.push_back(Loc_s[state[index[i]][minIndex]]);
	        		f_loc_d.push_back(Loc_d[state[index[i]][minIndex]]);
	        	}

        	}
        	else
        	{
        		f_loc_x.push_back(Loc_x[state[index[i]][0]]);
	        	f_loc_y.push_back(Loc_y[state[index[i]][0]]);
	        	f_loc_s.push_back(Loc_s[state[index[i]][0]]);
	        	f_loc_d.push_back(Loc_d[state[index[i]][0]]);
        	}
        	
			
		}
		cout<<"taskid= "<<taskid<<" finalSize="<<f_loc_x.size()<<endl;

		int final_x_temp[500001];
    	int final_y_temp[500001];
    	int final_speed_temp[500001];
    	int final_dirc_temp[500001];
		int final_count=f_loc_x.size();
        memcpy(final_x_temp,&f_loc_x[0],final_count*sizeof(int));
        memcpy(final_y_temp, &f_loc_y[0], final_count*sizeof(int));
        memcpy(final_speed_temp, &f_loc_s[0],final_count*sizeof(int));
        memcpy(final_dirc_temp, &f_loc_d[0], final_count*sizeof(int));

	    MPI_Send(&final_count,1,MPI_INT,master,15,MPI_COMM_WORLD);
	    MPI_Send(&final_x_temp,final_count,MPI_INT,master,16,MPI_COMM_WORLD);
	    MPI_Send(&final_y_temp,final_count,MPI_INT,master,17,MPI_COMM_WORLD);
	    MPI_Send(&final_speed_temp,final_count,MPI_INT,master,18,MPI_COMM_WORLD);
	    MPI_Send(&final_dirc_temp,final_count,MPI_INT,master,19,MPI_COMM_WORLD);


	}
	MPI_Finalize();

}