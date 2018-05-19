@ECHO OFF

ECHO Select the type of project you would like to create:
ECHO 1. Visual Studio 2015 Solution
ECHO 2. Visual Studio 2013 Solution
ECHO 3. Visual Studio 2012 Solution
ECHO 4. Visual Studio 2010 Solution
ECHO 5. Visual Studio 2008 Solution
ECHO 6. Visual Studio 2005 Solution
ECHO 7. Visual Studio 2003 Solution
ECHO 8. Visual Studio 2002 Solution
ECHO 9. GNU Makefile

CHOICE /N /C:123456789 /M "[1-9]:"

IF ERRORLEVEL ==9 GOTO NINE
IF ERRORLEVEL ==8 GOTO EIGHT
IF ERRORLEVEL ==7 GOTO SEVEN
IF ERRORLEVEL ==6 GOTO SIX
IF ERRORLEVEL ==5 GOTO FIVE
IF ERRORLEVEL ==4 GOTO FOUR
IF ERRORLEVEL ==3 GOTO THREE
IF ERRORLEVEL ==2 GOTO TWO
IF ERRORLEVEL ==1 GOTO ONE
GOTO END

:NINE
 ECHO Creating GNU Makefile...
 ..\bin\premake5.exe gmake
 GOTO END
:EIGHT
 ECHO Creating VS2002 Project...
 ..\bin\premake5.exe vs2002
 GOTO END
:SEVEN
 ECHO Creating VS2002 Project...
 ..\bin\premake5.exe vs2003
 GOTO END
:SIX
 ECHO Creating VS2003 Project...
 ..\bin\premake5.exe vs2005
 GOTO END
:FIVE
 ECHO Creating VS2005 Project...
 ..\bin\premake5.exe vs2008
 GOTO END
:FOUR
 ECHO Creating VS2008 Project...
 ..\bin\premake5.exe vs2010
 GOTO END
:THREE
 ECHO Creating VS2010 Project...
 ..\bin\premake5.exe vs2012
 GOTO END
:TWO
 ECHO Creating VS2012 Project...
 ..\bin\premake5.exe vs2013
 GOTO END
:ONE
 ECHO Creating VS2013 Project...
 ..\bin\premake5.exe vs2015
 GOTO END

:END
