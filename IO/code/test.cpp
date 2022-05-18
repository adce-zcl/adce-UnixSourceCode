#include <iostream>
#include <fstream>
#define NUM 3
using namespace std;
int main()
{
    fstream f("./sysio/file/test1.txt");
    fstream temp("./sysio/file/test2.txt",ios_base::out);
    if(!f.is_open() || !temp.is_open())
    {
        perror("is_open()");
        exit(1);
    }
    int num = 1;
    string line;
    while(!f.eof())
    {
        getline(f, line);
        if(num != NUM)
        {
            temp << line << endl;
            //cout << line << endl;
        }
        num++;
    }
    f.close();
    temp.close();
    //system("pause");
    return 0;
}
