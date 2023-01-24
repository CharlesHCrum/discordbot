# bot
Bot for Discord that bass boosts audio files, and distorts images, gifs, and video files. 

# Contributors
Ben Tate, Kevin Daniels, Charles Crum

# Development Environment Setup
In order to get the C code working (and, incidentally, the ffmpeg code), the
recommended development environment is MinGW when on Windows.

First, install [MSYS2](https://www.msys2.org/). Then, start up the
MSYS2 MinGW x64 environment -- this is the primary development environment
for the C code.

(Note: there are several other environments that MSYS2 installs; the MinGW x64
environment is the one we need).

Once MinGW is open, you can install all the needed packages using pacman:

`pacman -S make mingw-w64-x86_64-gcc mingw-w64-x86_64-ffmpeg mingw-w64-x86_64-python`

(Note: these include make, gcc, ffmpeg, and python so far. We might want a resampling library in the future).

We also need to get discord.py working. This involves installing pip as well as 
some discord.py dependencies that are available on mingw:

`pacman -S mingw-w64-x86_64-python-pip mingw-w64-x86_64-python-yarl `

Then we can do pip install:

`pip install discord`

`pip install requests`

`pip install python-dotenv`

To get OpenCV working, install it on MinGW:

`pacman -S mingw-w64-x86_64-opencv`

Now you are ready to build and run the code.

First, you can cd into the 'bot' directory. If you've never used MinGW before,
note that you can cd into your C drive using cd /c/.

Just as an example: my Windows username is 'Sharp4', so the way I actually get
to the project directory on my machine is with:

cd /c/Users/Sharp4/Documents/College/CS321/bot

You will of course have to cd to where it is actually located on your computer,
but if you're new to MinGW you might not know how to get to the C drive.

Now, to build the various compiled code, just run build.sh:
`./build.sh`

This will build the C code and the image/video processing C++ code.
