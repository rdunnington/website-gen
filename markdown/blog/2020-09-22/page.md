#Simple CI in Github
###2020-9-22

One of the goals of the [rjd]([newtab] https://github.com/rdunnington/rjd) engine/library project I’ve been working on is to be multiplatform. As of this writing it’s building on both Windows and macOS. Up until recently, there wasn’t any CI, so to see if the latest changes broke the other platform, I’d need to actually boot up my other machine to check. Then after the changes were committed, switch back and make sure those didn’t break anything. Definitely a somewhat tedious process, especially with all the hardware swapping. There are a lot of ways to solve that problem, but I heard about Github Actions recently and figured I could implement a simple CI system that would help validate changes for both platforms.

I’m not going to do an explanation of how Github Actions works - their [quickstart]([newtab] https://docs.github.com/en/actions/quickstart) guide does a fine job of that. But actually getting some examples of a simple working CI were hard to find, especially for C. We’ll go through it step by step, but here’s a link to the final configuration so you can see what it looks like: [ci.yml](/blog/2020-9-22/ci.yml).

Step one is to make a file at the path: `.github/workflows/ci.yml`. You can also use the web UI to edit the file, which can be nice since it'll catch syntax errors and provide autocomplete suggestions. Github should automatically detect that this is an action once you have it setup with the `name` and `on` tags as below. This is the default Github sets you up with, and it will ensure any commits or PRs into `master` get coverage.

```
name: CI

on:
  push:
    branches: [ master ]
  pull_request:
    branches: [ master ]
```

Now we need to setup the jobs for the needed platforms. This example will use OSX and Windows as they're what the rjd project is using.

###macOS
For a simple project, just use a makefile. Using CMake or other multi-step build systems is overkill. You can see how we have an entry for the default make as well as what to run for tests. That makes it easy to change the test command without having to fiddle with this CI configuration file again.

```
all:
	gcc *.c -o program
test:
	./program --run-tests
```

Now we can define the job and break it up into checkout, build, and test steps.

```jobs:
  build_osx:
    name: OSX Build & Test
    runs-on: macos-latest
    steps:
      - name: checkout
        uses: actions/checkout@v2
      - name: build
        run: make
      - name: run tests
        run: make test 
		timeout-minutes: 1
```

The default environment for the macOS VMs Github uses has make, but you can see the full list of software in a list [here]([newtab] https://docs.github.com/en/actions/reference/specifications-for-github-hosted-runners).

###Windows
It always has to be tricky on a Microsoft platform. Everyone has their favorite build system, and some people even handroll their own. However, I heard a great tip listening to [Handmade Hero]([newtab] https://youtu.be/Ee3EtYb8d1o?t=1349) once. Make a batch file and be done with it. It’s great for small projects and avoids the needless complexity of yet another build system.

```
@echo off
cl *.c /F tests.exe
```

Before attempting to build, we need to initialize the environment to know about the Visual Studio compiler. Luckily VS has a number of batch files to do the job. For now we’ll just use the x64 version. Note that this path uses the enterprise version installed in the machine environment, but you can find the same files in the Community edition. Also note the special vertical bar denoting a multiline script. Finally, because these actions are run in Powershell by default, we need to specify the cmd shell to avoid any bad interactions with the batch file.

For running the tests, you can make a batch file to call the test exe to wrap it like the makefile, or just call it directly in the config file.

```
  build_win32:
    name: Windows Build & Test
    runs-on: windows-latest
    steps:
      - name: checkout
        uses: actions/checkout@v2
      - name: build
        shell: cmd
        run: |
          call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
          ./build.bat
      - name: run tests
        run: .\tests.exe
    	timeout-minutes: 1
```

And that's it! CI in just a couple simple shell scripts and one small configuration file. You can see a working example of this on the [rjd]([newtab] https://github.com/rdunnington/rjd) github page.

