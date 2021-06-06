# VDM2CPPGenerator
* [https://github.com/AdrianNguyen-UIT/VDM2CPPGenerator](https://github.com/AdrianNguyen-UIT/VDM2CPPGenerator)
* A Formal Specification project at university.
* This project is capable of generating a **VDM_SL function** ([see notes below](#Note-for-VDM_SL)) to a fully executable C++ solution.
* Using OpenGL for rendering, [glfw + glad](#Credits) for manipulating OpenGL window, [Dear ImGui](#Credits) for GUI and [Premake](#Credits) for generating project.
* The core parser is inspired by [uliwitness](https://github.com/uliwitness/simpleparser), [link](https://www.youtube.com/user/uliwitness) to his Youtube channel.

## Instruction
* Clone (working with submodules) at [https://github.com/AdrianNguyen-UIT/VDM2CPPGenerator](https://github.com/AdrianNguyen-UIT/VDM2CPPGenerator) or execute these commands **line by line** as below.<br>
<pre>git clone https://github.com/AdrianNguyen-UIT/VDM2CPPGenerator
cd VDM2CPPGenerator/
git submodule init
git submodule update</pre>
* Run ***GenProject.bat*** file to generate Visual Studio 2019 solution.
* You can choose the version of Visual Studio you want by modidying the ***GenProject.bat*** as below.
* The original of ***GenProject.bat*** looks like this.
<pre>call vendor\premake\premake5.exe vs2019
PAUSE</pre>
* Replace *vs2019* with [here](https://premake.github.io/docs/Using-Premake#using-premake-to-generate-project-files)

## Images
Start up window
![Start up window](https://github.com/AdrianNguyen-UIT/VDM2CPPGenerator/blob/main/images/image1.png)

Read Input and Update Input Editor Window
![Read Input window](https://github.com/AdrianNguyen-UIT/VDM2CPPGenerator/blob/main/images/image2.png)

Parse, Update Output Editor Window and Update Token window
![Parse window](https://github.com/AdrianNguyen-UIT/VDM2CPPGenerator/blob/main/images/image3.png)


## Credits
* [Premake](https://github.com/premake/premake-core)
* [glfw](https://github.com/glfw/glfw)
* [glad](https://github.com/Dav1dde/glad)
* [Dear ImGui](https://github.com/ocornut/imgui)
* [ImGuiColorTextEdit](https://github.com/BalazsJako/ImGuiColorTextEdit)
* [spdlog](https://github.com/gabime/spdlog)
* [IconFontCppHeaders](https://github.com/juliettef/IconFontCppHeaders)

## Note for VDM_SL
Because this is a university project for assignment, there are some modifications to the data type keywords and syntaxes to meet the requirements of the instructor.

### Data type keywords
|Data types      | VDM_SL (release)              |VDM_SL (in project)          |
|----------------|-------------------------------|-----------------------------|
|Boolean         |bool                           |B                            |
|Natural Numbers |nat                            |N                            |
|Integers        |int                            |Z                            |
|Real Numbers    |real                           |R                            |
|String          |char*                          |char*                        |

### Syntax
#### VDM_SL (release)
<pre>SQRTP (x:real) r:real
pre x >=0
post r*r = x and r >= 0</pre>

#### VDM_SL (in project)
<pre>SQRTP (x:R) r:R
pre x >=0
post x = r*r && r >= 0</pre>
