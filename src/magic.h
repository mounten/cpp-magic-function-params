#include <functional>
#include <type_traits>
#include <concepts>

#include <string>

struct Context
{
    std::string param;
    int id;

    static Context extract(const Context &context) { return context; }
};

struct Param
{
    std::string param;
    std::string operator()() { return param; }
    
    static Param extract(const Context &context) { return {context.param} ; }
};

struct Id
{   
    int id;
    int operator()() { return id; }

    static Id extract(const Context &context) { return {context.id}; }
};

template<typename T>
concept FromContext = requires(const Context &context) 
{
    { T::extract(context) } -> std::convertible_to<T>;
};

using Handler = std::function<void(Context)>;

template<FromContext... T>
Handler callable(void (*fn)(T...))
{
    return [fn](Context context){
        fn(std::forward<T>(T::extract(context))...);
    };
}

void trigger(Context context, Handler handler)
{
   handler(context); 
}