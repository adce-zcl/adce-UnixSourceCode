#include <iostream>
#include <memory>
#include <list>
using namespace std;

int main()
{
    list<int> li;
    li.push_back(1);
    li.push_back(2);
    li.push_back(3);
    cout << li.size() << endl;
    list<int> t;
    t.splice(t.begin(), li, li.begin(), li.end());
    cout << li.size() << endl;
    cout << t.size() << endl;
    exit(0);
}