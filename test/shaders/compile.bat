@ECHO OFF

CD shaders

SET list=flat.vert tilemap1d.vert checker.frag gradient.frag palette.frag gfx.frag obj.frag tilemap.frag
(FOR %%a in (%list%) do (
	CALL :Compile %%a
))
EXIT /B 0

:Compile
ECHO Beginning compilation of %~1
C:/VulkanSDK/1.3.216.0/Bin/glslc.exe %~1 -w -O -o %~1.spv
IF %ERRORLEVEL% NEQ 0 (
	ECHO Could not compile shader: %~1
	ECHO.
) ELSE (
	ECHO Compiled shader %~1 as %~1.spv
	ECHO.
)

EXIT /B %ERRORLEVEL%