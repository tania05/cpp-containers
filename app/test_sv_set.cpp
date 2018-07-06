#include "ra/sv_set.hpp"
#include <iostream>
#include <string.h>
#include <experimental/iterator>
#include <cassert>
#include <functional>
#include <algorithm>

using namespace ra::container;
using namespace std;


void check_int_range()
{
    //check a few elements
    int first[10] = {0,1,2,3,4,5,6,7,8,9};
    sv_set<int>::ordered_and_unique_range our;
    sv_set<int> sv1(our, first, 3);
    assert(sv1.size() == 3);
    assert(sv1.capacity() == 3);

    for(auto it = sv1.begin(); it < sv1.end(); ++it)
    {
        cout << *it << endl;
    }

    //check what happens if I put 0 the size
    sv_set<int> sv2(our, first, 0);
    assert(sv2.size() == 0);
    assert(sv2.capacity() == 0);
}

template <class T>
void constructor_tests()
{
  cout << "...Testing constructors..." << endl;
  
  //creating empty set
  sv_set<T> s1;
  assert(s1.size() == 0);
  assert(s1.capacity() == 0);
  assert(s1.capacity() == 0);
  assert(s1.begin() == nullptr);
  assert(s1.begin() == s1.end());
  
  //checking for set containing elements specified by teh range
  check_int_range();


  //move construction
  T vals[10] = {1,2,3,4,5,6,7,8,9,0};
  
  sv_set<T> sv2(typename sv_set<T>::ordered_and_unique_range(), vals, 4);
  sv_set<T> svm1(std::move(sv2));
  assert(svm1.size() == 4);
  assert(svm1.capacity() == 4);
  //move empty
  sv_set<T> svm2(std::move(s1));
  assert(svm2.size() == 0);
  assert(svm2.capacity() == 0);
  assert(svm2.capacity() == 0);
  assert(svm2.begin() == nullptr);
  assert(svm2.begin() == svm2.end());
  std::cout << "Constructor done" << std::endl;

  //move assignemnt
  sv_set<T> ma1;
  ma1 = std::move(svm1);
  assert(ma1.size() == 4);
  assert(ma1.capacity() == 4);
  assert(svm1.size() == 0);
  assert(svm1.capacity() == 0);
  assert(svm1.capacity() == 0);
  assert(svm1.begin() == nullptr);
  assert(svm1.begin() == svm2.end());

  std::cout << "Constructor done" << std::endl;  
  std::cout << *(ma1.begin()) << std::endl;  
  ma1.erase(ma1.begin());
}



template <class T> void do_test()
{
    constructor_tests<T>();
}

// std::less<T> is a functor class that provides a less-than predicate
// for two objects x and y (in that order) of type T, where x precedes y
// if x < y.

// std::greater<T> is a functor class that provides a less-than predicate
// for two objects x and y (in that order) of type T, where x precedes y
// if x > y.

bool is_even(int x)
{
        return (x % 2) == 0;
}

// A functor class that provides a less-than predicate for two objects
// x and y of type int, where x preceeds y if x would come before y
// in the sequence of integers that consists of all even integers in
// ascending order followed by all odd integers in ascending order.
struct even_then_odd {
        bool operator()(int x, int y) const {
                if (is_even(x) != is_even(y)) {
                        return is_even(x);
                } else {
                        return x < y;
                }
        }
};

int main()
{
    do_test<int>();
    // cout << "doing less" << endl;
    sv_set<int, std::less<int>> s1;
    
    // cout << "doing greater" << endl;
    sv_set<int, std::greater<int>> s2;

    // cout << "doing even then odd" << endl;
    sv_set<int, even_then_odd> s3;

    // cout << "For loop" << endl;    
    for (auto&& i : {5, 6, 7, 4, 1, 2, 3, 8}) {
            // cout << i << endl;
            s1.insert(i);
            // s2.insert(i);
            // s3.insert(i);
    }
    std::cout << "values in ascending order:\n";
    std::copy(s1.begin(), s1.end(),
        std::experimental::make_ostream_joiner(std::cout, ", "));
    std::cout << '\n';
    std::cout << "values in descending order:\n";
    std::copy(s2.begin(), s2.end(),
        std::experimental::make_ostream_joiner(std::cout, ", "));
    std::cout << '\n';
    std::cout << "even values in ascending order followed by "
        "odd values in ascending order:\n";
    std::copy(s3.begin(), s3.end(),
        std::experimental::make_ostream_joiner(std::cout, ", "));
    std::cout << '\n';
    
    return 0;
}

