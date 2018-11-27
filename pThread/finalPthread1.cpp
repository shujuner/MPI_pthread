#include <iostream>
#include <list>
#include <unordered_map>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <sys/time.h> 
#define DEBUG
#define NUM_THREADS 1
using namespace std;

enum DIRC {u,l,lu,ld,d,r,rd,ru};
pthread_mutex_t mutex;
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

list<Piece> tempPieces[NUM_THREADS];
list<Piece> share_mem;
unordered_map<int,vector<list<Piece>::iterator>> state[NUM_THREADS];

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
int getPostion(int x,int y)
{
	int postion= (y-1)*1600+x;
	return (postion*NUM_THREADS-1)/(1600*900);
}
int getPos(int x,int y)
{
	return (y-1)*1600+x;
}
void *doSplit(void *threadarg)
{

	int tid = *((int*)threadarg);
	for(auto it=tempPieces[tid].begin();it!=tempPieces[tid].end();++it)
	{
		int postion=getPos(it->x,it->y);
		if(state[tid].find(postion)==state[tid].end())
		{
			vector<list<Piece>::iterator> tmpvec{it};
			state[tid].insert(make_pair(postion,tmpvec));
		}
		else
		{
			state[tid][postion].push_back(it);
		}
	}
	unordered_map<int,vector<list<Piece>::iterator>>::iterator item=state[tid].begin();
	for(item;item!=state[tid].end();item++)
    {
		if(item->second.size()>=2)
		{
			auto it=item->second.begin();
			auto mins=(*it)->speed;
			auto mind=(*it)->dirc;
			bool alive=true;
			++it;
			for(;it!=item->second.end();++it){
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
			it=item->second.begin();
			if(alive){
				(*it)->speed=mins;
				(*it)->dirc=mind;
				++it;
			}
			for(;it!=item->second.end();++it){
				int postion=item->first;
				tempPieces[tid].erase(*it);
			}
		}
	}
}
void *runPiece(void *threadarg)
{
	int tid = *((int*)threadarg);
	for(auto it=tempPieces[tid].begin();it!=tempPieces[tid].end();)
	{
		auto temp=it;
		int start=getPostion(it->x,it->y);
		run(*it);
		int end=getPostion(it->x,it->y);
		if(start!=end)
		{
			pthread_mutex_lock (&mutex);
			share_mem.push_back(*it);
			pthread_mutex_unlock(&mutex);
			list<Piece>::iterator temp=it++;
			tempPieces[tid].erase(temp);
		}
		else
		{
			it++;
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
		int pos=getPostion(tmpx,tmpy);
		tempPieces[pos].emplace_back(tmpx,tmpy,str,tmps);
	}
	input.close();
	int index[NUM_THREADS];
	pthread_t threads[NUM_THREADS];
	pthread_t stateThreads[NUM_THREADS];
	int t=0;
	int i;
	int rc;
	while(t<T){
		++t;
		pthread_mutex_init(&mutex,NULL); 
		for(i=0;i<NUM_THREADS;++i)
		{
			index[i] = i;
			int ret=pthread_create(&threads[i],NULL,runPiece,(void *)&(index[i]));
			if(ret){
				cout<<"Error:unable to creat thread"<<ret<<endl;
				exit(-1);
			}
		}		
		for(i=0;i<NUM_THREADS;++i){
			pthread_join(threads[i],NULL);
		}
		for(auto it=share_mem.begin();it!=share_mem.end();it++)
		{
			int pos=getPostion(it->x,it->y);
			tempPieces[pos].insert(tempPieces[pos].begin(),*it);
		}
        for(i=0;i<NUM_THREADS;i++)
	    {
	    	index[i]=i;
        	rc = pthread_create(&stateThreads[i], NULL,doSplit, (void *)&(index[i]));
        	if (rc){
        		cout << "Error:unable to create thread," << rc << endl;
        		exit(-1);
        	}

	    }
	    for(i=0;i<NUM_THREADS;i++)
        {
        	pthread_join(stateThreads[i],NULL); 
        }
        int sum=0;
        for(i=0;i<NUM_THREADS;i++)
        {
        	sum+=tempPieces[i].size();
        	state[i].clear();
        }
        share_mem.clear();
		#ifdef DEBUG
			cout << "After " << t << "s " << sum<< endl;
		#endif

	}
	return 0;
}
