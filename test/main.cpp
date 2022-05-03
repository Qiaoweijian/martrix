#include<iostream>
#include "test_int.h"
#include "test_sec.h"
#include "test_ptr.h"
#include "hash_map.h"
#include <map>
#include <array>
#include <string>

using namespace std;
using namespace martrix::hashmap;

bool case_int()
{
    using testint::map_type;
    map_type a;
    a.init(1);
    if (!testint::func(a))
    {
        return false;
    }
    map_type a2;
    a2.init(2);
    if (!testint::func(a2))
    {
        return false;
    }
    map_type a3;
    a3.init(3);
    if (!testint::func(a3))
    {
        return false;
    }
    return true;
}

bool case_sec()
{
    using testsec::map_type;
    map_type a;
    a.init(10240);
    if (!testsec::func(a))
    {
        return false;
    }
    return true;
}

bool case_ptr()
{
    using testptr::map_type;
    map_type a;
    a.init(3);
    if (!testptr::func(a))
    {
        return false;
    }
    return true;
}

int main()
{
    if (!case_int())
    {
        return -1;
    }

    if (!case_sec())
    {
        return -1;
    }

    if (!case_ptr())
    {
        return -1;
    }
    
    return 0;
}
