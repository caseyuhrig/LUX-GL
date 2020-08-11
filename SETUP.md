# Visual Studio 2019 Setup Instructions

1) Add Include Directories
   * __Configuration Properties >> VC++ Directories >> Include Directories__
   * $(ProjectDir)src\vendor
   * $(SolutionDir)Dependencies\irrKlang\include
   * $(SolutionDir)Dependencies\FT\include
   * $(SolutionDir)Dependencies\GLFW\include
1) Add Linking Directories
   * __Configuration Properties >> Linker >> General >> Additional Library Directories__
   * $(SolutionDir)Dependencies\irrKlang\lib-Winx64-visualStudio 
   * $(SolutionDir)Dependencies\FT\win64 
   * $(SolutionDir)Dependencies\GLFW\lib-vc2019 
1) Link opengl32.lib + glfw3.lib
   * __Configuration Properties >> Linker >> Input >> Additional Dependencies__
1) Add _CRT_SECURE_NO_WARNINGS
   * __Configuration Properties >> C/C++ >> Preporocessor >> Preprocessor Definitions__
1) Use /std:c++17 to enable C++17 standard-specific features and behavior.
   * __Configuration Properties >> General >> C++ Language Support __
1) Post build event
   * __Configuration Properties >> Build Events >> Post-Build Event >> Command Line__
   xcopy /y "$(SolutionDir)Dependencies\irrKlang\dll-Win64-visualStudio\*.dll" "$(OutDir)"
