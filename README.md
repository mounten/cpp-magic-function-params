# C++ Magic functions parameter example

After reading the [rust axum magic function example](https://github.com/alexpusch/rust-magic-function-params) and how it is used in [Axum](https://github.com/tokio-rs/axum), i wanted to try if the same concept of magic functions can be implemented in c++.

For example magic functions in rust are like:

```rust
fn print_id(id: Id) {
    println!("id is {}", id.0);
}

// Param(param) is just pattern matching
fn print_all(Param(param): Param, Id(id): Id) {
    println!("param is {param}, id is {id}");
}

pub fn main() {
    let context = Context::new("magic".into(), 33);

    trigger(context.clone(), print_id);
    trigger(context.clone(), print_all);
```

And in c++ this is could be implemented like:

```cpp
void print_id(Id id)
{
    std::cout << "id is " << id() << std::endl;   
}

void print_all(Param param, Id id)
{
    std::cout << "param is " << param();
    std::cout << ", id is "  << id() << std::endl;
}

int main()
{
    Context context { "magic", 42};

    trigger(context, callable(print_id));
    trigger(context, callable(print_all)); 
}
```

## How it is done
### The Context
```cpp
struct Context
{
    std::string param;
    int id;
};
```
The `Context` holds the data that can be extracted by other classes.

### The FromContext Concept as a template type constraint (since c++20)
```cpp
template<typename T>
concept FromContext = requires(const Context &ctx) 
{
    { T::from_context(ctx) } -> std::convertible_to<T>;
};
```

We define a new concept as a type constraint. The concept checks if the class/struct has implemented a static `extract` method for its own type. The `extract` method extracts the needed data from the `Context`.

Technically Concepts and c++20 features are not needed. But the compiler outputs better compile errors when using concepts as template constraints.

For Example:
```cpp
struct Param
{
    std::string param;

    static Param extract(const Context &ctx) { return {ctx.param}; }
};
```

## Handler and callable helper method
We define Handler as a Interface for our magic function.
The Handler has a `Context` as Parameter.

```cpp 
using Handler = std::function<void(Context)>;
```

The `callable` helper method does all the magic. It returns a lambda of signature `Handler` and calls the static `extract` method for all parameter types from the given function (in our case Param or Id).

```cpp
template<FromContext T>
Handler callable(void (*fn)(T))
{
    return [fn](Context ctx){
        fn(T::extract(ctx));
    };
}
```

To support multiple function parameters the `callable` helper method has to be implemented for 2, 3, 4 and so on template types. The template method is agnostic to the order of parameters and generates combinations for different types like `callable<Id, Param>` and `callable<Param, Id>`.

```cpp
template<FromContext T1, FromContext T2>
Handler callable(void(*fn)(T1,T2))
{
    return [fn](Context ctx){
        fn(T1::extract(ctx), T2::extract(ctx));
    };
}
```

Without Concepts and c++20 features, we can replace `FromContext T` with `typename T`.
##

## Putting it together
We can now define a simple trigger function that takes a `Context` and a `Handler`
```cpp
void trigger(Context context, Handler handler)
{
   handler(context); 
}
```
Now when we call trigger 
```cpp
Context context { "magic", 42};
trigger(context, callable(print_id));
```
## What happens
- `callable(print_id)` will generate a template instantiation for `T = Id`.
- Inside the `Handler` lambda `Id::extract(context)` is called.
- `print_id` is called with the expected paramter type Id.

voila
