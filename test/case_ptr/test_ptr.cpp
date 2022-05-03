#include "test_ptr.h"
#include "hash_map.h"
#include "iostream"
using namespace std;

namespace testptr
{

    bool func(map_type& map)
    {
        cout << "----------------------start case testptr-------------------------" << endl;
        std::vector<std::pair<int*, int>> vec;
        UserKeyType t;
        int total = 1000;
        for (int i = 0; i < total; ++i)
        {
            auto p = new int(i);
            vec.push_back({p, i});
            t.key = p;
            map.insert(t, i);
        }

        for (int i = 0; i < 1; ++i)
        {
            t.key = vec[i].first;
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
                map.insert_or_assign(t, i);
            }
        }

        for (auto i = 0; i < map.bucket_count(); ++i)
        {
            cout << "bucket " << i << " has " << map.bucket_size(i) << " elements" << endl;
        }

        //++itr
        for(auto itr = map.begin(); itr != map.end(); ++itr)
        {
            if (*itr->first != itr->second)
            {
                cout << "Error" << endl;
                return false;
            }
        }

        //itr++
        for(auto itr = map.begin(); itr != map.end(); itr++)
        {
            if (*itr->first != itr->second)
            {
                cout << "Error" << endl;
                return false;
            }
        }

        for (int i = 0; i < total; ++i)
        {
            t.key = vec[i].first;
            auto itr = map.find(t);
            if (itr != map.end())
            {
                if (*itr->first != itr->second)
                {
                    cout << "Error" << endl;
                    return false;
                }
            }
        }

        for (int i = 0; i < total; ++i)
        {
            t.key = vec[i].first;
            auto itr = map.find(t);
            if (itr != map.end())
            {
                auto& pair = *itr;
                if (*pair.first != pair.second)
                {
                    cout << "Error" << endl;
                    return false;
                }
            }
        }

        t.key = (int*)0xf;
        auto itr = map.find(t);
        if (itr != map.end())
        {
            cout << "Error" << endl;
            return false;
        }
        cout << "----------------------end case testptr---------------------------" << endl;
        return true;
    }
}
