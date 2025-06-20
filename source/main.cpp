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


std::string print_binary(size_t num)
{TRF;
    std::string str;
    for (int i = (sizeof(num) * 8) - 1; i >= 0; i--) {
        str += '0' + ((num >> i) & 1);
        if (i % 8 == 0) str += ' ';
    }
    return str;
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

    SecondFileClass sfc;
    sfc.function();

    R("raw text1");
    R("\r""raw text2");
    R("\n");

    f5(1,2,3,4);


    // printf("%llu\n", Log::computeMaxNumberFromNumberLength(4));
    // printf("%llu\n", Log::increaseNumberToClosestTwoSquare(Log::computeMaxNumberFromNumberLength(4)));

    // for(int i=0; i<30; i++)
    // {
    //     int _i = 1 << i;

    //     printf("%s -> %s\n", print_binary(_i-1).c_str(),
    //         print_binary(Log::increaseNumberToClosestTwoSquare(_i-1)).c_str()
    //     );

    //     printf("%s -> %s\n", print_binary(_i  ).c_str(),
    //        print_binary(Log::increaseNumberToClosestTwoSquare(_i  )).c_str()
    //     );

    //     printf("%s -> %s\n", print_binary(_i+1).c_str(),
    //        print_binary(Log::increaseNumberToClosestTwoSquare(_i+1)).c_str()
    //     );

    //     printf("\n");
    //     fflush(stdout);
    // }
}
