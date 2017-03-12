robocopy "%1rsc/shaders" "%2rsc/shaders" *.hlsl > log:nul
IF %ERRORLEVEL% GEQ 8 exit 1
exit 0