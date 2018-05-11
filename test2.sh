#!/bin/sh

exec > test.out 2>&1         #redirect all output to test.out

echo TEST2.sh - test driver for CS157B project inner join
echo Compile your code
gcc db.cpp -m32 -o db         #-m32 for 32bit code
echo Clean up
echo ==========================================================================================
echo #01 Test setup - create table + insert data
./db "create table schedule ( Student char(12) NOT NULL, class char(12) )"
echo ------------------------------------------------------------------------------------------
echo #02 Insert 15 rows of data
./db "insert into schedule values ('Siu', 'CS156')"
./db "insert into schedule values ('Frank', 'CS174')"
./db "insert into schedule values ('Jordon', 'CS160')"
echo ------------------------------------------------------------------------------------------
echo #ls -al verify file sizes
ls -al
echo ------------------------------------------------------------------------------------------
echo #03 & #04 Simple select * & verify headings & verify NULL display with -
./db "select * from schedule"
echo ------------------------------------------------------------------------------------------
