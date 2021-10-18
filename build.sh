#!bash
cd /home/origami/code/bisheng/SysY2021/src

bash ../clear.sh normal

cd frontend
flex lexer.lex
bison -d parser.y 
cd ..

python ../scripts/ast_gen.py

if [ $1 == "production" ]
then
    addition_flag="-O2"
else
    addition_flag="-Wall -g -DDEBUG"
fi

# ref: https://unix.stackexchange.com/a/19656
for c_file in $(find . | grep ".*\.c$") 
do
    gcc -std=c11 ${addition_flag} -c -I$PWD -o ${c_file%.c}.o $c_file
done

gcc -std=c11 ${addition_flag} $(find . | grep ".*\.o$") -o ../ssyc