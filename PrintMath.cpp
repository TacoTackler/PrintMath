// PrintMath.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <thread>

#include "random.h"

using namespace std;
using namespace test_tool;

static void print_guide()
{
    cout << "\nPrintMath.exe [<level> <count> <item_per_row>]"
        << "\nArgs:\n"
        << "\nlevel:  levels of math equation (1--add/sub, 2--add/sub/mul/div, 3--...), default is 1"
        << "\ncount:  number of math equations to be printed, default is 100"
        << "\nitem_per_row:  number of equations in each row, default is 5";
}

static void print_args(int level, int count, int item_per_row)
{
    cout << "\nArgs:"
        << "\nlevel:  " << level
        << "\ncount:  " << count
        << "\nitem_per_row:  " << item_per_row;
}

typedef int (*calc)(int& a, int& b);
typedef bool (*check)(int min, int max, int a, int b, int c);

static int add(int &a, int &b)
{
    return a + b;
}

static int sub(int &a, int &b)
{
    return a - b;
}

static int mul(int &a, int &b)
{
    return a * b;
}

static int div(int &a, int &b)
{
    if ((abs(a) < abs(b)) && a != 0)
    {
        int tmp = b;
        b = a;
        a = tmp;
    }
    if (b == 0)
        b = rand();
    int quotient = a / b;
    a = quotient * b;
    return quotient;
}

static int min4(int min, int a, int b, int c)
{
    return std::min(std::min(min, a), std::min(b, c));
}

static int max4(int max, int a, int b, int c)
{
    return std::max(std::max(max, a), std::max(b, c));
}

static bool add_check(int min, int max, int a, int b, int c)
{
    return (min4(min, a, b, c) == min) && (max4(max, a, b, c) == max);
}

static bool sub_check(int min, int max, int a, int b, int c)
{
    return (min4(min, a, b, c) == min) && (max4(max, a, b, c) == max);
}

static bool mul_check(int min, int max, int a, int b, int c)
{
    return (min4(min, a, b, c) == min) && (max4(max, a, b, c) == max);
}

static bool div_check(int min, int max, int a, int b, int c)
{
    return (min4(min, a, b, c) == min) && (max4(max, a, b, c) == max);
}

typedef struct Calculator
{
    string symbol;
    calc   calculator;
    check  checker;
} Calculator;

#define METHOD_ADD (0)
#define METHOD_SUB (1)
#define METHOD_MUL (2)
#define METHOD_DIV (3)

static Calculator CALC_MAP[] = {
    { "+", add, add_check},
    { "-", sub, sub_check},
    { "x", mul, mul_check},
    { "��", div, div_check}
};

static int LEVEL_MAP[] = { 2, 2, 4 };

static string generate_equation(Random &rnd, int level)
{
    const int DIGIT_MIN = 0;
    const int DIGIT_MAX = level < 2 ? 20 : 100;
    const int MUL_DIV_MAX = sqrt(DIGIT_MAX);
    int digit[3] = { rnd.random() % DIGIT_MAX, rnd.random() % DIGIT_MAX, 0 };

    // calculation and check, if failed generate again
    int method = rnd.random() % LEVEL_MAP[level];
    switch (method)
    {
    case METHOD_MUL:
        digit[0] %= MUL_DIV_MAX;
    case METHOD_DIV:
        digit[1] %= MUL_DIV_MAX;
        break;
    default:
        break;
    }
    digit[2] = CALC_MAP[method].calculator(digit[0], digit[1]);
    if (!CALC_MAP[method].checker(DIGIT_MIN, DIGIT_MAX, digit[0], digit[1], digit[2]))
        return generate_equation(rnd, level);

    // generate digits
    const string FILL_BLANK = "____"; // "(   )";
    int hide_idx = rnd.random() % 3;
    string digit_str[3];
    for (size_t i = 0; i < 3; i++)
    {
        if (hide_idx == i)
        {
            if (i != 2)
                digit_str[i] = FILL_BLANK;
        }
        else
            digit_str[i] = to_string(digit[i]);
    }

    // generate equation string
    string equation = digit_str[0] + " " + CALC_MAP[method].symbol + " " + digit_str[1] + " = " + digit_str[2];
    return equation;
}

uint32_t get_time_tick() {
    using namespace chrono;
    steady_clock::time_point tp = steady_clock::now();
    steady_clock::duration dtn = tp.time_since_epoch();
    return (uint32_t)((dtn.count() / 1000000) % (1 << 31));
}

int main(const int argc, char* const* const argv)
{
    if (argc > 1 && (!_stricmp(argv[1], "-h") || !_stricmp(argv[1], "--help")))
    {
        print_guide();
        return 0;
    }

    int level = 1;
    int count = 100;
    int item_per_row = 5;

    if (argc == 1)
    {
        // TODO: interaction command
    }
    else
    {
        level = atoi(argv[1]);
        if (argc > 2)
        {
            count = atoi(argv[2]);
            if (argc > 3)
            {
                item_per_row = atoi(argv[3]);
                if (item_per_row > count)
                {
                    item_per_row = count;
                    cout << "\nitem per row is less than count, will be modified to "
                        << count << "(same as count)";
                }
            }
        }
    }
    print_args(level, count, item_per_row);

    Random rnd(0, 1024, get_time_tick());
    for (int i = 0; i < count; i++)
    {
        if (!(i % item_per_row))
            cout << "\n";
        cout << generate_equation(rnd, level) << "\t";
    }

    return 0;
}
