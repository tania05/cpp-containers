#include <iostream>
#include "ra/intrusive_list.hpp"
#include <vector>
#include <utility>

namespace ri = ra::intrusive;
using iterator = ri::slist_iter<ri::list_hook>;

struct Widget {
    Widget (int value_ ) : value(value_) {}
    int value;
    ri::list_hook hook;
};


int main()
{
    std::vector<Widget> storage ;
    storage.push_back(Widget(42));


    ri::list<Widget, &Widget::hook> values ;
    
    for (auto&& i : storage ) 
    {
        values.push_back(i);
    }

    ri::list<Widget, &Widget::hook>::const_iterator itr = values.begin();
    ++itr;
    itr++;

    Widget val = values.back();
    std::cout << val.value << std::endl;
    
    iterator iter = values.begin();
    
    val  = *(ra::util::parent_from_member<Widget, ri::list_hook>(&(*iter), &Widget::hook));
    
    std::cout << val.value << std::endl;

    iterator iter2 = values.end();
    ++iter2;
    val  = *(ra::util::parent_from_member<Widget, ri::list_hook>(&(*iter2), &Widget::hook));
    
    std::cout << val.value << std::endl;

    std::vector<Widget> storage2 ;
    storage2.push_back(Widget(45));    
    storage2.push_back(Widget(49));
    ri::list<Widget, &Widget::hook> values2 ;
        

    for (auto&& ij : storage2 ) 
    {
        iterator iter3 = values2.insert(values2.begin(),ij);
        
        val  = *(ra::util::parent_from_member<Widget, ri::list_hook>(&(*iter3), &Widget::hook));
        
        std::cout << val.value << std::endl;

    }
    
    iterator iter5 = values2.begin();
    while(iter5 != values2.end())
    {   std::cout<< "here" <<std::endl;
        val  = *(ra::util::parent_from_member<Widget, ri::list_hook>(&(*iter5), &Widget::hook));
        std::cout << val.value << std::endl;
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