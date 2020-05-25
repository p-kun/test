@echo off
doskey ..=cd ..
doskey ll=dir /OENG /Q /A-
doskey ls=dir /W
doskey pp=dir /W
doskey build=msbuild -m -v:m
doskey rebuild=msbuild -m -v:m -t:Rebuild
doskey release=msbuild -m -v:m -t:Rebuild -p:Configuration=Release
rem doskey ssh=ssh -i c:\Users\rsp95\.ssh\id_rsa ab@192.168.56.101
@SET PROMPT=$T$H$H$H [$P]$$ 

rem call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

@SET Path=%PATH%;C:\Program Files\Git\bin;

cd c:\user\Products
