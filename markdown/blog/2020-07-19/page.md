#Flat Pimpl
###2020-7-19

The [pimpl idiom]([newtab] https://en.cppreference.com/w/cpp/language/pimpl) is a common approach to hiding implementation details in C/C++. A typical C++ use case looks like:

```
// .h
class FileImpl;
class File
{
	File();
	~File();
	bool Open(const char* path);
	void Close();

	std::unique_ptr<FileImpl> pimpl;
};
```

There are some problems with this approach:

* Another dynamic allocation is required. You could do some sort of pooled approach where the object comes from an arena and another block of structs are allocated on demand. However, in my experience that almost never happens in cases where pimpl is used.
* There is always an extra pointer hop. There’s no way to get around this one if you want the members of the implementation to remain completely private. It’s the cost of the way C/C++ defines type linkage through headers.
* There's a hidden maintenance cost, where object-oriented design can guide well-meaning programmers into replicating the interface of the public object down to the private object. The public interface ends up being a wrapper object making mirrored calls down to the private object’s functions, duplicating the interface code for no benefit.

What if we did it more C-style? Still has similar problems, but at least there isn’t the OOP trap to fall into.

```
// .h
struct file_impl_t;
struct file_t
{
	struct file_impl_t* pimpl;
};

bool file_open(struct file_t* file, const char* path);
void file_close(struct file_t* file);
```

Since we're in C, we don't have any special requirements regarding RAII. What if we reserved all the space for the private implementation inside the struct?

```
// .h
struct file_t
{
	char pimpl[32];
};

// .c
struct file_impl_t
{
	// OS-specific data
};;

_Static_assert(sizeof(struct file_impl_t) &lt;= sizeof(struct file_t));
```

No extra dynamic allocation or pointer hop needed. All this takes is an extra cast inside the functions. Both C++11 and C11 have static assert, so you can guarantee the casts are safe and catch potential cast size mismatches at build time.

```
bool file_open(struct file_t* file, const char* path)
{
	struct file_impl_t* impl = (struct file_impl_t*)file;
	// ...
}
```

This also folds nicely into the principle of [zero-by-default]([newtab] https://ourmachinery.com/post/defaulting-to-zero/). If the API follows this convention, users can simply zero-initialize the struct and not have to worry about another function that initializes the private implementation. This approach works well with C-style codebases that don’t do a lot of RAII.

he main drawback of this approach is the types become annoying to inspect in a debugger, since you have to cast them to the pimpl type. Most debuggers have custom visualizers that will make this a non-issue, like [Visual Studio]([newtab] https://docs.microsoft.com/en-us/visualstudio/debugger/create-custom-views-of-native-objects) and [gdb]([newtab] http://plohrmann.blogspot.com/2013/10/writing-debug-visualizers-for-gdb.html).

Here's an example for the above code in Visual Studio's visualizer:

```
&lt?xml version=&quot;1.0&quot; encoding=&quot;utf-8&quot;?&gt;
&lt;AutoVisualizer xmlns=&quot;http://schemas.microsoft.com/vstudio/debugger/natvis/2010&quot;&gt;
    &lt;Type Name=&quot;file_t&quot;&gt;
   	 &lt;Expand&gt;
   		 &lt;Item Name=&quot;Impl&quot;&gt;(file_impl_t*)impl&lt;/Item&gt;
   	 &lt;/Expand&gt;
    &lt;/Type&gt;
&lt;/AutoVisualizer&gt;
```

I call this technique "flat pimpl" because it's essentially flattening the old pointer pimpl into the parent struct. That's pretty much it. It's a pretty simple idea, but easy to remember.

