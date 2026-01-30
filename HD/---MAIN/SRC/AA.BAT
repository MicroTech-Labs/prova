rem /L per .prn, ad esempio m80 =asmlib/z/L

m80 =KBDSCAN/z
copy KBDSCAN.rel ..\REL
del KBDSCAN.rel

m80 =asmlib/z
copy asmlib.rel ..\REL
del asmlib.rel

m80 =JCA/z
copy JCA.rel ..\REL
del JCA.rel

m80 =Z/z
copy Z.rel ..\REL
del Z.rel
