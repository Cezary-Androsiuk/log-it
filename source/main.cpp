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

    void f1()
    {T

    }

    void f2()
    {T
        this->f2();
    }

    int x;

};

void f1(int a)
{T

}

void f2(int b)
{T
    f1(0);
}

void f3(int a, int b)
{T

}

void f4(int a, int b, int c)
{T
    f1(a);
    f2(b);
    f3(a,b);
}

void f5(int a, int b, int c, int d)
{T
    f1(a);
    f2(b);
    f3(a,b);
    f4(a,b,c);
}

int main()
{T
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

    f5(1,2,3,4);
}
