#Comparing Aggregate Data
###2021-04-24

Comparing aggregate types in idiomatic C++ is simple, right? You just need to write a custom `operator==` for every aggregate that you want to compare. If the type contains more objects, you'll need to implement `operator==` for those as well. C++20 is a bit better here as it autogenerates `operator==` that does a simple memberwise comparison. But if the object happens to contain raw pointers or even `std::unique_ptr` and you want a deep comparison, you'll need to implement it yourself again to handle the special cases, since the compiler won't automatically know which ones are supposed to be compared by value and which ones are deep. And if you implement `operator==`, often you want to implement `operator!=` to be semantically complete. Oh yeah, and don’t forget to update your comparison functions if you add a new member to your type later on! ...Ok, this is getting a bit complicated.

In C, aggregate types don’t have comparison operators. If you try to take the C++ approach of manually writing compare functions for your types, you have the same maintenance problems that C++ has. And it’s a bit more error prone than C++, because it’s not obvious from a user of your code that they should be calling the comparison function unless they happen to see it in the API.

However, as usual, I believe C merits its own, simpler approach. Just use memcmp! A struct that requires a custom recursive comparison is a strong code smell that indicates you should probably rethink how you’re storing your data to avoid dynamic allocations sprinkled throughout your codebase. Consider using slotmaps or intern pool strategies to make the simple memory comparison foolproof. I suspect you’ll find it makes the overall code simpler and more resilient as well.

However, a far more tricky gotcha when memcmp-ing structs is uninitialized padding. I tend to write all my structs to be compatible with the [Defaulting to Zero]([newtab] https://ourmachinery.com/post/defaulting-to-zero/) philosophy, and as a consequence, usually use the aggregate-initialization syntax to init everything to zero like so:

```
struct foo f = {0};
```

However, I ran into a problem where I was writing some simple serialization code and tests for it. The struct looked like:

```
struct data
{
    uint64_t a;
    uint32_t b;
    uint16_t c;
    uint8_t d;
};
```

I had an “expected” one on the stack that I filled out with the data I expected, and one that ran through the serialization code. I ran memcmp on these two and got a test failure. Ok, no problem, there must be a bug in the serialization code. But there wasn’t. Looking at the structs in the debugger, everything looked the same.

That was the day I learned struct padding doesn’t get initialized with the zero-init syntax. It only initalizes explicitly declared struct members. When I added pad bytes to manually align the struct to 8 bytes, they got initialized to zero and the test passed.

```
struct data
{
    uint64_t a;
    uint32_t b;
    uint16_t c;
    uint8_t d;
    uint8_t pad[1];
};
```
The major compiler vendors have warnings to let you know when structs get padded out to their default alignment. Even if you don’t want the overhead of turning it on for your entire codebase, it’s still very useful in certain situations where so many structs are allocated in an array that packing them tightly can save a few megs, and you want some help seeing any hidden padding.

On GCC:

```
#pragma GCC diagnostic push
#pragma GCC diagnostic error "-Wpadded"

struct A
{
    char a;
    int b;
    char c[1];
};

#pragma GCC diagnostic pop

gcc.c:7:6: error: padding struct to align ‘b’ [-Werror=padded]
    7 |  int b;
      |      ^
gcc.c:9:1: error: padding struct size to alignment boundary [-Werror=padded]
    9 | };
      | ^
```

On MSVC:

```
#pragma warning(push)
#pragma warning(error:4820)

struct A
{
    char a;
    int b;
    char c[1];
};

#pragma warning(pop)

msvc.c(7): error C4820: 'A': '3' bytes padding added after data member 'a'
msvc.c(9): error C4820: 'A': '3' bytes padding added after data member 'c'
```

Overall I find C's approach simpler and requires less code maintenance overall. C has its own gotchas, but I prefer gotchas that require me to write and maintain less code to deal with them.

