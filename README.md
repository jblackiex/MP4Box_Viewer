
<p align="right">
	<img alt="GitHub code size in bytes" src="https://img.shields.io/github/languages/code-size/jblackiex/MP4Box_Viewer?color=lightblue" />
	<img alt="Number of lines of code" src="https://img.shields.io/tokei/lines/github/jblackiex/MP4Box_Viewer?color=critical" />
	<img alt="Code language count" src="https://img.shields.io/github/languages/count/jblackiex/MP4Box_Viewer?color=yellow" />
	<img alt="GitHub top language" src="https://img.shields.io/github/languages/top/jblackiex/MP4Box_Viewer?color=black" />
	<img alt="GitHub last commit" src="https://img.shields.io/github/last-commit/jblackiex/MP4Box_Viewer?color=green" />
</p>


# MP4Box_Viewer
A small C++ application using Qt library that prints size and type of boxes found inside a "MPEG-4 Part 12 ISOBMFF" file. Then prints MDAT box content and saves encoded images (if found).

Read more about file format here: https://en.wikipedia.org/wiki/ISO_base_media_file_format

The file i use is ```"text0.mp4"``` and it contains multiple, nested boxes: one MOOF box (which itself has two sub
boxes) and one MDAT box; this is the structure:

● MOOF:

    ○ MFHD
    
    ○ TRAF:
          ■ TFHD
          
          ■ TRUN
          
          ■ UUID
          
          ■ UUID
● MDAT

The first four bytes (bytes 0 - 3) specify the size (or length) of the box (it’s a 4 Byte Big Endian
Integer).. Bytes 4 - 7 specify the type of the box (each byte can read as an ASCII character). There are 8 remaining bytes for actual box data.


[LINUX]

## ./how_to_run 🕹️

#### 1. ```sudo apt-get install libgl1-mesa-dev libgl1-mesa-glx libglu1-mesa-dev``` (if you don't have OpenGL)

#### 2. ```chmod +x ./mp4box-x86_64.AppImage```

#### 3. ```./mp4box-x86_64.AppImage --appimage-extract```

#### 4. ```cp text0.mp4 squashfs-root/ && cd squashfs-root/```

#### 5. ```./mp4box```

```TESTED ON:``` Ubuntu --> 22.04.3 LTS (jammy)/ 20.04.4 LTS, Kali GNU/Linux Rolling 2023.2, Parrot OS 5.3 (Electro Ara)

## How it works 🛠️

This application is written in C++ using Qt library. I chose Qt because it is a cross-platform application framework that is widely used for developing application software and it's compatible with various software and hardware platforms, with little or no change in the underlying codebase. In this case i do not use a GUI, i just use Qt for its classes and functions.

My first approach was to use ```QFile``` class to read the file and then a ```QByteArray``` variable to store 8 bytes at a time and print each box entirely to see the content. I noticed something like this: ```\x00\x00\x00\xB5moof``` with the first 4 bytes representing the size (each exadecimal couple is a byte) of the box and the next 4 the type of the box; and something a little bit different: ```"uuidm\x1D\x9B\x05"``` or ```"\xF4mdat<?x"```. So i decided to use 2 variables where ```counter``` is a counter that increments by 8 each time i read the file and the second one, ```next_byte``` is a second counter that increments its value by the size of a box only when i found a string inside the box that corresponds to the box type (moof, mfhd, traf etc..): I start reading the file and if ```counter >= next_byte``` (at first they are both 0) it means that the program is about to read a box with the "type" string inside it. After a box that contains a "type", except for MOOF and TRAF, there are 8 more "content" bytes that i don't need to print so i skip them thanks to the control guard. If it's "true" i check for the "type" inside box and if matches i do a second check ```if (!(next_byte % 8))``` inside ```save_box``` function to see if i have 8 "normal" bytes like this "\x00\x00\x00\x18tfhd" (in this case i compute the first 4 bytes as a decimal number (Big Endian integer), the size, and the last 4 as a string, the type) or odd bytes like ```\xF4mdat<?x"```. In this second case scenario ```count > next_byte``` therefore i've read more bytes than it was expected so a "piece" of box-size is in the previous box, but fortunately i forgot to mention that i have also a ```prev_box``` variable which contains the previous box, the previous 8 bytes. 
```Example:``` i found "\x00\x00\x00=uuid" at ```next_byte``` 120 and its size is 61, so after 61 bytes i'm gonna find the next type-box with the size, but 120 + 61 = 181 is NOT a multiple of my 8-bytes-reading-action, in fact ```counter``` value at that point is 192 (i've read 192 bytes since the start) but my box-size is settled at 181, at the previous box. Inside ```save_box``` function i check in my current box where is type-box, so inside ```\xF4mdat<?x``` mdat is 1 bytes after the start of the box and, since i know size is made out of 4 bytes, i have 3 bytes in the previous box ("\x00\x00""E"), and one in the current, this byte: ```\xF4```. I compute box size creating a new QByteArray object where i "add" the two parts (3 + 1 bytes) toghether. Then ```next_byte += size``` to know how many bytes i have to skip to find the next box: in case of "moof" or "traf" i add 8 not size because they have no content but boxes. At the end i print timestamp, box type and box size.


```Box:  "\x00\x00\x00=uuid"```

```Box:  "\xD4\x80~\xF2\xCA""9F\x95"```

```Box:  "\x8ET&\xCB\x9E""F\xA7\x9F"```

```Box:  "\x01\x00\x00\x00\x02\x00\x00""0"```

```Box:  "\xAF""f\xAC\xA3\x00\x00\x00\x00"```

```Box:  "\x00\x01""1-\x00\x00\x00""0"```

```Box:  "\xAFg\xDD\xD0\x00\x00\x00\x00"```

```Box:  "\x00\x01""1-\x00\x00\x00""E"```

```Box:  "\xF4mdat<?x"```

When i find MDAT box i break the while and i start reading the file one line at a time ('\n' for the end of the line), so i print the content of MDAT box and whenever i find a line that starts with "imagetype=", with ```extract_images``` function i start saving each line into a string object. When "</smpte:image>" comes up i stop saving lines into string and i decode the base64 string and save the image into a file.

#### Which problem can occur if the content of the MDAT box is very large?

In this case scenario i can't read it all at once and i can't save it into a variable because it would be too big. Read file one line at a time and save each line into a string object would be a smart choice BUT also this approach has its own limits: The main problem is related to handling large files in the terminal cause of losing visibility of previously printed content due to the limited ability of the terminal to maintain message history. This is more of an issue related to the development environment or terminal user interface than the code itself. To address this problem, i could use pauses or prompts to allow the user to manually scroll through the content of the file or maybe i could use a GUI to display the content of the file or i could write the file content to a log file.

## How to compile 📦

If you want to compile this application you need to install ```Qt library```, ```g++``` and ```cmake```, then you can edit ```CMakeLists.txt``` file with your own path to Qt library and your own requirements.

Run ```cmake .``` and ```make``` commands.

### Deploy 🚀

For deployment i used ```linuxdeployqt``` tool, you can find it here: 
https://github.com/probonopd/linuxdeployqt
