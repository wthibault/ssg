This folder has projects for CodeBlocks that build and run the ssg examples.
The projects all include the ssg source code, to encourage exploration and experimentation.

You need to have codeblocks, glut, and glew installed on your system.
Glew is included in ssg along with a library built for codeblocks, so you don't need to download glew,
although there is some manual file copying required (see below).

=======================
Get Code::Blocks at 
http://www.codeblocks.org
Be sure to get the version that includes the "mingw" compiler (has mingw in the filename).
We'll assume you installed it at the default location, C:\Program Files (x86)\CodeBlocks

Get the glut binaries for win32 at:
http://www.xmission.com/~nate/glut.html
(use the first download link)

After you download and open the GLUT bin zip file, you will need to:

Copy glut32.dll to c:\windows\system,
Copy glut32.lib to c:\program files (x86)\CodeBlocks\mingw\lib, and
Copy glut.h     to c:\program files (x86)\mingw\include\GL

(These are default locations, your paths may be different. But basically, you place the .lib and .h files in the location of your compiler (in this case the mingw g++ compiler that comes with codeblocks). The .dll file goes in the windows system directory or something on your path.)

Now, copy the glew includes, library and dll into place from the ssg installation:

Copy <ssg-install-dir>\win32\ssg-lib-codeblocks\glew-1.10.0\lib\glew32.dll             to C:\windows\system
Copy <ssg-install-dir>\win32\ssg-lib-codeblocks\glew-1.10.0\lib\libglew32.dll.a        to c:\program files (x86)\CodeBlocks\MINGW\lib
Copy the .h files in <ssg-install-dir>\win32\ssg-lib-codeblocks\glew-1.10.0\include\GL to c:\Program Files (x86)\CodeBlocks\MINGW\include\GL


=====================
ALSO IMPORTANT: 
Set your SSG_DATA environment variable to the "data" folder of your ssg installation.
If you run everything from within Codeblocks, then in Codeblocks navigate to Settings...Environment...Environment Variables...
and click on the "Add" button to create a new environment variable with "key" SSG_DATA and "value" the location of your ssg install's "data" folder. 
If you run programs from the prompt, you can set SSG_DATA in Windows from Computer..... System Properties...Environment Variables.

=====================

You can try all the examples by opening example0.cbp in codeblocks and removing example0.cpp from the project,
then adding a different example cpp file from <ssg-install-dir>/examples.

To create a new ssg project:

If the examples are building and running for you, (which means you installed codeblocks and the rest as assumed above)
1. You can make a copy of one of the "example" projects IN THE SAME FOLDER
2. and then replace "exampleX.cpp" with your cpp source file(s).

OR

1. Create a new "GLUT project" in codeblocks.  When it asks for where glut is installed, use the codeblocks compiler directory:
   C:\Program Files (x86)\CodeBlocks\MINGW

2. add the ssg source code to the project:  in CodeBlocks: Project... Add Files Recursively...  
   (select the "src" folder in your ssg installation, be sure to *** leave stb_image.c and Makefile unchecked ***!!!)

3. replace main.cpp with your ssg-based cpp file.

4. Add the glew library to the project. Project... Build Options.... then the Linker Settings tab.
    BE SURE YOU HAVE YOUR PROJECT SELECTED in the left side of the dialog box.
    Click the "Add" button and type in  "libglew32.dll.a" (without the quotes)

5. Setup codeblocks to find the ssg header files:
   goto the "Search Directories" tab under Project...Build Options...
   Use the Add button to add <ssg-install-dir>\include

================================

Some ssg examples need command line arguments.  To supply them via CodeBlocks:
Project...Set program's arguments...
and then enter arguments in the "Program arguments" box, such as:
objfiles/cube.obj

BE SURE you've set the SSG_DATA environment variable to the location of the ssg "data" folder.

