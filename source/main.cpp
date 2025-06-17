#include <cstdio>

#include "Log/Log.h"

class C{
public:
    C()
    {TRM;
        DOLT;
        DOLTV("");
        DOLTV("", this);
        DOLT_F;
        DOLTV_F("");
        DOLTV_F("", this);
    }
    C(int a, std::string b)
    {TRM;
        DOLT;
        DOLTV(SAPF("a(int): %d, b(std::string) %s", a, b.c_str()));
        DOLTV(SAPF("a(int): %d, b(std::string) %s", a, b.c_str()), this);
        DOLT_F;
        DOLTV_F(SAPF("a(int): %d, b(std::string) %s", a, b.c_str()));
        DOLTV_F(SAPF("a(int): %d, b(std::string) %s", a, b.c_str()), this);
    }
    ~C()
    {TRM;
        DOLT;
        DOLTV("");
        DOLTV("", this);
        DOLT_F;
        DOLTV_F("");
        DOLTV_F("", this);
    }

    void mf1()
    {TRM;

    }

    void mf2()
    {TRM;
        this->mf1();
    }

    int x;

};

void f1(int a)
{TRF;

}

void f2(int b)
{TRF;
    f1(0);
}

void f3(int a, int b)
{TRF;

}

void f4(int a, int b, int c)
{TRF;
    f1(a);
    f2(b);
    f3(a,b);
}

void f5(int a, int b, int c, int d)
{TRF;
    f1(a);
    f2(b);
    f3(a,b);
    f4(a,b,c);
}

int main()
{TRF;

    C c, c2(1,"2");
    c.x = 1234;
    c.mf1();
    c2.mf2();

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
