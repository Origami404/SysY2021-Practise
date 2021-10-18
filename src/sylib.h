#ifndef HEADER_SYLIB_H__
#define HEADER_SYLIB_H__

int getint();
int getch();
int getarray(int []);

void putint(int);
void putch(int);  
void putarray(int, int []);

void putf(char const *, ...);

void starttime();
void stoptime();

#endif // HEADER_SYLIB_H__