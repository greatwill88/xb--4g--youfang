call tools\core_launch.bat
cd out\appimage_release
if "%1"=="" (cmake ..\.. -G Ninja -U BUILD_CUST_PROJ) else (cmake ..\.. -G Ninja -D BUILD_CUST_PROJ=%1)
ninja
ninja >..\..\ log.txt
cd hex
copy /Y /B  ..\..\..\tools\win32\target-upgrade.xml target-upgrade.xml
nwy_mkpackage.exe
cd ..\..\..\
copy out\appimage_release\hex\hello_flash.pac Z:\share_File\
copy out\appimage_release\hex\hello_flash.elf Z:\share_File\
copy out\appimage_release\hex\app.bin Z:\share_File\
pause

