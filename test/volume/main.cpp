#include "volume/id_mapper.h"
#include "sstream"

using namespace std;

class test {
public:
    explicit test(int w);
    test();
    int num;
};

test::test(int w) {
    num = w;
}

test::test() {
    num=-1;
};

int main() {
    IdMapper<test,int> what;
    uint32_t index;
    index = what.emplace_item(1);
    printf("%d\n", index);
    index = what.emplace_item(2);
    printf("%d\n", index);
    index = what.emplace_item(3);
    printf("%d\n", index);
    index = what.emplace_item(4);
    printf("%d\n", index);
    index = what.emplace_item(5);
    what.delete_item(1);
    printf("%d\n", index);
    index = what.emplace_item(9);
    test *get = what.search_item(5);
    printf("%d\n",get->num);

    return 0;
}


