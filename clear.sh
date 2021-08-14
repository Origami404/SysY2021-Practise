#!bash
cd /home/origami/code/bisheng/SysY2021/src

rm $(find . | grep ".*\.o")
rm $(find . | grep ".*\.inl")
rm frontend/parser.tab.c frontend/parser.tab.h frontend/lex.yy.c
rm ../ssyc