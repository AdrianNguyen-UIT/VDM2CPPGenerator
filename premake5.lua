workspace "VDM2CPPGenerator"
    architecture "x64"
    startproject "VDM2CPPGenerator"

    configurations
    {
        "Debug", 
        "Release"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--Include Dir Table/Struct
IncludeDir = {}
IncludeDir["glfw"] = "Dependencies/glfw/include"
IncludeDir["Glad"] = "Dependencies/Glad/include"
IncludeDir["imgui"] = "Dependencies/imgui"
IncludeDir["VDMParser"] = "Dependencies/VDMParser/src"
IncludeDir["spdlog"] = "Dependencies/spdlog/include"
IncludeDir["ImGuiColorTextEdit"] = "Dependencies/ImGuiColorTextEdit"
IncludeDir["IconFontCppHeaders"] = "Dependencies/IconFontCppHeaders"

group "Dependencies"
    include "Dependencies/glfw"
    include "Dependencies/Glad"
    include "Dependencies/imgui"
    include "Dependencies/VDMParser"
group ""

project "VDM2CPPGenerator"
    location "VDM2CPPGenerator"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    targetdir ("bin/"..outputdir.."/%{prj.name}")
    objdir ("obj/"..outputdir.."/%{prj.name}")

    pchheader "pch.h"
    pchsource "%{prj.name}/src/pch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.VDMParser}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.ImGuiColorTextEdit}",
        "%{IncludeDir.IconFontCppHeaders}"
    }

    libdirs
    {
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
    }

    links
    {
        "glfw",
        "Glad",
        "imgui",
        "VDMParser",
        "opengl32.lib"
    }
    
    postbuildcommands { 'xcopy /Q /E /Y /I "%{prj.location}\\resources" "%{cfg.buildtarget.directory}\\resources"' }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"



