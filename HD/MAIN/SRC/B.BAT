echo --- %1 ---

rem .c - .tco
rem cf -mr15:9:8 %1    come l'ho trovato
cf -mr15:10:8 %1

rem fpc %1 lib

rem .tco - .mac
rem cg -r1600 %1        come l'ho trovata
rem 1300 senza debug, 1000 con XMDBG
cg -r1000 %1

rem .mac - .rel
m80 =%1/z

del %1.tco
rem del %1.mac

copy %1.rel ..\REL
del %1.rel
































































