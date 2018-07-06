#include <iostream>
#include "ra/intrusive_list.hpp"
#include <vector>
#include <utility>
#include <cassert>
#include <iterator>
#include <cstddef>
#include <type_traits>


namespace ri = ra::intrusive;

struct Widget {
    Widget (int value_ ) : value(value_) {}
    int value;
    ri::list_hook hook;
};

using iterator = ri::slist_iter<Widget,&Widget::hook>;


int main()
{
    std::vector<Widget> storage ;
    storage.push_back(Widget(42));


    ri::list<Widget, &Widget::hook> values ;
    
    for (auto&& i : storage ) 
    {
        values.push_back(i);
    }

    ri::list<Widget, &Widget::hook> issame ;    
    ri::list<Widget, &Widget::hook>::iterator itr = issame.begin();
    ++itr;
    itr++;

    // assert(std::is_same_v< typename std::iterator_traits<iterator> >());
      

    Widget val = values.back();
    std::cout << val.value << std::endl;
    
    iterator iter = values.begin();
    
    
    std::cout << iter->value << std::endl;

    iterator iter2 = values.end();
    ++iter2;
    
    std::cout << iter2->value << std::endl;

    std::vector<Widget> storage2 ;
    storage2.push_back(Widget(45));    
    storage2.push_back(Widget(49));
    ri::list<Widget, &Widget::hook> values2 ;
        

    for (auto&& ij : storage2 ) 
    {
        iterator iter3 = values2.insert(values2.begin(),ij);
        
        std::cout << iter3->value << std::endl;

    }
    
    iterator iter5 = values2.begin();
    while(iter5 != values2.end())
    {   std::cout<< "here" <<std::endl;
        std::cout << iter5->value << std::endl;
        ++iter5;
    }

    // while(values2.size() != 0)
    // {
    //     iterator iter4 = values2.erase(values2.begin());
    //     val  = *(ra::util::parent_from_member<Widget, ri::list_hook>(&(*iter4), &Widget::hook));
    //     std::cout << val.value << std::endl;
    // }
    

    values.clear();
    values2.clear();

    return 0;
}