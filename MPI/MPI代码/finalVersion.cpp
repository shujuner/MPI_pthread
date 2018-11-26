#include <iostream>
#include <list>
#include <map>
#include <fstream>
#include <cstdlib>
#include <vector>
#include "mpi.h"

#define DEBUG

using namespace std;

class Piece{
	public:
		Piece(int x,int y,int dirc,int speed){
			this->x=x;
			this->y=y;
			this->dirc=dirc;
			this->speed=speed;
		}
		int x;
		int y;
		int dirc;
		int speed;
};

list<Piece> pieces;
list<Piece> share_memory;
map<int,vector<list<Piece>::iterator> > state;

void run(Piece &piece){
	for(int i=0;i<piece.speed;++i){
		switch(piece.dirc){
			case 1:
				if(piece.y==900){
					if(piece.x==1600){
						--piece.x;
						piece.dirc=3;
					}
					else{
						--piece.y;
						++piece.x;
						piece.dirc=8;
					}
				}
				else{
					++piece.y;
				}
				break;
			case 2:
				if(piece.y==1){
					if(piece.x==1){
						++piece.x;
						piece.dirc=4;
					}
					else{
						++piece.y;
						--piece.x;
						piece.dirc=5;
					}
				}
				else{
					--piece.y;
				}
				break;
			case 3:
				if(piece.x==1){
					if(piece.y==900){
						--piece.y;
						piece.dirc=2;
					}
					else{
						++piece.x;
						++piece.y;
						piece.dirc=7;
					}
				}
				else{
					--piece.x;
				}
				break;
			case 4:
				if(piece.x==1600){
					if(piece.y==1){
						++piece.y;
						piece.dirc=1;
					}
					else{
						--piece.x;
						--piece.y;
						piece.dirc=6;
					}
				}
				else{
					++piece.x;
				}
				break;
			case 5:
				if(piece.x==1||piece.y==900){
					++piece.x;
					piece.dirc=4;
				}
				else{
					--piece.x;
					++piece.y;
				}
				break;
			case 6:
				if(piece.x==1||piece.y==1){
					++piece.y;
					piece.dirc=1;
				}
				else{
					--piece.x;
					--piece.y;
				}
				break;
			case 7:
				if(piece.x==1600||piece.y==900){
					--piece.y;
					piece.dirc=2;
				}
				else{
					++piece.x;
					++piece.y;
				}
				break;
			case 8:
				if(piece.x==1600||piece.y==1){
					--piece.x;
					piece.dirc=3;
				}
				else{
					++piece.x;
					--piece.y;
				}
				break;
			default:
				break;
		}
	}
}
int getPos(int x,int y,int nodeNum)
{
	int postion=y*1600+x;
	return (postion*nodeNum)/(1600*900)+1;
}
int main(int argc, char* argv[])
{
	int tmpx,tmpy,tmpdirc,tmps;
	int T,t=0;
	int numprocs,taskId;
	string str;
	MPI_Status status;
	MPI_Request *req;
	map<string,int> m;
	m["U"]=1;m["D"]=2;m["L"]=3;m["R"]=4;m["LU"]=5;m["LD"]=6;m["RU"]=7; m["RD"]=8;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&taskId);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	int master=0;
	int nodeNum=numprocs-1;
	if(taskId == master){
		int piece[4];
		int firstSend[10]={1,1,1,1,1,1,1,1,1,1};
		ifstream input("input.txt");
		//发送迭代信息 信号标记为0
		input >> T;
		for(int dest=1;dest<=nodeNum;dest++){
			MPI_Send(&T,1,MPI_INT,dest,0,MPI_COMM_WORLD);
		}
		//发送初始Pieces信息列表，每个进程有自己的信号标记，从1开始计数
		while(input >> tmpx){
			input >> tmpy >> str >> tmps;
			piece[0]=tmpx;
			piece[1]=tmpy;
			piece[2]=m[str];
			piece[3]=tmps;
			int dest=getPos(tmpx,tmpy,nodeNum);
			MPI_Rsend(piece,4,MPI_INT,dest,firstSend[dest],MPI_COMM_WORLD);	
			firstSend[dest]++;
		}
		input.close();
		//发送第一次信息的结束标记
		piece={0,0,0,0};
		for(int dest=1;dest<=nodeNum;dest++)
		{	
			MPI_Send(piece,4,MPI_INT,dest,firstSend[dest],MPI_COMM_WORLD);
		}

		//接收共享内存内容并reAllocate
		while(t<T)
		{
			t++;
			int shareTeam[4]={1,0,0,0};
			int p1[10]={200,200,200,200,200,200,200,200,200,200};
			//int rShareFlag=100;
			int shareSize[10]={0,0,0,0,0,0,0,0,0,0};
			for(int source=1;source<=nodeNum;source++){
				int rShareFlag=100;
				while(shareTeam[0]!=0){
					//接收来自node结点的共享变量没问题
					MPI_Recv(shareTeam,4,MPI_INT,source,rShareFlag,MPI_COMM_WORLD,&status);
					rShareFlag++;
					shareSize[source]++;
					//将接收到的信息返回node结点
					if(shareTeam[0]!=0)
					{
						int dest=getPos(shareTeam[0],shareTeam[1],nodeNum);
			            MPI_Send(shareTeam,4,MPI_INT,dest,p1[dest],MPI_COMM_WORLD);
			            p1[dest]++;
					}
				}
				shareTeam[0]=1;
			}
			
			shareTeam[0]=0;
			for(int dest=1;dest<=nodeNum;dest++)
			{			
				MPI_Send(shareTeam,4,MPI_INT,dest,p1[dest],MPI_COMM_WORLD);
			}
			
			//
			int temp=0,alive_team=0;
			for(int i=1; i<=nodeNum; i++){
				MPI_Recv(&temp,1,MPI_INT,i,67,MPI_COMM_WORLD,&status);
				alive_team += temp;
			}
			cout << "After " << t <<"s " << alive_team<<endl;
		}
		

	}
	else
	{
		//接收第一次来自于文件的数据
		MPI_Recv(&T,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);		
		int piece[4]={1,0,0,0},x=2;
		while(piece[0]!=0)
		{
            MPI_Recv(piece,4,MPI_INT,master,x,MPI_COMM_WORLD,&status);
			x++;
			if(piece[0]!=0){
				pieces.push_back(Piece(piece[0],piece[1],piece[2],piece[3]));
			}
		}
		cout<<pieces.size()<<endl;

		//将不属于本进程处理的数据发送至master重新分配,信号从100开始
		while(t<T){
			t++;
			state.clear();
			share_memory.clear();
			
			for(list<Piece>::iterator it=pieces.begin(); it!=pieces.end();){
				run(*it);
				if(getPos(it->x,it->y,nodeNum) != taskId){
					share_memory.push_back(*it);
					list<Piece>::iterator erase_it=it++;
					pieces.erase(erase_it);
				}else{
					it++;
				}
			}

			int share_team[4];
			int shareFlag=100;
			for(list<Piece>::iterator it=share_memory.begin(); it!=share_memory.end(); it++)
			{
				share_team[0]=it->x;
				share_team[1]=it->y;
				share_team[2]=it->dirc;
				share_team[3]=it->speed;
				MPI_Rsend(share_team,4,MPI_INT,master,shareFlag,MPI_COMM_WORLD);
				shareFlag++;
			}
			
			share_team[0]=0;
			MPI_Rsend(share_team,4,MPI_INT,master,shareFlag,MPI_COMM_WORLD);	

			//接收来自master重新分配的小队信息，信息量从200开始
			
			int team2s[4]={1,0,0,0},j=200;
			while(team2s[0]!=0){
				MPI_Recv(team2s,4,MPI_INT,master,j,MPI_COMM_WORLD,&status);
				if(team2s[0]!=0){
					pieces.push_back(Piece(team2s[0],team2s[1],team2s[2],team2s[3]));
				}
				j++;
			}
			
			//所有属于本进程的数据已经完全获得

			
			for(list<Piece>::iterator it=pieces.begin(); it!=pieces.end(); ++it){
				if(state.find(it->y*1600+it->x)==state.end()){
					vector<list<Piece>::iterator> tmpvec;
					tmpvec.push_back(it);
					state.insert(make_pair(it->y*1600+it->x,tmpvec));
				}else{
					state[it->y*1600+it->x].push_back(it);
				}
			}
			
			for(map<int,vector<list<Piece>::iterator> >::iterator iter=state.begin(); iter!=state.end(); ++iter){
				if(iter->second.size() >= 2){
					vector<list<Piece>::iterator>::iterator it=iter->second.begin();
					int mins=(*it)->speed;
					int mind = (*it)->dirc;
					bool alive = true;
					++it;
					for(;it!=iter->second.end();++it){
						if((*it)->speed < mins){
							mins = (*it)->speed;
							mind = (*it)->dirc;
							alive = true;
						}else if((*it)->speed == mins){
							alive = false;
						}else{}
					}
					it = iter->second.begin();
					if(alive){
						(*it)->speed = mins;
						(*it)->dirc = mind;
						++it;
					}
					for(;it!=iter->second.end();++it){
						pieces.erase(*it);
					}

				}
			}
			int team_pieces = pieces.size();
			MPI_Send(&team_pieces,1,MPI_INT,0,67,MPI_COMM_WORLD);
			//
			
		}
	}
	MPI_Finalize();
}