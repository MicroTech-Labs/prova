rem .c - .tco
cf -mr15:9:8 %1

rem fpc %1 lib

rem .tco - .mac
cg -r800 %1

rem .mac - .rel
m80 =%1/z

del %1.tco
rem del %1.mac

copy %1.rel ..\REL
del %1.rel
