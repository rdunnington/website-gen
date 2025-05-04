#Resume
##Reuben Dunnington: Systems Programmer
Empathetic, intrinsically motivated generalist. Building performant, scalable systems across teams and disciplines.

##Professional Experience

<div class="resume-header">
	<div class="resume-h1-left">Unannounced Open World RPG</div>
	<div class="resume-h1-right">Believer Entertainment</div>
</div>
<div class="resume-header">
	<div class="resume-h2-left">PC, iOS</div>
	<div class="resume-h2-right">August 2023 to Present</div>
</div>

* Developed technical interview process for systems engineers and made key hires. Cross-team systems eng meeting, driving quality, performance, scaling, identifying problems with systems, process, and workflow.
* Led development on a Mass-based NPC spawn/despawn system featuring multi-agent patrols with editor and runtime tooling and visualization. Character cap and intelligent despawn.
* Created and drove Unreal upgrade process, coordinating stakeholders across the studio to ensure minimal disruption. "Smoothest engine upgrade I've seen in my career" - Principal Tech Artist Matt Canei.
* Created optimization plan highlighting effort-to-reward. Implemented character and replication optimizations, bringing single-threaded server frametime down from 54.97ms avg (141ms max) to 21.83ms avg (62ms max).
* Championed and drove debug menu adoption across the studio. Integrated and [modified Imgui]([newtab] https://github.com/believer-oss/UnrealImGui) to work seamlessly in a client-server environment.
* Custom OSS [git source control]([newtab] https://github.com/believer-oss/ethos/tree/main/friendshipper) solution for Unreal. UGS parity with game/engine syncing for content creators. Rust-based external tooling. OFPA name translation. Optimizations for [Unreal plugin]([newtab] https://github.com/believer-oss/FriendshipperSourceControl) bringing ops from 20s to 0.5s, and batched locking 1000x faster in DynamoDB (1500s -> 1.5s).
* From-scratch code testing framework, with Slate UI test runner. Code tests have native coroutine support. Unit, "world", and client-server tests for guaranteeing replicated behavior.
* OpenTelemetry library integration and [OSS plugin](https://github.com/believer-oss/UnrealOpenTelemetry) featuring spans, metrics, and events. Built-in editor, map, and PIE load timing observability.
* Mentored interns, junior, and senior engineers. No gameplay or systems experience to effective Unreal C++ developers in ~6 months. Code reviews, “mob” programming, and quick feedback loops to accelerate learning.

<div class="resume-header">
	<div class="resume-h1-left">Canceled PvEvP FPS</div>
	<div class="resume-h1-right">Team Kaiju</div>
</div>
<div class="resume-header">
	<div class="resume-h2-left">PC</div>
	<div class="resume-h2-right">March 2022 to August 2023</div>
</div>

* Identified and optimized both code and data sources of Unreal game thread perf problems across a variety of different systems. Champion for workflow iteration times, improving PIE performance from 45ms down to 15ms, map load times from 5+ mins to ~30s, and out-of-process PIE startup times from 30+ mins to <1 min.
* Wrote custom tooling and debug systems to support content creators' specific needs around animation, world building, audio (Wwise), and gameplay systems. Extended existing Unreal systems with new UI and hotkeys.
* Owned the full stack of NPC AI, locomotion, and animation systems, implementing behavior tree-driven, designer-tweakable, custom movement abilities synced with animation in a networked environment.
* Extended the Houdini Engine for Unreal plugin to leverage native landscape spline curves for procgen roads.
* Champion for test automation, extending the native Unreal framework to simplify writing tests, creating multiple code and BP-oriented tests to lead by example, and integrated them with our automated Jenkins pipeline.
* Leveraged Unreal replication and RepGraph, minimizing net traffic while providing a compelling real-time, debuggable experience on the client for gameplay systems such as destruction and AI-oriented functionality.
* Fixed crashes and critical bugs in various engine-level systems, such as World Partition, Level Instances, UObject, Navigation, Input, Animation, and Gameplay Ability System.

<div class="resume-header">
	<div class="resume-h1-left">Halo Infinite</div>
	<div class="resume-h1-right">343 Industries</div>
</div>
<div class="resume-header">
	<div class="resume-h2-left">Xbox Series X, Xbox One, PC</div>
	<div class="resume-h2-right">Jan 2018 to March 2022</div>
</div>

* Ported Slipspace engine and shader compiler pipeline to the prerelease Xbox Series X hardware, OS, and SDK, while working with the platform team in a tight loop of kernel and SDK bug feedback, testing, and verification.
* Aggressively profiled and optimized memory for many systems in different parts of the engine, saving hundreds of megs that allowed the memory-constrained Xbox One title to ship and increasing Mean-Time-To-Failure over 4x (46.6 to 197.6 mins).
* Owned all engine-level memory APIs, creating new in-engine and standalone tools for memory analysis, improving existing tracking features, and hooking telemetry support for continuous integration visiblity.
* Profiled and implemented optimizations for major perf bottlenecks in the asset pipeline's asset metadata system, in some cases shaving hours off nightly build times.
* Upgraded tools, asset pipeline, and infrastructure to support external studios. Enabled multiple codevelopment partners to be able to work on code and assets worldwide, which before would have been impossible.

<div class="resume-header">
	<div class="resume-h1-left">Dragonstone: Kingdoms</div>
	<div class="resume-h1-right">Ember Entertainment</div>
</div>
<div class="resume-header">
	<div class="resume-h2-left">iOS, Android</div>
	<div class="resume-h2-right">April 2016 to Nov 2017</div>
</div>

* Discovered and executed CPU and memory optimization opportunities in client code, including a new custom Lua memory profiler and content tools that shaved over 40% of overall memory usage and cut load times in half.
* Designed and implemented NoSQL-based systems capable of unlimited horizontal scaling in a live environment.
* Created an automatic server-based hot-reload system for Lua to enable quick iteration on mobile devices.
* Full stack engineer for gameplay. UI/UX, gameplay logic, node.js backend, redis and rethinkdb storage.
* Mentored junior engineers, increasing their independence and spreading knowledge of in-house systems.

<div class="resume-header">
	<div class="resume-h1-left">Scribblenauts: Fighting Words</div>
	<div class="resume-h1-right">5th Cell Media</div>
</div>
<div class="resume-header">
	<div class="resume-h2-left">iOS</div>
	<div class="resume-h2-right">June 2015 to March 2016</div>
</div>

* Led engineering development from inception through closed beta with excellent KPIs, resulting in an external evaluation projection of $30 million gross revenue over 1 year.
* Co-designed and integrated data analytics and A/B testing solutions in collaboration with publisher requirements to discover, analyze, and resolve user experience problems and increase retention and revenue.
* Implemented many crucial game features including core loop battle system, UI prototypes, and data pipelines.
* Collaborated closely with designers on tutorial, iterating quickly on publisher feedback for greenlight milestones.
* Mentored remote engineer new to the project, providing feedback and building their connection with the team.

<div class="resume-header">
	<div class="resume-h1-left">Scribblenauts Unlimited</div>
	<div class="resume-h1-right">5TH Cell Media</div>
</div>
<div class="resume-header">
	<div class="resume-h2-left">iOS</div>
	<div class="resume-h2-right">Oct 2015 to Nov 2015</div>
</div>

* Gathered publisher requirements, achieving milestones for data analytics, ads, and in-app messaging.

<div class="resume-header">
	<div class="resume-h1-left">Onyx Engine</div>
	<div class="resume-h1-right">5TH Cell Media</div>
</div>
<div class="resume-header">
	<div class="resume-h2-left">PC, XboxOne, iOS</div>
	<div class="resume-h2-right">July 2013 to June 2015</div>
</div>

* Integrated Scaleform's full technology stack including custom renderer, ActionScript interop, and input on multiple platforms to meet milestone requirements, gameplay programmer needs, and UX design aspirations.
* Invented a custom localization solution that integrated tightly with inhouse tools, streamlining designer workflows while reducing localization costs.

<div class="resume-header">
	<div class="resume-h1-left">Scribblenauts Unmasked</div>
	<div class="resume-h1-right">5TH Cell Media</div>
</div>
<div class="resume-header">
	<div class="resume-h2-left">Nintendo WiiU, 3DS, PC</div>
	<div class="resume-h2-right">Sept 2012 to July 2013</div>
</div>

* Added and polished features on engine and gameplay systems, optimizing the game on 3DS to run at 30 FPS.
* Owned and improved designer tool workflows. Fixed an “impossible” tools bug that cost countless man-hours.

<div class="resume-header">
	<div class="resume-h1-left">Scribblenauts Unlimited</div>
	<div class="resume-h1-right">5TH Cell Media</div>
</div>
<div class="resume-header">
	<div class="resume-h2-left">Nintendo WiiU, 3DS, PC</div>
	<div class="resume-h2-right">April 2012 to Sept 2012</div>
</div>

* Implemented features, fixed bugs, and improved stability for a critical E3 demo a month after joining the team.
* Closely collaborated with artists to realize their UI vision throughout the game on multiple platforms.

<div class="resume-header">
	<div class="resume-h1-left">Run Roo Run!</div>
	<div class="resume-h1-right">5TH Cell Media</div>
</div>
<div class="resume-header">
	<div class="resume-h2-left">iOS</div>
	<div class="resume-h2-right">March 2012 to May 2012</div>
</div>

* Single-handledly shipped the universal version of the game, adding iCloud save and other bugfixes.

<div class="resume-header">
	<div class="resume-h1-left">Canceled Mobile Title</div>
	<div class="resume-h1-right">5TH Cell Media</div>
</div>
<div class="resume-header">
	<div class="resume-h2-left">iOS</div>
	<div class="resume-h2-right">March 2011 to Mar 2012</div>
</div>

* Implemented gameplay features and designer-oriented editing tools that tightly integrated with Unity.

<div class="resume-header">
	<div class="resume-h1-left">Drawn to Life: Collection</div>
	<div class="resume-h1-right">5TH Cell Media</div>
</div>
<div class="resume-header">
	<div class="resume-h2-left">Nintendo DS</div>
	<div class="resume-h2-right">June 2010 to Aug 2010</div>
</div>

* Independently brought the entire project from inception to ship within two months.

<div class="resume-header">
	<div class="resume-h1-left">Valencia</div>
	<div class="resume-h1-right">Yorba Foundation</div>
</div>
<div class="resume-header">
	<div class="resume-h2-left">Linux</div>
	<div class="resume-h2-right">June 2009 to Aug 2009</div>
</div>

* Supported and added major features, polished UX, and met inhouse needs for a Vala IDE plugin for Gedit.

##Open Source Experience
<div class="resume-header">
	<div class="resume-h1-left">[Orca]([newtab] https://github.com/orca-app/orca)</div>
</div>
<div class="resume-header">
	<div class="resume-h2-left">Windows, MacOS</div>
	<div class="resume-h2-right">Aug 2023 to Present</div>
</div>
* Major contributor to the Orca project, a development platform and runtime environment for cross-platform, sandboxed graphical applications.
* Implemented a full C standard library implmentation in terms of Orca APIs to provide a familiar programming environment for C programmers.
* Provided a complete set of idomatic API bindings to the Zig programming langugage.
* Replaced raw calls to wasm3 with a custom wasm layer that allows for easy compile-time switching between wasm backends.
* Platform layer work fixing bugs and bringing Windows to parity with MacOS.

<div class="resume-header">
	<div class="resume-h1-left">[Bytebox]([newtab] https://github.com/rdunnington/bytebox)</div>
</div>
<div class="resume-header">
	<div class="resume-h2-left">Windows, MacOS</div>
	<div class="resume-h2-right">Oct 2021 to Present</div>
</div>
* Primary author and maintainer of a from-scratch WebAssembly interpreter written in Zig, capable of running in standalone mode or embedded as a library.
* Fully compliant with the WASM 2.0 spec.
* Implemented WASI Preview 1 support, an extension that allows WebAssembly programs to interact with objects outside it's sandbox via IO.
* Debugging interface supports single-stepping, and tracing per-function or per-instruction.
* C FFI interface for use in other languages.

##Education
<div class="resume-header">
	<div class="resume-h2-left">B.S. in Computer Science</div>
	<div class="resume-h2-right">Digipen Institute of Technology</div>
</div>
<div class="resume-header">
	<div class="resume-h2-right">2011</div>
</div>
