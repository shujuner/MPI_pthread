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
		Piece(int inx,int iny,int indirc,int s){
			x=inx;
			y=iny;
			dirc=indirc;
			speed=s;
		}
		int x;
		int y;
		int dirc;
		int speed;
};

list<Piece> pieces,pieces_no;
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

int isarea(int x,int y){
	if(y <= 300){
		if(x <= 320){
			return 1;
		}else if(x>320 && x<=640){
			return 1;
		}else if(x>640 && x<=1060){
			return 1;
		}else if(x>1060 && x<=1380){
			return 1;
		}else if(x>1380 && x<=1600){
			return 1;
		}
	}else if(y>300 && y<=600){
		if(x <= 320){
			return 2;
		}else if(x>320 && x<=640){
			return 2;
		}else if(x>640 && x<=1060){
			return 2;
		}else if(x>1060 && x<=1380){
			return 2;
		}else if(x>1380 && x<=1600){
			return 2;
		}
	}else if(y>600){
		if(x <= 320){
			return 3;
		}else if(x>320 && x<=640){
			return 3;
		}else if(x>640 && x<=1060){
			return 3;
		}else if(x>1060 && x<=1380){
			return 3;
		}else if(x>1380 && x<=1600){
			return 3;
		}
	}
}

int main(int argc, char* argv[]){
	int tmpx,tmpy,tmpdirc,tmps,T,t=0;	//tmpx - x ; tmpy - y ; str - fangxiang ; tmps - v ; T - time
	int numprocs,procid;
	string str;
	MPI_Status status;
	MPI_Request *req;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&procid);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	if(procid == 0){
		int piece[4],i1=2,i2=2,i3=2,i4=2,i5=2,i6=2,i7=2,i8=2,i9=2,i10=2,i11=2,i12=2,i13=2,i14=2,i15=2;
		ifstream input("input2.txt");
		input >> T;
		for(int v=1;v<=3;v++){
			MPI_Send(&T,1,MPI_INT,v,0,MPI_COMM_WORLD);
		}

		while(input >> tmpx){
			input >> tmpy >> str >> tmps;
			if(str.compare("U")==0){
				tmpdirc=1;
			}else if(str.compare("D")==0){
				tmpdirc=2;
			}else if(str.compare("L")==0){
				tmpdirc=3;
			}else if(str.compare("R")==0){
				tmpdirc=4;
			}else if(str.compare("LU")==0){
				tmpdirc=5;
			}else if(str.compare("LD")==0){
				tmpdirc=6;
			}else if(str.compare("RU")==0){
				tmpdirc=7;
			}else if(str.compare("RD")==0){
				tmpdirc=8;
			}
			piece[0]=tmpx;
			piece[1]=tmpy;
			piece[2]=tmpdirc;
			piece[3]=tmps;
			if(isarea(tmpx,tmpy) == 1){
				MPI_Rsend(piece,4,MPI_INT,1,i1,MPI_COMM_WORLD);
				i1++;
			}else if(isarea(tmpx,tmpy) == 2){
				MPI_Rsend(piece,4,MPI_INT,2,i2,MPI_COMM_WORLD);
				i2++;
			}else if(isarea(tmpx,tmpy) == 3){
				MPI_Rsend(piece,4,MPI_INT,3,i3,MPI_COMM_WORLD);
				i3++;
			} 
		}
		piece[0]=0;
		MPI_Send(piece,4,MPI_INT,1,i1,MPI_COMM_WORLD);
		MPI_Send(piece,4,MPI_INT,2,i2,MPI_COMM_WORLD);
		MPI_Send(piece,4,MPI_INT,3,i3,MPI_COMM_WORLD);
		
		input.close();
		
		while(t<T){
			t++;
			int team[4]={1,0,0,0},p1=200,p2=200,p3=200,p4=200,p5=200,p6=200,p7=200,p8=200,p9=200,p10=200,p11=200,p12=200,p13=200,p14=200,p15=200;
			for(int a=1;a<=3;a++){
				int o=100;
				while(team[0]!=0){
					MPI_Recv(team,4,MPI_INT,a,o,MPI_COMM_WORLD,&status);
					o++;
					if(team[0]!=0){
						if(isarea(team[0],team[1]) == 1){
							MPI_Send(team,4,MPI_INT,1,p1,MPI_COMM_WORLD);
							p1++;
						}else if(isarea(team[0],team[1]) == 2){
							MPI_Send(team,4,MPI_INT,2,p2,MPI_COMM_WORLD);
							p2++;
						}else if(isarea(team[0],team[1]) == 3){
							MPI_Send(team,4,MPI_INT,3,p3,MPI_COMM_WORLD);
							p3++;
						}
					}
				}
				team[0]=1;
			}
			team[0]=0;
			MPI_Send(team,4,MPI_INT,1,p1,MPI_COMM_WORLD);
			MPI_Send(team,4,MPI_INT,2,p2,MPI_COMM_WORLD);
			MPI_Send(team,4,MPI_INT,3,p3,MPI_COMM_WORLD);
			

			int temp=0,alive_team=0;
			for(int i=1; i<=3; i++){
				MPI_Recv(&temp,1,MPI_INT,i,67,MPI_COMM_WORLD,&status);
				alive_team += temp;
			}
			cout << "After " << t << "s " << alive_team << endl;
		}
	}else{
		MPI_Recv(&T,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
		
		int piece[4]={1,0,0,0},x=2;
		while(piece[0]!=0){
                	MPI_Recv(piece,4,MPI_INT,0,x,MPI_COMM_WORLD,&status);
			x++;
			if(piece[0]!=0){
				pieces.push_back(Piece(piece[0],piece[1],piece[2],piece[3]));
			}
		}
		
		while(t<T){
			t++;
			state.clear();
			pieces_no.clear();
			
			for(list<Piece>::iterator it=pieces.begin(); it!=pieces.end();){
				run(*it);
				if(isarea(it->x,it->y) != procid){
					pieces_no.push_back(*it);
					list<Piece>::iterator erase_it=it++;
					pieces.erase(erase_it);
				}else{
					it++;
				}
			}

			int team1s[4],i=100;
			for(list<Piece>::iterator it=pieces_no.begin(); it!=pieces_no.end(); it++){
				team1s[0]=it->x;
				team1s[1]=it->y;
				team1s[2]=it->dirc;
				team1s[3]=it->speed;
				MPI_Rsend(team1s,4,MPI_INT,0,i,MPI_COMM_WORLD);
				i++;
			}
			team1s[0]=0;
			MPI_Rsend(team1s,4,MPI_INT,0,i,MPI_COMM_WORLD);
			
			int team2s[4]={1,0,0,0},j=200;
			while(team2s[0]!=0){
				MPI_Recv(team2s,4,MPI_INT,0,j,MPI_COMM_WORLD,&status);
				if(team2s[0]!=0){
					pieces.push_back(Piece(team2s[0],team2s[1],team2s[2],team2s[3]));
				}
				j++;
			}

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
		}
	}
	/*
	else if(procid == 2){
		MPI_Recv(&T,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
		
		int piece2[4]={1,0,0,0},y=2;
		while(piece2[0]!=0){
			MPI_Recv(piece2,4,MPI_INT,0,y,MPI_COMM_WORLD,&status);
			y++;
			if(piece2[0]!=0){
				pieces2.push_back(Piece(piece2[0],piece2[1],piece2[2],piece2[3]));
			}
		}

		while(t<T){
			t++;
			state2.clear();
			pieces1no.clear();
			pieces2no.clear();
			for(list<Piece>::iterator it=pieces2.begin();it!=pieces2.end();){
				run(*it);
				if(it->x <= 800){
					pieces2no.push_back(*it);
					list<Piece>::iterator erase_it = it++;
					pieces2.erase(erase_it);
				}else{
					it++;
				}
			}
			int team2s[4],team1s[4]={1,0,0,0},i=100,j=20000;
			while(team1s[0]!=0){
				MPI_Recv(team1s,4,MPI_INT,1,i,MPI_COMM_WORLD,&status);
				if(team1s[0]!=0){
					pieces1no.push_back(Piece(team1s[0],team1s[1],team1s[2],team1s[3]));
					pieces2.push_back(Piece(team1s[0],team1s[1],team1s[2],team1s[3]));
				}
				i++;
			}

			for(list<Piece>::iterator it=pieces2.begin(); it!=pieces2.end(); ++it){
				if(it->x <=800){
					cout << "procid==2 teamx=" << it->x << endl;
				}
			}			
	
			for(list<Piece>::iterator it=pieces2no.begin(); it!=pieces2no.end();it++){
				team2s[0]=it->x;
				team2s[1]=it->y;
				team2s[2]=it->dirc;
				team2s[3]=it->speed;
				MPI_Send(team2s,4,MPI_INT,1,j,MPI_COMM_WORLD);
				j++;
			}
			team2s[0]=0;
			MPI_Send(team2s,4,MPI_INT,1,j,MPI_COMM_WORLD);
			
			for(list<Piece>::iterator it=pieces2.begin(); it!=pieces2.end(); ++it){
				if(state2.find(it->y*1600+it->x)==state2.end()){
					vector<list<Piece>::iterator> tmpvec;
					tmpvec.push_back(it);
					state2.insert(make_pair(it->y*1600+it->x,tmpvec));
				}else{
					state2[it->y*1600+it->x].push_back(it);
				}
			}
			cout << "after state2.size=" << state2.size() << endl;
			for(map<int,vector<list<Piece>::iterator> >::iterator iter=state2.begin(); iter!=state2.end(); ++iter){
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
					int b=0;
					for(;it!=iter->second.end();++it){
						pieces2.erase(*it);
						b++;
					}
				}
			}
			int team_pieces2 = pieces2.size();
			MPI_Send(&team_pieces2,1,MPI_INT,0,67,MPI_COMM_WORLD);
			cout << "procid==2 after erase pieces2.size= " << pieces2.size() << endl;
		}
	}*/
	MPI_Finalize();
	return 0;
}

