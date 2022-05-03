#include <vector>
#include <iostream>
#include "hash_map.h"
#include"test_sec.h"
#include "boost/unordered_map.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

size_t hash_value(const sec_key_type& p)
{
    size_t seed = 0;
    boost::hash_combine(seed, boost::hash_value(p.sec_id));
    boost::hash_combine(seed, boost::hash_value(p.market));
    return seed;
}
namespace testsec
{
    using namespace std;
    using boost::property_tree::ptree;

    void readele(std::vector<UserKeyType>& vec)
    { 
        const char* filename = "./file/list.xml";
        boost::property_tree::ptree ptree_node;
        auto* ptree_node_p = &ptree_node;
        if (access(filename, F_OK) != 0)
        {
            cout << "File does not exist" << endl;
            return;
        }
        read_xml(filename, ptree_node);
        for(auto& val : ptree_node_p->get_child("list"))
        {
            auto sec_t = val.second.get<std::string>("SecurityID");
            auto mar = val.second.get<mar_type>("MarketID");
            sec_type sec;
            ToArray(sec_t, sec);
            UserKeyType sec_key(sec, mar);
            vec.push_back(sec_key);
        }
        return;
    }

    bool func(map_type& map)
    {
        cout << "----------------------start case testsec-------------------------" << endl;
        std::vector<UserKeyType> vec;
        readele(vec);

        for (auto& ele : vec)
        {
            map.insert(ele, ele.key.sec_id);
        }
        cout << "vec eles : " << vec.size() << endl;

        auto sum = 0;
        for (auto i = 0; i < map.bucket_count(); ++i)
        {
            sum += map.bucket_size(i);
            //cout << "bucket " << i << " has " << map.bucket_size(i) << " elements" << endl;
        }

        for(auto itr = map.begin(); itr != map.end(); itr++)
        {
            if (itr->first.sec_id != itr->second)
            {
                cout << "Error" << endl;
                return false;
            }
            //cout << ToString(itr->first.sec_id) << endl;
        }

        cout << "map eles : " << sum << endl;
        cout << "----------------------end case testsec---------------------------" << endl;
        return true;
    }
}
