#Projects
Like many programmers, I've had plenty of half-baked ideas in my spare time, but here are a few clean enough for public viewing.

##bytebox
A zero-dependency WebAssembly runtime.
My first serious Zig project, I started writing this as a way to learn about next-gen web technology. In the future, I see WebAssembly as not limited only to the web, but a serious contender for becoming a new platform-independent IR that can be easily translated to platform-dependent machine code, simplifying distribution and deployment toolchains. Though this started as a learning exercise, I'm excited to see how it can provide real value to other people.
Platforms: Windows, MacOS, and Linux
[Source]([newtab] https://github.com/rdunnington/bytebox)

##zig-slotmap
Handle-based array.
Zig is a great up-and-coming language with the potential to replace C. While writing a small asteroids game with it, I needed a handle-based, contiguous-memory data structure. I decided to publish a small standalone library that other people could use in their own projects.
The slotmap data structure has been around for a very long time in concept and implementation, but there hasn't been a common name until recently, where the name "Slotmap" seems to have won the popular vote.
[Source]([newtab] https://github.com/rdunnington/zig-slotmap)

##zig-stable-array
Array with address-stable memory that works by leveraging virtual memory.
Another zig project, and one that implements a container I find myself using more and more these days. Specify a large amount of uncommitted virtual up-front, and never worry about the overhead of large array resizes. Also good to use if you want to guarantee pointer stability when the array grows. Uses `std.ArrayList` semantics, so it's relatively quick to use as a drop-in replacement if needed.
Platforms: Windows, MacOS, and Linux.

##rjd
A lightweight cross-platform game engine written in C11.
This started as an experiment in how to do things without any of the object-oriented or RAII paradigms of C++, and I quickly fell in love with the simplicity and cognitive load of C. I add more features to the library when I feel sufficiently motivated, though these days most of my development efforts have moved to Zig projects.
I'm bad at naming things, so I took a cue from the stb libraries and just used my initials.
Platforms: Windows, MacOS
[Source]([newtab] https://github.com/rdunnington/rjd)

##rjd-samples
Samples showing off features of the rjd APIs.
Platforms: Windows, MacOS
[Source]([newtab] https://github.com/rdunnington/rjd-samples)
