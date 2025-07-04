#include <cstdio>

#include "Log/Log.h"
#include "SecondFileClass.h"

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

    int constReturnMethod() const
    {TRM;
        return this->x;
    }

    int x;

};

class C2
{
public:
    C2(const std::vector<C *> &vec)
    {TRM; DOLTV(
            [&](){
                std::string t = "[";
                for(C *c : vec) t += SAPF("%p, ", c);
                if(!vec.empty()) t.resize(t.size() - 2);
                return t + "]";
            }().c_str()
        )

    }
};

void f1(int a)
{TRFV("%d", a);

}

void f2(int b)
{TRFV("%d", b);
    f1(0);
}

void f3(int a, int b)
{TRFV("%d, %d", a, b);

}

void f4(int a, int b, int c)
{TRFV("%d, %d, %d", a, b, c);
    f1(a);
    f2(b);
    f3(a,b);
}

void f5(int a, int b, int c, int d)
{TRFV("%d, %d, %d", a, b, c);
    f1(a);
    f2(b);
    f3(a,b);
    f4(a,b,c);
}


std::string print_binary(size_t num)
{TRFV("%llu", num);
    std::string str;
    for (int i = (sizeof(num) * 8) - 1; i >= 0; i--) {
        str += '0' + ((num >> i) & 1);
        if (i % 8 == 0) str += ' ';
    }
    return str;
}


int main()
{TRF;

    C c, cc(1,"2");
    c.x = 1234;
    c.mf1();
    cc.mf2();

    printf("qwea %d\n", c.constReturnMethod());
    I("qwea %d", c.constReturnMethod());

    I("12345");
    auto s = SAPF("1234");
    I("12345");

    D(SAPF("%d", c.x));
    D("%d", c.x);

    SecondFileClass sfc;
    sfc.function();

    R("raw text1");
    R("\r""raw text2");
    R("\n");

    f5(1,2,3,4);

    std::vector<C*> vecC;
    while(vecC.size() < 3)
        vecC.push_back(new C());

    C2 c2(vecC);

    // printf("%llu\n", Log::computeMaxNumberFromNumberLength(4));
    // printf("%llu\n", Log::increaseNumberToClosestTwoSquare(Log::computeMaxNumberFromNumberLength(4)));

    for(int i=0; i<6; i++)
    {
        int _i = 1 << i;

        printf("%s -> %s\n", print_binary(_i-1).c_str(),
            print_binary(_i-1).c_str()
        );

        printf("%s -> %s\n", print_binary(_i  ).c_str(),
           print_binary(_i  ).c_str()
        );

        printf("%s -> %s\n", print_binary(_i+1).c_str(),
           print_binary(_i+1).c_str()
        );

        printf("\n");
        fflush(stdout);
    }

    for(C *c_ptr : vecC)
        delete c_ptr;
}
