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
map<int,vector<list<Piece>::iterator>> state;

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
	int postion=(y-1)*1600+x;
	return (postion*nodeNum-1)/(1600*900)+1;
}
int main(int argc, char** argv){
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
		int firstSend[10]={1};
		ifstream input("input.txt");
		input >> T;
		for(int dest=1;dest<=nodeNum;dest++){
			MPI_Send(&T,1,MPI_INT,dest,0,MPI_COMM_WORLD);
		}
		int master0=0;
		int dest1=0;
		int dest2=0;
		while(input >> tmpx){
			input >> tmpy >> str >> tmps;
			piece[0]=tmpx;
			piece[1]=tmpy;
			piece[2]=m[str];
			piece[3]=tmps;
			int dest=getPos(tmpx,tmpy,nodeNum);
			MPI_Rsend(piece,4,MPI_INT,dest,firstSend[dest],MPI_COMM_WORLD);	
			first[dest]++;
		}
		input.close();
		piece={0,0,0,0};
		for(int dest=1;dest<=nodeNum;dest++)
		{	
			MPI_Send(piece,4,MPI_INT,dest,i2[dest],MPI_COMM_WORLD);
		}
		
		while(t<T){
			t++;
			int team[4]={1,0,0,0};
			int p1[10]={200};
			for(int source=1;source<=nodeNum;source++){
				int o=100;
				while(team[0]!=0){
					MPI_Recv(team,4,MPI_INT,source,o,MPI_COMM_WORLD,&status);
					o++;
					if(team[0]!=0)
					{
						int dest=getPos(team[0],team[1],nodeNum);
			            MPI_Send(team,4,MPI_INT,dest,p1[dest],MPI_COMM_WORLD);
			            p1[dest]++;
					}
				}
				team[0]=1;
			}
			team[0]=0;
			for(int dest=1;dest<=nodeNum;dest++)
			{			
				MPI_Send(team,4,MPI_INT,dest,p1[dest],MPI_COMM_WORLD);
			}
			cout<<"master"<<endl;
			int alive_team=0;
			for(int source=1; source<=nodeNum; source++){
				int temp=0;
				MPI_Recv(&temp,1,MPI_INT,source,67,MPI_COMM_WORLD,&status);
				alive_team += temp;
			}
			cout << "After " << t << "s " << alive_team << endl;
		}
	}
	else
	{
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
			int share_team[4],i=100;
			for(list<Piece>::iterator it=share_memory.begin(); it!=share_memory.end(); it++){
				share_team[0]=it->x;
				share_team[1]=it->y;
				share_team[2]=it->dirc;
				share_team[3]=it->speed;
				MPI_Rsend(share_team,4,MPI_INT,master,i,MPI_COMM_WORLD);
				i++;
			}
			share_team[0]=0;
			MPI_Rsend(share_team,4,MPI_INT,master,i,MPI_COMM_WORLD);	
			int team2s[4]={1,0,0,0},j=200;

			while(team2s[0]!=0){
				MPI_Recv(team2s,4,MPI_INT,master,j,MPI_COMM_WORLD,&status);
				if(team2s[0]!=0){
					pieces.push_back(Piece(team2s[0],team2s[1],team2s[2],team2s[3]));
				}
				j++;
			}
			cout<<taskId<<"node1"<<endl;
			for(list<Piece>::iterator it=pieces.begin(); it!=pieces.end(); ++it){
				if(state.find((it->y-1)*1600+it->x)==state.end()){
					vector<list<Piece>::iterator> tmpvec{it};
					state.insert(make_pair((it->y-1)*1600+it->x,tmpvec));
				}else{
					state[(it->y-1)*1600+it->x].push_back(it);
				}
			}	
			cout<<taskId<<"node2"<<endl;
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
			cout<<taskId<<"node3"<<endl;
			int team_pieces = pieces.size();
			MPI_Send(&team_pieces,1,MPI_INT,master,67,MPI_COMM_WORLD);
			cout<<taskId<<"node4"<<endl;
		}
	}
	MPI_Finalize();
	return 0;
}

