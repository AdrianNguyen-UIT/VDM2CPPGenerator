# VDM2CPPGenerator
* [https://github.com/AdrianNguyen-UIT/VDM2CPPGenerator](https://github.com/AdrianNguyen-UIT/VDM2CPPGenerator)
* A Formal Specification project at university.
* This project is capable of generating a **VDM_SL function** ([see notes below](#Note-for-VDM_SL)) to a fully executable C++ solution.

## Instruction
* Clone or download zip files at [https://github.com/AdrianNguyen-UIT/VDM2CPPGenerator](https://github.com/AdrianNguyen-UIT/VDM2CPPGenerator)
* Run ***GenProject.bat*** file to generate Visual Stuio 2019 solution.
* You can choose the version of Visual Studio you want by modidying the ***GenProject.bat*** as below.
* The original of ***GenProject.bat*** looks like this.
<pre>call vendor\premake\premake5.exe vs2019
PAUSE</pre>
* Replace *vs2019* with [here](https://premake.github.io/docs/Using-Premake#using-premake-to-generate-project-files)

## Images

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
post r*r = x and r>=0</pre>

#### VDM_SL (in project)
<pre>SQRTP (x:R) r:R
pre x >=0
post x = r*r && r >= 0</pre>
