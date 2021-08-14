#!bash
cd /home/origami/code/bisheng/SysY2021/src

cd frontend
flex lexer.lex
bison -d parser.y 
cd ..

python ../scripts/ast_gen.py

# ref: https://unix.stackexchange.com/a/19656
for c_file in $(find . | grep ".*\.c$") 
do
    gcc -std=c11 -Wall -c -I$PWD -o ${c_file%.c}.o $c_file
done

gcc -std=c11 -Wall $(find . | grep ".*\.o$") -o ../ssyc