#ifndef RTREE_H_INCLUDED
#define RTREE_H_INCLUDED

#include <iostream>
#include <vector>
#include <list>
#include <math.h>



using namespace std;

template<int D>
struct R
{
    vector<int> point;
    R(){for(int i=0;i<D;++i)point.push_back(0);}
    int& operator [](int p){return point[p];}
    void destroy(){point.clear();}
    R<D>& operator = (R<D> ot)
    {
        for(int i=0;i<D;++i)
            (*this)[i]=ot[i];
        return (*this);
    }
    bool operator == (R<D> ot)
    {
        for(int i=0;i<D;++i)
        {
            if((*this)[i]!=ot[i])
                return 0;
        }
        return 1;
    }
};

template<int D>
double distancia(R<D> one,R<D> two)
{
    double acum=0;;
    for(int i=0;i<D;++i)
        acum+=pow(one[i]-two[i],2);
    return sqrt(acum);
}

template<int D>
bool operator < (R<D>& one,R<D>& two)
{
    for(int i=0;i<D;++i)
    {
        if((one)[i]>(two)[i])
            return 0;
    }
    return 1;
}
template<int D>
ostream& operator << (ostream& os,R<D> X)
{
    os<<"[";
    for(int i=0;i<D;++i)
        os<<X[i]<<" ";
    os<<"]";
    return os;
}

template<int _size,int D>
struct node
{
    typedef typename list<node<_size,D>*>::iterator iterador;
    R<D> point1;
    R<D> point2;
    bool type=1;
    vector<R<D>> Pt;
    list<node<_size,D>*> N;
    node<_size,D>* _up;
    bool in(R<D>& X){for(int i=0;i<D;++i){if(((X[i]<point1[i]) or (X[i]>point2[i])))return 0;}return 1;}
    int origin(){R<D> nulo;if(!Pt.size())return -1;int minus=0;double d=distancia(nulo,Pt[minus]);for(unsigned int i=1;i<Pt.size();++i){double n=distancia(nulo,Pt[i]);if(n<d){d=n;minus=i;}}return minus;}
    iterador origin_up(){R<D> nulo;if(!N.size())return N.end();iterador minus=N.begin();double d=distancia(nulo,(*minus)->point1);iterador i=N.begin();for(++i;i!=N.end();++i){double n=distancia(nulo,(*i)->point1);if(n<d){d=n;minus=i;}}return minus;}
    void set_space()
    {
        point1=Pt[0];
        point2=Pt[0];
        for(unsigned int i=1;i<Pt.size();++i)
        {
            for(unsigned int j=0;j<D;++j)
            {
                if(Pt[i][j]<point1[j])
                    point1[j]=Pt[i][j];
                if(point2[j]<Pt[i][j])
                    point2[j]=Pt[i][j];
            }
        }
    }
    void set_space_up()
    {
        iterador i=N.begin();
        point1=(*i)->point1;
        point2=(*i)->point2;
        for(++i;i!=N.end();++i)
        {
            for(unsigned int j=0;j<D;++j)
            {
                if((*i)->point1[j]<point1[j])
                    point1[j]=(*i)->point1[j];
                if(point2[j]<(*i)->point2[j])
                    point2[j]=(*i)->point2[j];
            }
        }
    }
    void last_space()
    {
        unsigned int i=Pt.size()-1;
        for(unsigned int j=0;j<D;++j)
        {
            if(Pt[i][j]<point1[j])
                point1[j]=Pt[i][j];
            if(point2[j]<Pt[i][j])
                point2[j]=Pt[i][j];
        }
    }
    node(node<_size,D>* _u){_up=_u;}
};

template<typename T,int _size,int D>
class RTree
{
    private:
        typedef typename list<node<_size,D>*>::iterator iterador;
        void (*CV)(T p,R<D>& F);
        node<_size,D>* root=nullptr;
        bool find2(R<D>& data,node<_size,D>**& temp)
        {
            if(*temp)
            {
                if((*temp)->type)
                {
                    for(unsigned int i=0;i<(*temp)->Pt.size();++i)
                    {
                        if((*temp)->Pt[i]==data)
                            return 1;
                    }
                }
                else
                {
                    node<_size,D>** megatemp=temp;
                    node<_size,D>** realtemp=temp;
                    for(iterador i=(*temp)->N.begin();i!=(*temp)->N.end();++i)
                    {
                        temp=&(*i);
                        if((*temp)->in(data))
                        {
                            realtemp=temp;
                            if(find2(data,temp))
                                return 1;
                        }
                        temp=megatemp;
                    }
                    temp=realtemp;
                }
            }
            return 0;
        }
        void splitnode(node<_size,D>**& nd)
        {
            node<_size,D>* node1=new node<_size,D>(*nd);
            node1->type=0;
            node<_size,D>* node2=new node<_size,D>(*nd);
            node2->type=0;
            vector<iterador> pos;
            vector<pair<iterador,double>> dis;
            pos.push_back((*nd)->origin_up());
            for(iterador i=(*nd)->N.begin();i!=(*nd)->N.end();++i)
            {
                if(i!=pos[0])
                    dis.push_back(pair<iterador,double>(i,distancia((*pos[0])->point1,(*i)->point1)));
            }
            int cont=((_size+1)/2)-1;
            while(cont)
            {
                typename vector<pair<iterador,double>>::iterator minus=dis.begin();
                for(typename vector<pair<iterador,double>>::iterator iter=(++dis.begin());iter!=dis.end();++iter)
                {
                    if((*minus).second>(*iter).second)
                        minus=(iter);
                }
                pos.push_back((*minus).first);
                dis.erase(minus);
                --cont;
            }
            for(unsigned int i=0;i<pos.size();++i)
            {
                (*pos[i])->_up=node1;
                node1->N.push_back(*pos[i]);
            }
            for(unsigned int i=0;i<dis.size();++i)
            {
                (*(dis[i].first))->_up=node2;
                node2->N.push_back(*(dis[i].first));
            }
            (*nd)->N.clear();
            if(((*nd)->_up))
            {
                ((*nd)->_up)->N.push_back(node1);
                ((*nd)->_up)->N.push_back(node2);
                iterador i=((*nd)->_up)->N.begin();
                for(;i!=((*nd)->_up)->N.end();++i)
                {
                    if((*i)==(*nd))
                        break;
                }
                ((*nd)->_up)->N.erase(i);
                node<_size,D>** temp=nd;
                nd=&((*nd)->_up);
                delete(*temp);
                node1->_up=(*nd);
                node2->_up=(*nd);
                if((*nd)->N.size()==(_size+1))
                    splitnode(nd);
                return;
            }
            (*nd)->N.push_back(node1);
            (*nd)->N.push_back(node2);
        }
        void splitleaf(node<_size,D>**& nd)
        {
            (*nd)->type=0;
            node<_size,D>* node1=new node<_size,D>(*nd);
            node<_size,D>* node2=new node<_size,D>(*nd);
            vector<unsigned int> pos;
            vector<pair<unsigned int,double>> dis;
            pos.push_back((*nd)->origin());
            for(unsigned int i=0;i<(*nd)->Pt.size();++i)
            {
                if(i!=pos[0])
                    dis.push_back(pair<unsigned int,double>(i,distancia((*nd)->Pt[pos[0]],(*nd)->Pt[i])));
            }
            int cont=((_size+1)/2)-1;
            while(cont)
            {
                vector<pair<unsigned int,double>>::iterator minus=dis.begin();
                for(vector<pair<unsigned int,double>>::iterator iter=(++dis.begin());iter!=dis.end();++iter)
                {
                    if((*minus).second>(*iter).second)
                        minus=(iter);
                }
                pos.push_back((*minus).first);
                dis.erase(minus);
                --cont;
            }
            for(unsigned int i=0;i<pos.size();++i)
                node1->Pt.push_back((*nd)->Pt[pos[i]]);
            for(unsigned int i=0;i<dis.size();++i)
                node2->Pt.push_back((*nd)->Pt[dis[i].first]);
            node1->set_space();
            node2->set_space();
            (*nd)->Pt.clear();
            if((*nd)->_up)
            {
                node1->_up=((*nd)->_up);
                node2->_up=((*nd)->_up);
                ((*nd)->_up)->N.push_back(node1);
                ((*nd)->_up)->N.push_back(node2);
                iterador i=((*nd)->_up)->N.begin();
                for(;i!=((*nd)->_up)->N.end();++i)
                {
                    if((*i)==(*nd))
                        break;
                }
                ((*nd)->_up)->N.erase(i);
                node<_size,D>** temp=nd;
                nd=&((*nd)->_up);
                delete(*temp);
                if((*nd)->N.size()==(_size+1))
                    splitnode(nd);
                Redor(root);
                return;
            }
            (*nd)->N.push_back(node1);
            (*nd)->N.push_back(node2);
        }
        void choose_leaf(R<D>& data,node<_size,D>**& temp)
        {
            while(!((*temp)->type))
            {
                iterador po=(*temp)->N.begin();
                for(iterador i=(*temp)->N.begin();i!=(*temp)->N.end();++i)
                {
                    if(distancia(data,(*po)->point1)>distancia(data,(*i)->point1))
                        po=i;
                }
                temp=&(*po);
            }
        }
        void Pr(node<_size,D>*& nd,int level){if(nd){if(nd->type){for(unsigned int i=0;i<nd->Pt.size();++i)cout<<nd->Pt[i]<<" ";cout<<" | ";}else{for(iterador i=nd->N.begin();i!=nd->N.end();++i){Pr(*i,level+1);}cout<<endl<<level<<endl<<endl;}}}
        void Redor(node<_size,D>*& nd)
        {
            if(nd)
            {
                if(nd->type)
                    nd->set_space();
                else
                {
                    for(iterador i=nd->N.begin();i!=nd->N.end();++i)
                        Redor(*i);
                    nd->set_space_up();
                }
            }
        }
        
    public:
        RTree(void (*X)(T p,R<D>& F)){CV=X;root=new node<_size,D>(nullptr);}
        void print(){Pr(root,0);}
        //void allegroprint(Camera* d){allegroPr(d,root);}
        bool find(T data){R<D> Temp;CV(data,Temp);node<_size,D>** t=&root;return find2(Temp,t);}
        bool insert(T data)
        {
            R<D> Temp;
            CV(data,Temp);
            node<_size,D>** leaf=&root;
            if(find2(Temp,leaf))
                return 0;
            if(!((*leaf)->type))
                choose_leaf(Temp,leaf);
            (*leaf)->Pt.push_back(Temp);
            if((*leaf)!=root)
                (*leaf)->last_space();
            if((*leaf)->Pt.size()==(_size+1))
                splitleaf(leaf);
            Redor(root);
            return 1;
        }
};

#endif // R-TREE_H_INCLUDED
