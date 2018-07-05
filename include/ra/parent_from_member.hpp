#ifndef ra_util_parent_from_member_hpp
#define ra_util_parent_from_member_hpp

/*

SENG 475
University of Victoria, Victoria, Canada
Michael Adams

This file contains some nonportable code for determining a parent
object from a pointer to one of its members.

*/

namespace ra::util {

template <class Parent, class Member>
inline std::ptrdiff_t offset_from_pointer_to_member(
  const Member Parent::* ptr_to_member)
{
	const Parent * const parent = 0;
	const char * const member = static_cast<const char*>(
	  static_cast<const void*>(&(parent->*ptr_to_member)));
	return std::ptrdiff_t(member - static_cast<const char*>(
	  static_cast<const void*>(parent)));
}

template<class Parent, class Member>
inline Parent *parent_from_member(Member *member,
  const Member Parent::* ptr_to_member)
{
	return static_cast<Parent*>(static_cast<void*>(
	  static_cast<char*>(static_cast<void*>(member)) -
	  offset_from_pointer_to_member(ptr_to_member)));
}

template<class Parent, class Member>
inline const Parent *parent_from_member(const Member *member,
  const Member Parent::* ptr_to_member)
{
	return static_cast<const Parent*>(static_cast<const void*>(
	  static_cast<const char*>(static_cast<const void*>(member)) -
	  offset_from_pointer_to_member(ptr_to_member)));
}

}

#endif