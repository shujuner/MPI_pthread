#include <iostream>
#include <list>
#include <unordered_map>
#include <fstream>
#include <cstdlib>
#include <vector>

#define DEBUG

using namespace std;

enum DIRC {u,l,lu,ld,d,r,rd,ru};

class Piece{
	public:
		Piece(int inx,int iny,string str,int s){
			x=inx;
			y=iny;
			speed=s;
			if(str.compare("L")==0){
				dirc=DIRC::l;
			}
			else if(str.compare("D")==0){
				dirc=DIRC::d;
			}
			else if(str.compare("U")==0){
				dirc=DIRC::u;
			}
			else if(str.compare("D")==0){
				dirc=DIRC::d;
			}
			else if(str.compare("LU")==0){
				dirc=DIRC::lu;
			}
			else if(str.compare("LD")==0){
				dirc=DIRC::ld;
			}
			else if(str.compare("RU")==0){
				dirc=DIRC::ru;
			}
			else if(str.compare("RD")==0){
				dirc=DIRC::rd;
			}
			else{}
		}
		int x;
		int y;
		DIRC dirc;
		int speed;
};

list<Piece> pieces;
unordered_map<int,vector<list<Piece>::iterator>> state;

void run(Piece &piece){
	for(int i=0;i<piece.speed;++i){
		switch(piece.dirc){
			case DIRC::u:
				if(piece.y==900){
					if(piece.x==1600){
						--piece.x;
						piece.dirc=DIRC::l;
					}
					else{
						--piece.y;
						++piece.x;
						piece.dirc=DIRC::rd;
					}
				}
				else{
					++piece.y;
				}
				break;
			case DIRC::d:
				if(piece.y==1){
					if(piece.x==1){
						++piece.x;
						piece.dirc=DIRC::r;
					}
					else{
						++piece.y;
						--piece.x;
						piece.dirc=DIRC::lu;
					}
				}
				else{
					--piece.y;
				}
				break;
			case DIRC::l:
				if(piece.x==1){
					if(piece.y==900){
						--piece.y;
						piece.dirc=DIRC::d;
					}
					else{
						++piece.x;
						++piece.y;
						piece.dirc=DIRC::ru;
					}
				}
				else{
					--piece.x;
				}
				break;
			case DIRC::r:
				if(piece.x==1600){
					if(piece.y==1){
						++piece.y;
						piece.dirc=DIRC::u;
					}
					else{
						--piece.x;
						--piece.y;
						piece.dirc=DIRC::ld;
					}
				}
				else{
					++piece.x;
				}
				break;
			case DIRC::lu:
				if(piece.x==1||piece.y==900){
					++piece.x;
					piece.dirc=DIRC::r;
				}
				else{
					--piece.x;
					++piece.y;
				}
				break;
			case DIRC::ld:
				if(piece.x==1||piece.y==1){
					++piece.y;
					piece.dirc=DIRC::u;
				}
				else{
					--piece.x;
					--piece.y;
				}
				break;
			case DIRC::ru:
				if(piece.x==1600||piece.y==900){
					--piece.y;
					piece.dirc=DIRC::d;
				}
				else{
					++piece.x;
					++piece.y;
				}
				break;
			case DIRC::rd:
				if(piece.x==1600||piece.y==1){
					--piece.x;
					piece.dirc=DIRC::l;
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

int main(){
	ifstream input("input.txt");
	int tmpx,tmpy,tmps,T;
	string str;
	input >> T;
	while(input >> tmpx){
		input >> tmpy >> str >> tmps;
		pieces.emplace_back(tmpx,tmpy,str,tmps);
	}
	input.close();

	int t=0;
	while(t<T){
		//Run 1s
		++t;
		state.clear();
		for(auto it=pieces.begin();it!=pieces.end();++it){
			run(*it);
			if(state.find(it->y*1600+it->x)==state.end()){
				vector<list<Piece>::iterator> tmpvec{it};
				state.insert(make_pair(it->y*1600+it->x,tmpvec));
			}
			else{
				state[it->y*1600+it->x].push_back(it);
			}
		}

		for(auto &item:state){
			if(item.second.size()>=2){
				auto it=item.second.begin();
				auto mins=(*it)->speed;
				auto mind=(*it)->dirc;
				bool alive=true;
				++it;
				for(;it!=item.second.end();++it){
					if((*it)->speed<mins){
						mins=(*it)->speed;
						mind=(*it)->dirc;
						alive=true;
					}
					else if((*it)->speed==mins){
						alive=false;
					}
					else{}
				}
				it=item.second.begin();
				if(alive){
					(*it)->speed=mins;
					(*it)->dirc=mind;
					++it;
				}
				for(;it!=item.second.end();++it){
					pieces.erase(*it);
				}
			}
		}
		#ifdef DEBUG
		cout << "After " << t << "s " << pieces.size() << endl;
		#endif
	}
	return 0;
}

