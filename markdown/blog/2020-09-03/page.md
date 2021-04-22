#Scoped Resources in C
###2020-9-3

For the purpose of this post, I’ll define a "resource" as some object that requires initialization and cleanup. Resources can be created at one time and used much later, or they can be created for local use and immediately cleaned up when the function exits. Some examples: files, locks, and memory. Initialization is easy to remember because it usually won’t work otherwise. But forgetting to cleanup the object often doesn’t immediately break the program execution, but can lead to problems much later, making it tricky to track down the source of the problem.

In C++, making this bulletproof is a no-brainer. You just use a class with a constructor and destructor pair. The constructor initializes the object, then the destructor cleans it up.

```
class ScopedResource
{
	ScopedResource()
	{
   		ResourceInitialize(&amp;m_resource);
	}

	~ScopedResource()
	{
		ResourceCleanup(&amp;m_resource);
	}

	Resource m_resource;
};
```

In C, we would just use the `ResourceInitialize()` and `ResourceCleanup()` functions directly, because it doesn’t have destructors. In general, I think that’s A Good Thing™, but that’s the subject of another post. In this type of scoped resource cleanup situation however, it will inevitably lead to situations where you will forget to call the cleanup function, no matter how careful you are.

```
void foo()
{
	struct Resource scoped_resource;
	ResourceInitialize(&amp;scoped_resource);

	// Several pages of code later, a return without a cleanup. Oops!
}
```

In case you haven’t listened to it yet, go check out the Our Machinery podcast. Trisha, Niklas, and Tobias have a wealth of knowledge from years in the industry to share. I was listening to the [Bad Decisions]([newtab] https://anchor.fm/ourmachinery/episodes/S2-EP4-Bad-Decisions-eafc81) episode and heard a truly great tip from Niklas about how to enforce paired macros in C.

The tip is to lean on the unused variable warning. If the init/cleanup calls are wrapped in macros, that detail is hidden from the user and you get the automatic benefit of a warning if a matching cleanup macro is not used.

```
#define SCOPED_RESOURCE_BEGIN(name)		\
	struct Resource name;			\
	ResourceInitialize(&amp;name);		\
	int name ## _END_SCOPE_NOT_FOUND;

#define SCOPED_RESOURCE_END(name)		\
	ResourceCleanup(&amp;name);			\
	(void)name ## _END_SCOPE_NOT_FOUND;

void foo()
{
	SCOPED_RESOURCE_BEGIN(scoped_resource);
	// ...
	SCOPED_RESOURCE_END(scoped_resource);
}
```

If the matching end scope isn’t found, you’ll get a warning if you have a high enough warning level turned on. This error is from gcc:

```
main.c: In function ‘main’:
main.c:29:24: error: unused variable ‘scoped_resource_END_SCOPE_NOT_FOUND’ [-Werror=unused-variable]
   29 |  SCOPED_RESOURCE_BEGIN(scoped_resource);
      |                        ^~~~~~~~~~~
main.c:21:6: note: in definition of macro ‘SCOPED_RESOURCE_BEGIN’
   21 |  int name ## _END_SCOPE_NOT_FOUND;
	  |      ^~~~
```

Of course, this assumes you are working in a project that has the capability of turning on such warnings and ideally enabling warnings as errors. Not all of us can be so lucky.
You can download the code I used to test this idea [here](2020-9-3/main.c). Build with the appropriate warning level on to repro the error and the flag to turn warnings as errors on:

```
gcc -Wall -Werror main.c
cl /W3 /WX main.c
```

