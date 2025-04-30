#include <cstdio>

#include "Log.h"

class C{
public:
    C(){
        DOLT(this);
        DOLTV(this, "abc");
    }
    ~C(){
        DOLT(this);
        DOLTV(this, "abc");
        DOLTV(this, "abc");
        DOLTV(this, "abc");
    }

    int x;
};

int main(){
    C c;
    c.x = 1234;

    printf("qwea\n");

    I("12345");
    auto s = SAPF("1234");
    I("12345");

    D(SAPF("%d", c.x));
    D("%d", c.x);

    R("raw text1");
    R("\r""raw text2");
    R("\n");
}
