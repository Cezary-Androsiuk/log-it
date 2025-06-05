#include <cstdio>

#include "Log/Log.h"

class C{
public:
    C()
    {TR;
        DOLT;
        DOLTV("");
        DOLTV("", this);
        DOLT_F;
        DOLTV_F("");
        DOLTV_F("", this);
    }
    C(int a, std::string b)
    {TR;
        DOLT;
        DOLTV(SAPF("a(int): %d, b(std::string) %s", a, b.c_str()));
        DOLTV(SAPF("a(int): %d, b(std::string) %s", a, b.c_str()), this);
        DOLT_F;
        DOLTV_F(SAPF("a(int): %d, b(std::string) %s", a, b.c_str()));
        DOLTV_F(SAPF("a(int): %d, b(std::string) %s", a, b.c_str()), this);
    }
    ~C()
    {TR;
        DOLT;
        DOLTV("");
        DOLTV("", this);
        DOLT_F;
        DOLTV_F("");
        DOLTV_F("", this);
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
{TR;
    C c, c2(1,"2");
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
