@echo OFF

	echo.                         
	echo "  _______      _______                _ _ _     "
	echo " |  __ \ \    / / ____|              | (_| |    "
	echo " | |__) \ \  / | (___  _ __   ___ ___| |_| |__  "
	echo " |  ___/ \ \/ / \___ \| '_ \ / _ / __| | | '_ \ "
	echo " | |      \  /  ____) | | | |  __\__ | | | |_) |"
	echo " |_|       \/  |_____/|_| |_|\___|___|_|_|_.__/ "
 	echo.
	echo.                        
	echo This batch script helps to build PVSNESLIB                            
	echo.
	echo (1) When run the DEVKITSNES and DEVKIT65XX EnvVars will be configured  
	echo     temporarily for the local directory.                              
	echo (2) PVSNESLIB clean script will be launched and then PVSNESLIB will be
 	echo.
 	echo  Based on SGDK (https://github.com/Stephane-D/SGDK) build script. 

:START
    echo.
    echo.
    echo.
    echo 1) Starting:
    echo ---------------------------------------------------
    set "szscript=%~f0"
    echo Now running %szscript%
    echo.
		
		setlocal

:CREATEENVVARS
    echo.
    echo 2) Creating EnvVars:
    echo ---------------------------------------------------
		SET "DEVKITSNES=%cd:\=/%"
		SET "DEVKIT65XX=%cd:\=/%/devkitsnes"

    echo DEVKITSNES is %DEVKITSNES%
    echo DEVKIT65XX is %DEVKIT65XX%
		
:CHECKINGPATH
    echo.
    echo 3) Checking PATH:
    echo ---------------------------------------------------
    set "PATHREAL=%PATH%"
    echo PATH = %PATH%
    echo.
		
:LAUNCHING
    echo.
    echo 4) Launching:
    echo ---------------------------------------------------
    make -f Makefile clean
		if %ERRORLEVEL% == 0 goto :LAUNCH1
		echo Errors encountered during execution.  Exited with status: %ERRORLEVEL%
		goto :CLEANUP
		
:LAUNCH1
    echo.
    echo ---------------------------------------------------
		make -f Makefile release
		if %ERRORLEVEL% == 0 goto :LAUNCH2
		echo Errors encountered during execution.  Exited with status: %ERRORLEVEL%
		goto :CLEANUP

:LAUNCH2
    echo.
    echo ---------------------------------------------------
		make -f Makefile docs
		if %ERRORLEVEL% == 0 goto :CLEANUP
		echo Errors encountered during execution.  Exited with status: %ERRORLEVEL%
		goto :CLEANUP
		
:CLEANUP
    echo.
    echo 6) Done. Cleaning up.
    echo ---------------------------------------------------
    echo.
    echo Reseting PATH.
    set "PATH=%PATHREAL%"
    echo PATH = %PATH%

:END
    echo.
		echo PVSneslib compilation is over...
    endlocal
    @echo ON
