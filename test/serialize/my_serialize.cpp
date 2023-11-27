#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <fcntl.h>
#include <ostream>
#include <unistd.h>

using namespace std;

typedef map<int, int> myMap;

void dumps(myMap m)
{
    // int m_size = m.size();
    // ofstream out_stream("test.bin", ios::binary);
    // out_stream.write((char *)&m_size, sizeof(int));
    // for (const auto &i : m)
    // {
    //     out_stream.write((char *)&i.first, sizeof(int));
    //     out_stream.write((char *)&i.second, sizeof(int));
    // }
    // out_stream.close();

    int m_size = m.size();
    int buffer_size = m_size * 2 + 1;
    int *buffer = (int *)malloc(sizeof(int) * buffer_size);
    int *buffer_ptr = buffer;
    *buffer_ptr = m_size;
    buffer_ptr = buffer_ptr + 1;
    for (const auto iter : m)
    {
        *buffer_ptr = iter.first;
        // cout << *buffer_ptr;
        buffer_ptr = buffer_ptr + 1;
        *buffer_ptr = iter.second;
        // cout << *buffer_ptr;
        buffer_ptr = buffer_ptr + 1;
    }
    FILE *f = fopen("test.txt", "w");
    size_t res = fwrite(buffer, sizeof(int), buffer_size, f);
    // cout << "write res: " << res << endl;
    fclose(f);

    free(buffer);
}

void loads()
{
    // ifstream in_stream("test.bin", ios::binary);
    // myMap m = {};
    // int m_size;
    // in_stream.read((char *)&m_size, sizeof(int));
    // for (int i = 0; i < m_size; i++)
    // {
    //     int k, v;
    //     in_stream.read((char *)&k, sizeof(int));
    //     in_stream.read((char *)&v, sizeof(int));
    //     m.emplace(k, v);
    // }
    // in_stream.close();

    FILE *f = fopen("test.txt", "r");
    int m_size;
    fread(&m_size, sizeof(int), 1, f);
    int buffer_size = m_size * 2;
    int *buffer = (int *)malloc(sizeof(int) * buffer_size);
    int *buffer_ptr = buffer;
    fread(buffer_ptr, sizeof(int), buffer_size, f);
    myMap m = {};
    for (int i = 0; i < buffer_size; i = i + 2)
    {
        int k, v;
        k = *buffer_ptr;
        buffer_ptr = buffer_ptr + 1;
        v = *buffer_ptr;
        buffer_ptr = buffer_ptr + 1;
        m.emplace(k, v);
    }
    fclose(f);
    free(buffer);

    // auto iter = m.end();
    // iter--;
    // cout << iter->first << endl;
    // cout << iter->second << endl;
}

double per_cost(clock_t s, clock_t e, int nums)
{
    return (double)(e - s) / CLOCKS_PER_SEC / nums;
}

void loop_test(myMap m_map, int loop)
{
    // cout << "-----------my dumps-----------\n";
    clock_t dumps_start = clock();
    for (int i = 0; i < loop; i++)
    {
        dumps(m_map);
    }
    clock_t dumps_end = clock();
    cout << "dumps: " << per_cost(dumps_start, dumps_end, loop) << "s" << endl;

    // cout << "-----------my loads-----------\n";
    clock_t loads_start = clock();
    for (int i = 0; i < loop; i++)
    {
        loads();
    }
    clock_t loads_end = clock();
    cout << "loads: " << per_cost(loads_start, loads_end, loop) << "s" << endl;
}

int main()
{
    cout << "self..." << endl;
    myMap my_map = {};
    int loop = 1000;
    for (int d = 0; d < 10; d++)
    {
        int i = d * 2000;
        for (int j = 0; j < 2000; j++)
        {
            my_map.emplace(i, i);
            i += 1;
        }
        cout << "map_size: " << my_map.size() << " loop num: " << loop << endl;
        loop_test(my_map, loop);
    }

    return 0;
}