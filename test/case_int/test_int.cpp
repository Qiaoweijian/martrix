#include "test_int.h"
#include "hash_map.h"
#include "iostream"
using namespace std;

namespace testint
{

    bool func(map_type& map)
    {
        cout << "----------------------start case testint-------------------------" << endl;
        int total = 1000;
        UserKeyType t;
        t.key = 1;
        map.insert(t, 1);
        auto itr = map.begin();
        decltype(itr)* itr1 = new decltype(itr)();

        for (int i = 0; i < total; ++i)
        {
            t.key = i;
            map.insert(t, i);
            map.bucket_size(0);
        }

        for (int i = 0; i < 1; ++i)
        {
            t.key = i;
            if (!map.insert(t, i))
            {
                cout << "insert int failed, key-value [" << t.key << "-" << i << "]" << endl;
            }
                
            if (map.insert_or_assign(t, 100))
            {
                cout << "insert_or_assign int success. key[" << t.key << "]" << endl;
                if (map.find(t)->second == 100)
                {
                    cout << "insert_or_assign assert success" << endl;
                }
                // rollback
                map.insert_or_assign(t, t.key);
            }
        }

        for (auto i = 0; i < map.bucket_count(); ++i)
        {
            cout << "bucket " << i << " has " << map.bucket_size(i) << " elements" << endl;
        }

        //++itr
        for(auto itr = map.begin(); itr != map.end(); ++itr)
        {
            if (itr->first != itr->second)
            {
                cout << "Error" << endl;
                return false;
            }
        }

        //itr++
        for(auto itr = map.begin(); itr != map.end(); itr++)
        {
            if (itr->first != itr->second)
            {
                cout << "Error" << endl;
                return false;
            }
        }

        for (int i = 0; i < total; ++i)
        {
            t.key = i;
            auto itr = map.find(t);
            if (itr != map.end())
            {
                if (itr->first != itr->second)
                {
                    cout << "Error" << endl;
                    return false;
                }
            }
        }

        for (int i = 0; i < total; ++i)
        {
            t.key = i;
            auto itr = map.find(t);
            if (itr != map.end())
            {
                auto& pair = *itr;
                if (pair.first != pair.second)
                {
                    cout << "Error" << endl;
                    return false;
                }
            }
        }

        t.key += 100;
        itr = map.find(t);
        if (itr != map.end())
        {
            cout << "Error" << endl;
            return false;
        }
        cout << "----------------------end case testint---------------------------" << endl;
        return true;
    }
}
