#include <cstdio>

#include "Log/Log.h"

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
    {TR

    }

    void f2()
    {TR
        this->f2();
    }

    int x;

};

void f1(int a)
{TR

}

void f2(int b)
{TR
    f1(0);
}

void f3(int a, int b)
{TR

}

void f4(int a, int b, int c)
{TR
    f1(a);
    f2(b);
    f3(a,b);
}

void f5(int a, int b, int c, int d)
{TR
    f1(a);
    f2(b);
    f3(a,b);
    f4(a,b,c);
}

int main()
{TR
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
