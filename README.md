# browser

browser based on CEF

## build requirements
 - Visual Studio 2017 15.9
 - Windows SDK 10.0.17763.0
 - Chromium Embedded Framework ([cef_binary_78.3.4+ge17bba6+chromium-78.0.3904.108_windows32](http://opensource.spotify.com/cefbuilds/index.html))

## how to build and execute the project
 1. build solution in MSVC. (Ctrl + Shift + B)
 2. copy files for CEF into the build output directory.
    - copy the same configuration as the output.
        - Debug & Release : copy all files in `thirdparty\cef\x86\Resources`
        - Debug : copy all files in `thirdparty\cef\x86\Debug`
        - Release : copy all files in `thirdparty\cef\x86\Release`
        - you don't have to copy `.lib` files in the directory, as you know.
 3. make `test.html` in the output directory.
    - browser program needs the file for execution.

## future
 1. batch file for execution will be added.
 2. x64 platform configuration will be added.
 3. support for local HTML file will be improved.