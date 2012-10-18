@ECHO OFF

ECHO Select the type of project you would like to create:
ECHO 1. Visual Studio 2010 Solution
ECHO 2. Visual Studio 2008 Solution
ECHO 3. Visual Studio 2005 Solution
ECHO 4. Visual Studio 2003 Solution
ECHO 5. Visual Studio 2002 Solution
ECHO 6. Code::Blocks Project
ECHO 7. CodeLite Project
ECHO 8. GNU Makefile

CHOICE /N /C:12345678 /M "[1-8]:"

IF ERRORLEVEL ==8 GOTO EIGHT
IF ERRORLEVEL ==7 GOTO SEVEN
IF ERRORLEVEL ==6 GOTO SIX
IF ERRORLEVEL ==5 GOTO FIVE
IF ERRORLEVEL ==4 GOTO FOUR
IF ERRORLEVEL ==3 GOTO THREE
IF ERRORLEVEL ==2 GOTO TWO
IF ERRORLEVEL ==1 GOTO ONE
GOTO END

:EIGHT
 ECHO Creating GNU Makefile...
 dist\bin\premake4.exe gmake
 GOTO END
:SEVEN
 ECHO Creating CodeLite Project...
 dist\bin\premake4.exe codelite
 GOTO END
:SIX
 ECHO Creating Code::Blocks Project...
 dist\bin\premake4.exe codeblocks
 GOTO END
:FIVE
 ECHO Creating VS2002 Project...
 dist\bin\premake4.exe vs2002
 GOTO END
:FOUR
 ECHO Creating VS2003 Project...
 dist\bin\premake4.exe vs2003
 GOTO END
:THREE
 ECHO Creating VS2005 Project...
 dist\bin\premake4.exe vs2005
 GOTO END
:TWO
 ECHO Creating VS2008 Project...
 dist\bin\premake4.exe vs2008
 GOTO END
:ONE
 ECHO Creating VS2010 Project...
 dist\bin\premake4.exe vs2010
 GOTO END

:END
