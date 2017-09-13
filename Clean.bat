del Engine\Binaries\*.* /F /S /Q /AH
del Engine\Intermediate\*.* /F /S /Q /AH
rd Engine\Binaries /S /Q
rd Engine\Intermediate /S /Q
del *.sln /F /S /Q
pause