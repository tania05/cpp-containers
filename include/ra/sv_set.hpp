#ifndef sv_set_hpp
#define sv_set_hpp

#include <iostream>
#include <memory>


namespace ra::container {

// A class representing a set of unique elements (which uses
// a sorted array).
template <class Key , class Compare = std::less<Key>>
class sv_set {
public:

  // A dummy type used to indicate that elements in a range
  // are both ordered and unique.
  struct ordered_and_unique_range {};

  // The type of the elements held by the container.
  // This is simply an alias for the template parameter Key.
  using value_type = Key ;
  using key_type = Key ;

  // The type of the function/functor used to compare two keys.
  // This is simply an alias for the template parameter Compare.
  using key_compare = Compare ;

  // An unsigned integral type used to represent sizes.
  using size_type = std::size_t;

  // The mutable (random-access) iterator type for the
  // container.
  // This type must support all of the functionality associated
  // with a random-access iterator.
  using iterator = Key *;

  // The non-mutable (random-access) const_iterator type for
  // the container.
  // This type must support all of the functionality associated
  // with a random-access iterator.
  using const_iterator = const Key *;

  // Creates an empty set (i.e., a set containing no elements)
  // with a capacity of zero (i.e., no allocated storage for
  // elements).
  sv_set () noexcept( std::is_nothrow_default_constructible_v <
  value_type >): start_(nullptr), finish_(nullptr), end_(nullptr) {}

  // Create a set containing the elements specified by the
  // range [first, first + n), where the elements in
  // the range must be both unique and ordered with
  // respect to the comparison operation embodied by the
  // key_compare type.
  // If the specified range is not both ordered and unique,
  // the behavior of this function is undefined.
  // Note:
  // The value of the first parameter is always ignored.
  // The first parameter is only present to allow for
  // future expansion (i.e., adding a constructor that does
  // not require an ordered and unique range).
  template <class InputIterator >
  sv_set ( ordered_and_unique_range , InputIterator first ,
  std::size_t n )
  {
    start_ = static_cast<Key *>(::operator new(n * sizeof(Key)));
    end_ = start_ + n;
    try
    {
      finish_ = std::uninitialized_copy_n(first, n, start_);
    } catch(...)
    {
      ::operator delete(start_);
      throw;
    }

  }
  // Move construction.
  // Creates a new set by moving from the specified set other.
  // After construction, the source set (i.e., other) is
  // guaranteed to be empty.
  sv_set ( sv_set && other ) noexcept(
  std :: is_nothrow_move_constructible_v < value_type >)
  {
    start_ = other.start_;
    other.start_  = nullptr;
    end_ = other.end_;
    other.end_ = nullptr;
    finish_ = other.finish_;
    other.finish_ = nullptr;
  }
  // Move assignment.
  // Assigns the value of the specified set other to *this
  // via a move operation.
  // After the move operation, the source set (i.e., other)
  // is guaranteed to be empty.
  // Precondition: The objects *this and other are distinct.
  sv_set & operator=( sv_set && other ) noexcept(
  std :: is_nothrow_move_assignable_v < value_type >)
  {
      if(this != &other)
      {
        clear();
        ::operator delete(start_);
        start_ = other.start_;
        other.start_ = nullptr;
        finish_ = other.finish_;
        other.finish_ = nullptr;
        end_ = other.end_;
        other.end_ = nullptr;
      }
      return * this;
  }

  // Copy construction.
  // Creates a new set by copying from the specified set other.
  sv_set (const sv_set & other )
  {
    start_ = static_cast<Key*>(::operator new(other.size() * sizeof(Key)));
    end_ = start_ + other.size();
    try
    {
      finish_ = std::uninitialized_copy(other.start_, other.finish_, start_);
    }catch(...)
    {
      ::operator delete(start_);
      throw;
    }
  }

  // Copy assignment.
  // Assigns the value of the specified set other to *this.
  sv_set &operator=(const sv_set & other )
  {
    if(this != &other)
    {
      clear();
      if(other.size() > capacity())
      {
        grow(other.size());
      }
      finish_ = std::uninitialized_copy(other.start_, other.finish_, start_);
    }
    return *this;
  }
  // Erases all elements in the container and destroys the
  // container.
  ~sv_set()
  {
    clear();
    ::operator delete(start_);
  }

  // Returns the comparison object for the container.
  key_compare key_comp () const
  {
    return comp_;
  }

  // Returns an iterator referring to the first element in the
  // set if the set is not empty and end() otherwise.
  const_iterator begin () const noexcept
  {
    if(size() != 0)
    {
      return start_;
    }
    return end();
  }

  iterator begin () noexcept
  {
    if(size() != 0)
    {
      return start_;
    }
    return end();
  }
  // Returns an iterator referring to the fictitious
  // one-past-the-end element for the set.
  const_iterator end () const noexcept
  {
    return finish_;
  }
  iterator end () noexcept
  {
    return finish_;
  }

  // Returns the number of elements in the set (i.e., the size
  // of the set).
  size_type size () const noexcept
  {
    return finish_ - start_;
  }

  // Returns the number of elements for which storage is
  // available (i.e., the capacity of the set). This value is
  // always at least as great as size().
  size_type capacity () const noexcept
  {
    return end_ - start_;
  }

  // Reserves storage in the container for at least n elements.
  // After this function has been called with a value of n, it
  // is guaranteed that no memory-allocation is needed as long
  // as the size of the set does not exceed n.
  // Calling this function has no effect if the capacity of the
  // container is already at least n.
  void reserve ( size_type n )
  {
    start_ = static_cast<Key*>(::operator new(n * sizeof(Key)));
    end_ = start_ + n;
    try 
    {
      std::uninitialized_default_construct_n(start_, n);
    }catch (...) 
    {
      ::operator delete(start_);
      throw;
    }
    finish_ = end_;
  }

  // Reduces the capacity of the container to the container size.
  // If the capacity of the container is greater than its size,
  // the capacity is reduced to the size of the container.
  // Calling this function has no effect if the capacity of the
  // container does not exceed its size.
  void shrink_to_fit ()
  {
    if(capacity() > size())
    {
        end_ = finish_; 
    }
  }

  // Inserts the element x in the set.
  // If the element x is already in the set, no insertion is
  // performed (since a set cannot contain duplicate values).
  // Return value:
  // If the element x was inserted into the set, the pair
  // returned has its second and first elements set to true
  // and an iterator referring to the inserted element,
  // respectively.
  // If the element x was not inserted (due to already
  // being in the set), the pair returned has its second
  // and first elements set to false and end(), respectively.
  std::pair<iterator,bool> insert (const key_type & x )
  {
    // check if there is enough storage to put more elements
    if(finish_ = end_)
    {
      grow(2 * capacity());
    }
    //no element found
    if(find(x) != end())
    {
      //to be sued for finding where to put the element
      iterator pos = start_;

      //while not reaching at the end of the container
      while(pos!= end())
      {
        ++pos;
      }

      //insert element at that location
      std::uninitialized_fill_n(pos, 1, x);

      ++finish_;
      
      return std::make_pair(pos,true);
    }

    //element found, return the end() with false
    return std::make_pair(end(),false);
  }

  // Erases the element referenced by pos from the container.
  // Returns an iterator referring to the element following the
  // erased one in the container if such an elements exists or
  // end() otherwise.
  iterator erase ( const_iterator pos )
  {
    iterator itr = start_;
    std::destroy_at(pos);
    
    //catch up to where the position is referrring
    while(itr != pos)
    {
      ++itr;
    }

    iterator elem_itr = itr;

    //before it reaches finish-1 (since that's the last element)
    //shift elements left, so fill void
    while(itr != end()-1)
    {
      std::uninitialized_fill_n(itr, 1, *++itr);
    }

   --finish_;

    //check if the folowing value exists
    if(elem_itr != nullptr)
    {
      return elem_itr;
    }
    return end();
  }

  // Swaps the contents of the container with the contents of the
  // container x.
  void swap ( sv_set& x) noexcept(
  std::is_nothrow_swappable_v<value_type>)
  {
    std::swap(this,x);
  }

  // Erases any elements in the container, yielding an empty
  // container.
  void clear () noexcept
  {
    std::destroy(start_, finish_);
    finish_ = start_;
  }

  // Searches the container for an element with the key k.
  // If an element is found, an iterator referencing the element
  // is returned; otherwise, end() is returned.
  iterator find (const key_type & k )
  {
    //start is the lower bound, finish -1 is the last element position
    return binarySearch(start_, finish_ - 1, k);
  }

  const_iterator find (const key_type & k) const
  {
    return find(k);
  }

private:
  Compare comp_;
  Key* start_;
  Key* finish_;
  Key* end_;
  void grow(size_type n)
  {
    Key * new_start_ = static_cast<Key*>(::operator new(n* sizeof(Key)));
    size_type old_size = size();
    try
    {
      std::uninitialized_move(start_, finish_, new_start_);
    } catch(...)
    {
      ::operator delete(new_start_);
      throw;
    }
    ::operator delete(start_);
    start_ = new_start_;
    finish_ = start_ + old_size;
    end_ = start_ + n;
  }
  
  iterator binarySearch(iterator low, iterator high, key_type & x)
  {
    if(high >= low)
    {
      iterator mid = low + (high -1)/2;

      //check if the element is found
      if(*mid == x)
      {
        return mid;
      }

      //if value is greater than mid
      if(x > *mid)
      {
        return binarySearch(mid+1, high, x);
      }

      //if value is smaller than mid
      return binarySearch(low, mid-1, x);
    }
    
    //did not find anything
    return end();
  }
};
}
#endif