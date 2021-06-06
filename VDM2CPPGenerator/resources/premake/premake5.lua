workspace "{SolutionName}"
    configurations {
        "Debug", 
        "Release"
    }
    
project "{ProjectName}"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    files {
        "**.h",
        "**.cpp"
    }
    
    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"



