#include<iostream>
#include<dirent.h>
using namespace std;
int main()
{
    struct dirent *d;
    DIR *dr;
    dr = opendir(".");
    if(dr!=NULL)
    {
        cout<<"List of Files & Folders:-\n";
        for(d=readdir(dr); d!=NULL; d=readdir(dr))
        {
            cout<<d->d_name<<endl;
        }
        closedir(dr);
    }
    else
        cout<<"\nError Occurred!";
    cout<<endl;
    return 0;
}