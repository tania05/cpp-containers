#ifndef intrusive_list_hpp
#define intrusive_list_hpp

#include <type_traits>
#include <iostream>
#include <memory>
#include <iterator>
// NOTE: THE FOLLOWING LINE IS NEW!
#include <cstddef>
#include "ra/parent_from_member.hpp"

namespace ra :: intrusive {
  // Per-node list management information class.
  // The implementation-defined type that contains per-node list
  // management information (i.e., successor and predecessor).
  // This class has the list class template as a friend.
  // This type must contain pointers (of type list_hook*) to the
  // next and previous node in the list.

class list_hook {
public:
  // The only allowable public members for this class
  // are the ones listed below.
  // The particular behavior of the following special
  // member functions are implementation defined and
  // may be defaulted if appropriate.
  list_hook (){}
  list_hook (const list_hook&) = default;
  list_hook (list_hook&&)= default;
  list_hook &operator=(const list_hook&)= default;
  list_hook &operator=( list_hook &&)= default;
  ~list_hook() = default;

  private:
  template <class T , list_hook T ::* Hook > friend class list;

  //make it a friend so we can debug
  template <class T , list_hook T ::* Hook > friend class slist_iter;
  
  list_hook* next_;
  list_hook* prev_;
};


// singly-linked list node base (for intrusive container)
// template <class T> struct slist_node_base {
//         slist_node_base(T* next_) : next(next_) {}
//         T* next; // pointer to next node in list
// };

// single-linked list iterator (const and non-const)
template <class T , list_hook T ::* Hook > class slist_iter {
public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename std::remove_const_t<T>;
        using difference_type = std::ptrdiff_t;
        using reference = T&;
        using pointer = T*;
        slist_iter(T* node = nullptr)
        {
          if(node != nullptr)
          {
            node_ = node.*Hook;
          }
        }
        slist_iter(list_hook * node = nullptr): node_(node) {}

        template <class OtherT, list_hook OtherT::*OtherHook, class = std::enable_if_t<std::is_convertible_v<OtherT *, T *>>>
          slist_iter(const slist_iter<OtherT, OtherHook>& other) : node_(other.node_) {}
        
        reference operator*() {
          T * parent = ra::util::parent_from_member<T, list_hook>(node_,Hook);
          return *parent;
        }
        
        pointer operator->() {
          T * parent = ra::util::parent_from_member<T, list_hook>(node_,Hook);
          return parent;
        }

        slist_iter& operator++() {
                node_ = node_->next_;
                return *this;
        }
        slist_iter operator++(int) {
                slist_iter old(*this);
                node_ = node_->next_;
                return old;
        }

        //decrement operations
        slist_iter& operator--() {
                node_ = node_->prev_;
                return *this;
        }
        slist_iter operator--(int) {
                slist_iter old(*this);
                node_ = node_->prev_;
                return old;
        }
        template <class OtherT, list_hook T::* OtherHook> bool operator==(const slist_iter<OtherT, OtherHook>& other)
          const {return node_ == other.node_;}
          
        template <class OtherT, list_hook T::* OtherHook> bool operator!=(const slist_iter<OtherT,OtherHook>& other)
          const {return !(*this == other);}
private:
        template <class R , list_hook R ::* HookR> friend class slist_iter;
        
        template <class R , list_hook R ::* HookR > friend class list;
        
        list_hook* node_; // pointer to list node
};

  // Intrusive doubly-linked list (with sentinel node).
  template <class T , list_hook T ::* Hook >
  class list {
  public:
  // The type of the elements in the list.
  using value_type = T;
  // The pointer-to-member associated with the list hook object.
  static constexpr list_hook T ::* hook_ptr = Hook ;
  // The type of a mutating reference to a node in the list.
  using reference = T&;
  // The type of a non-mutating reference to a node in the list.
  using const_reference = const T&;
  // The mutating (bidirectional) iterator type for the list.
  // This type must provide all of the functionality of a
  // bidirectional iterator.
  // The Boost Iterator library may be used to implement this
  // type.
  using iterator = slist_iter<T,Hook>;
  // The non-mutating (bidirectional) iterator type for the list.
  // This type must provide all of the functionality of a
  // bidirectional iterator.
  // The Boost Iterator library may be used to implement this
  // type.
  using const_iterator =  slist_iter<const T, Hook>;
  // An unsigned integral type used to represent sizes.
  using size_type = std::size_t;
  // Creates an empty list.
  // Time complexity: Constant.
  list()
  {

    size_ = 0;
  } 
  // Erases any elements from the list and then destroys the list.
  // Time complexity: Either linear or constant.
  ~list ()
  {
    clear();
    // ::operator delete(&node_);
  }
  // Move construction.
  // The elements in the source list (i.e., other) are moved from
  // the source list to the destination list (i.e., *this), preserving
  // their relative order.
  // After the move, the source list is empty.
  // Time complexity: Constant.
  list ( list && other )
  {
    // node_.next_ = other.node_.next_;
    // other.node_.next_ = nullptr;
    // node_.prev_ = other.node_.prev_;
    // other.node_.prev_ = nullptr;
    // size_ = other.size_;
    // other.size_ = 0;
    node_ = other.node_;
    size_ = other.size_;
    other.clear();
  }
  // Move assignment.
  // The elements in the source list (i.e., other) are moved from
  // the source list to the destination list (i.e., *this),
  // preserving their relative order.
  // If *this is not empty, *this is cleared before the move operation
  // is applied.
  // After the move, the source list is empty.
  // Precondition: The objects *this and other are distinct.
  // Time complexity: Linear in size of *this.
  list & operator=( list && other )
  {
     if(this != &other)
      {
        clear();
        node_ = other.node_;
        size_ = other.size_;
        other.clear();
      }
      return * this;
  }
  // Do not allow the copying of lists.
  list (const list &) = delete;
  list & operator=(const list &) = delete;
  // Swaps the elements of *this and x.
  // Swapping the elements of a list with itself has no effect.
  // Time complexity: Constant.
  void swap ( list & x )
  {
    list_hook tmp = node_;
    size_type tmp_size = size_;

    node_ = x.node_;
    size_ = x.size_;

    x.node_ = tmp;
    x.size_ = tmp_size;
  }
  // Returns the number of elements in the list.
  // Time complexity: Constant.
  size_type size () const
  {
    return size_;
  }
  // Inserts an element in the list before the element referred to
  // by the iterator pos.
  // An iterator that refers to the inserted element is returned.
  // Time complexity: Constant.
  iterator insert ( iterator pos , value_type & value )
  {

    //when empty
    if(size() ==0)
    {
      // std::cout<< "youou" << std::endl;
      (value.*hook_ptr).next_ = &node_;
      (value.*hook_ptr).prev_ = &node_;

      node_.next_ = &(value.*hook_ptr);
      node_.prev_ = &(value.*hook_ptr);
      // T val = *(ra::util::parent_from_member<T, list_hook>(node_.prev_ , Hook));
      // std::cout << val.value << std::endl;
    }
    else{
      
      (value.*hook_ptr).next_ = pos.node_;
      (value.*hook_ptr).prev_ = pos.node_->prev_;

      pos.node_->prev_->next_ = &(value.*hook_ptr);
      pos.node_->prev_ = &(value.*hook_ptr);
    }

    ++size_;
    return --pos;
  }
  // Erases the element in the list at the position specified by the
  // iterator pos.
  // An iterator that refers to element following the erased element
  // is returned if such an element exists; otherwise, end() is
  // returned.
  // Time complexity: Constant.
  iterator erase ( iterator pos )
  {
    
    if(pos == end())
    {
      return end();
    }

    pos.node_->next_->prev_ = pos.node_->prev_;
    pos.node_->prev_->next_ = pos.node_->next_;
    --size_;
    return ++pos;
  }
  
  // Inserts the element with the value x at the end of the list.
  // Time complexity: Constant.
  void push_back ( value_type & x )
  {
    if(size() ==0)
    {
      (x.*hook_ptr).next_ = &(node_);
      (x.*hook_ptr).prev_ = &(node_);

      node_.next_ = &(x.*hook_ptr);
      node_.prev_ = &(x.*hook_ptr);
      ++size_;
    }
    else
    {
      (x.*hook_ptr).next_ = &(node_);
      (x.*hook_ptr).prev_ = node_.prev_;

      node_.prev_->next_ = &(x.*hook_ptr);
      node_.prev_ = &(x.*hook_ptr);
      ++size_;      
    }
    
  }
  // Erases the last element in the list.
  // Precondition: The list is not empty.
  // Time complexity: Constant.
  void pop_back ()
  {
    node_.prev_->prev_->next_ = &(node_);
    node_.prev_ = node_.prev_->prev_;
    --size_;
  }
  // Returns a reference to the last element in the list.
  // Precondition: The list is not empty.
  // Time complexity: Constant.
  reference back ()
  {

    return *(--end());
  }

  const_reference back () const
  {
    return back();
  }
  // Erases any elements from the list, yielding an empty list.
  // Time complexity: Either linear or constant.
  void clear ()
  {
    // std::destroy(node_.next_, node_.prev_);
    node_.next_ = &node_;
    node_.prev_ = &node_;
    size_= 0;
  }
  // Returns an iterator referring to the first element in the list
  // if the list is not empty and end() otherwise.
  // Time complexity: Constant.

  iterator begin ()
  {
    if(size() != 0)
    {
      // T* parent = ra::util::parent_from_member<T, list_hook>(node_.next_ , Hook);
      iterator itr(node_.next_);
      return itr;
    }
    return end();
  }

  const_iterator begin () const
  {
    if(size() != 0)
    {
      // T* parent = ra::util::parent_from_member<T, list_hook>(node_.next_ , Hook);
      const_iterator itr(node_.next_);
      return itr;
    }
    return end();  
  }

  // Returns an iterator referring to the fictitious one-past-the-end
  // element.
  // Time complexity: Constant.
  
  
  iterator end ()
  {
    iterator itr(&node_);
    return itr;
  }


  const_iterator end () const
  {
    const_iterator itr(&node_);
    return itr;
  }
  private:
    list_hook node_;
    size_type size_;
};
}
#endif
