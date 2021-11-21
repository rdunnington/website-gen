#Projects
Like any programmer, I've had plenty of half-baked ideas in my spare time, but here are a few clean enough for public viewing.

##rjd
A lightweight cross-platform game engine written in C11.
This started as an experiment in how to do things without any of the object-oriented or RAII paradigms of C++, and I quickly fell in love with the simplicity and cognitive load of C. It's still in active development as I add more features to the library when I feel sufficiently motivated.
I'm bad at naming things, so I took a cue from the stb libraries and just used my initials.
[Source]([newtab] https://github.com/rdunnington/rjd)

##rjd-samples
Samples showing off features of the rjd APIs.
[Source]([newtab] https://github.com/rdunnington/rjd-samples)

##zig-slotmap
Zig is a great up-and-coming language with the potential to replace C. While writing a small asteroids game with it, I needed a handle-based, contiguous-memory data structure. I decided to publish a small standalone library that other people could use in their own projects.
The slotmap data structure has been around for a very long time in concept and implementation, but there hasn't been a common name until recently, where the name "Slotmap" seems to have won the popular vote.
[Source]([newtab] https://github.com/rdunnington/zig-slotmap)

##zig-wasmhost
WebAssembly is the new hotness on the web, but I see it and the new APIs designed to allow low-level access to system resource such as WebGPU and WebAudio as a key to getting free of the nightmares of the unholy HTML/CSS/Javascript trinity. This project is an effort to write a standalone WebAssembly VM both to learn the strengths and weaknesses of the format and the first step towards a new low-level oriented browser protocol.
[Source]([newtab] https://github.com/rdunnington/zig-wasmhost)
