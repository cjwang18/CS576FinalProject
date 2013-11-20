
You will find
	1) A program to display and manipulate images. This has been
given as a Microsoft Visual C++ .dsp and .dsw project file along with
the accompanying code in the .cpp and .h files
	2) Example images, which are in the native RGB formats. They are
as follows with dimensions and brief description
image1.rgb -> 640x480 shows a weather map
image2.rgb -> 640x480 shows an expanding spiral
image3.rgb -> 640x480 shows an image of text
image4.rgb -> 352x288 shows a construction site
image5.rgb -> 480x360 shows a city by night
image6.rgb -> 320x240 shows a few buildings with clouds
image7.rgb -> 512x512 shows an example of a gray (not color) image

Each image is different in the colors, frequency content etc. used and should
server as good examples for playing with subsampling, color space transformations,
quantization, compression etc.

In the program, there is code to "modify" and image - also can be invoked through
the main menu. The student then has to write code to appropriate fill up the
"MyImage::Modify()" function in the file Image.cpp


Unzip the folder to where you want.
Launch Visual C++ and load the .dsw or .dsp project file.
If using the .net compiler, you can still load the project file, just 
follow the default prompts and it will create a solution .sln file for you.
Compile the program to produce a an executable Image.exe

To launch it you have to give it command line arguments, which can be provided
in Visual C++ under Project > Settings or just launched on command line.
To use it with the example images - here is the usage

Image.exe image1.rgb 640 480
Image.exe image2.rgb 640 480
Image.exe image3.rgb 640 480
Image.exe image4.rgb 352 288
Image.exe image5.rgb 480 360
Image.exe image6.rgb 320 240
Image.exe image7.rgb 512 512