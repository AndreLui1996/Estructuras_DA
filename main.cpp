#include <iostream>
#include "R-Tree.h"

using namespace std;

typedef pair<float,float> par;

void conv(par S,R<2>& F)
{
    F[0]=S.first;
    F[1]=S.second;
}

int main()
{
    RTree<par,3,2> Tree(conv);
    int opt=1;
    while(opt)
    {
        cout<<"1 Insertar"<<endl<<"0 salir"<<endl;
        cin>>opt;
        if(opt)
        {
            float numx,numy;
            cout<<"Ingrese x y: ";
            cin>>numx;
            cin>>numy;
            par x(numx,numy);
            cout<<Tree.insert(x)<<endl;
            Tree.print();
            cout<<endl;
        }
    }
    return 0;
}
