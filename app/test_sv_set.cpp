#include "ra/sv_set.hpp"
#include <iostream>
#include <string.h>
#include <cassert>

using namespace ra::container;
using namespace std;


template <class T>
void constructor_tests()
{
  cout << "Testing constructors" << endl;
}

template <class T> void do_test()
{
    constructor_tests<T>();
}

int main()
{
    do_test<int>();
    return 0;
}
