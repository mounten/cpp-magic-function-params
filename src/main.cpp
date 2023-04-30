#include "magic.h"

#include <iostream>

void print_nothing()
{
    std::cout << "no parameters" << std::endl;  
}

void print_id(Id id)
{
    std::cout << "id is " << id() << std::endl;   
}

void print_param(Param param)
{
    std::cout << "param is " << param() << std::endl;
}

void print_all(Param param, Id id)
{
    std::cout << "param is " << param() << ", id is " << id() << std::endl;
}

void print_all_switched(Id id, Param param)
{
    std::cout << "param is " << param() << ", id is " << id() << std::endl;
}

int main()
{
    Context context { "magic", 42};

    trigger(context, callable(print_nothing));
    trigger(context, callable(print_id));
    trigger(context, callable(print_param));
    trigger(context, callable(print_all));
    trigger(context, callable(print_all_switched));
}