#Five Resources
###2020-5-25

My last post mentioned there have been a number of articles, talks, and people who have impacted my career. It would be amiss if I didn’t at least share what some of them were. Here are five of the most valuable in the rough chronological order I encountered them.

###[Tactics, tactics, tactics]([newtab] http://dlowe-wfh.blogspot.com/2007/06/tactics-tactics-tactics.html)
####J. David Lowe

Somehow I stumbled across this blog post while I was in school. It helped me realize that while individual subjects of study are important, coding tactics are a force multiplier that make all other aspects of your work more effective. These days, the lesson has distilled into a single question: how can this code be simpler?

###[On Inlined Code]([newtab] http://number-none.com/blow/john_carmack_on_inlined_code.html)
####John Carmack

The subtle power of this idea is easy to overlook, like I did for years. Don’t try to hide complexity from the user and attempt to predict what you need to reuse and abstract ahead of time. Wait until you start seeing repeated patterns, then apply [semantic compression]([newtab] https://caseymuratori.com/blog_0015) to them. It saves time, leads to simpler code, and prevents harmful abstractions from taking root.

###[Data Oriented Design]([newtab] https://www.youtube.com/watch?v=rX0ItVEVjHc)
####Mike Acton

Data oriented design was a vague concept in my mind until Mike Acton’s talk. A key takeaway is gathering the input and output data requirements of the system. Once established, the system often becomes a linear data transformation, where each part of the pipeline can be broken apart into atomic pieces that are much easier to reason about.

###[Designing and Evaluating Reusable Components]([newtab] https://caseymuratori.com/blog_0024)
####Casey Muratori

Casey’s talk was the first I’d ever heard that broke API design down into individual pieces and had examples of them reflecting things I’d actually seen in the real world. The five characteristics Casey enumerates are invaluable metrics to evaluate both existing APIs and new ones you need to write for others.

###[Our Machinery Blog]([newtab] https://ourmachinery.com/post/)
####Niklas Gray, Tricia Gray, Tobias Persson
The Our Machinery team behind the influential [Bitsquid]([newtab] http://bitsquid.blogspot.com/) engine has a new blog, and it's the best you’ll find anywhere. Each post is clearly written and contains fresh ideas combined with the lessons learned from experienced careers. I couldn’t pick one article because they’re all just that good.
* [Defaulting to Zero]([newtab] https://ourmachinery.com/post/defaulting-to-zero/)
* [Vasa]([newtab] https://ourmachinery.com/post/vasa/)
* [The Anti-Feature Dream]([newtab] https://ourmachinery.com/post/the-anti-feature-dream/)

