#pragma once
#include <cassert>
#include <Memory/ProfiledAllocator.h>

#define DeclareNonCopyable(classname) classname ( const classname& ) = delete; classname &operator=(classname &a) = delete
#define DeclareNonMoveable(classname)     classname(classname &&) = delete; classname &operator=(classname &&a) = delete

#define DeclareDefaultCopyable(classname)     classname(const classname &) = default; classname &operator=(const classname &a) = default
#define DeclareDefaultMoveable(classname)     classname(classname &&) = default; classname &operator=(classname &&a) = default


#define lyraAssert(x) assert(x)

#define DefineGlobalConstexprVariableAccessor(classname, name, value)     inline constexpr classname name()\
    { return value;}

#define DefineGlobalStaticVariableAccessor(classname, name, value)     inline classname name()\
    {   static classname ret = value;  return value;}

#define DeclareBasicIteratorsToMemberContainer(member_container) \
    auto begin() { return member_container.begin(); }\
    auto cbegin() const { return member_container.cbegin(); }\
    auto end() { return member_container.end(); }\
    auto cend() const { return member_container.cend(); }

