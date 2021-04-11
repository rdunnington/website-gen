#Enforcing Static Strings in Structs
###2020-10-14

Let’s say we have some struct that contains a string. For some reason, we really need this string to be static because we don’t want to bother with cleanup. For example, the [rjd]([newtab] https://github.com/rdunnington/rjd) library uses a uniform error reporting interface called `rjd_result`, which is simply a struct that contains a static string. If the string is null, there’s no error. If the string has been set, there was an error. Here’s a usage example:

```
struct rjd_result
{
    const char* error;
};

struct rjd_result result = some_api_call();
if (result.error) {
    printf(“some_api_call returned: %s\n”, result.error);
    // handle the error
    return;
}
```

The user doesn’t have to worry about cleaning up the struct and can early out, because the error string is statically allocated. Until recently, this was not enforced and you had to know to never set a dynamic string in the struct. I was thinking about how to make this better and came up with a simple, portable way to do it. If you use a macro to create the struct, you can have the macro require the passed string to be static. Note that this requires support for C99’s compound literals.

```
#define RJD_RESULT(static_string) ((struct rjd_result){“” static_string}))

// using it later:
return RJD_RESULT(“unknown error”);
```

The key here is using C’s automatic static string concatenation to force the expression to be considered a static string. The empty quotes inside the compound literal are ensuring static_string is actually a static string. Note that the error message for passing a non-static string is not very good.

```
struct rjd_result
{
    const char* str;
};

#define RJD_RESULT(static_string) ((struct rjd_result){"" static_string})

int main(void)
{
    const char* p = "test";
    struct rjd_result r = RJD_RESULT(p);

    return 0;
}
```

Building the above code in clang results in the error:

```
main.c:11:35: error: expected '}'
        struct rjd_result r = RJD_RESULT(p);
                                         ^
main.c:11:24: note: to match this '{'
        struct rjd_result r = RJD_RESULT(p);
                              ^
main.c:6:55: note: expanded from macro 'RJD_RESULT'
#define RJD_RESULT(static_string) ((struct rjd_result){"" static_string})
```

Hopefully the `static_string` variable name in the macro source is enough to point the user in the right direction.
This enforcement method still relies on the macro. If the user decides to manually construct an `rjd_result` with a dynamic string there’s nothing stopping them from doing that. But maybe there’s a good reason for doing so, and the programmer will ensure that particular result will not be propagated all the way up to client code, to keep the “no need for cleanup” guarantee.
If anyone comes up with a way of doing this that has better error messaging or avoids macros, please let me know. 😊


