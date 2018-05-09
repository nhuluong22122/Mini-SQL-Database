#!/bin/sh

exec > test.out 2>&1         #redirect all output to test.out

echo TEST1.sh - test driver for CS157B project
echo Compile your code
gcc db.cpp -m32 -o db         #-m32 for 32bit code
echo Clean up
rm dbfile.bin *.tab
echo ==========================================================================================
echo #01 Test setup - create table + insert data
./db "create table class ( Student char(12) NOT NULL, Gender char(1), Exams int, Quiz int, Total int )"
echo ------------------------------------------------------------------------------------------
echo #02 Insert 15 rows of data
./db "insert into class values ('Siu', 'M', 480, 80, 560)"
./db "insert into class values ('Frank', 'M', 600, 100, 700)"
./db "insert into class values ('Jordon', 'M', 450, 75, 525)"
./db "insert into class values ('Jeff', 'M', 455, 60, 515)"
./db "insert into class values ('Ying', 'F', 540, 85, 625)"
./db "insert into class values ('David', 'M', 550, 83, 633)"
./db "insert into class values ('euna', 'F', 460, 75, 535)"
./db "insert into class values ('Victor', 'M', 475, 60, 535)"
./db "insert into class values ('Linawati', 'F', 490, 86, 576)"
./db "insert into class values ('Stephen', 'M', 520, 76, 596)"
echo ------------------------------------------------------------------------------------------
echo #ls -al verify file sizes
ls -al
echo ------------------------------------------------------------------------------------------
echo #03 & #04 Simple select * & verify headings & verify NULL display with -
./db "select * from class"
echo ------------------------------------------------------------------------------------------

echo ==========================================================================================
echo Error Testing Section
echo ------------------------------------------------------------------------------------------
echo Setup the table
./db "create table test (col1 char(10), col2 int, col3 int not null)"
./db "insert into test values ( 'one', 1, 11 )"
./db "insert into test values ( 'two', 2, 22 )"
./db "select * from test"
echo ------------------------------------------------------------------------------------------
echo #5 "syntax errors in various INSERT statement combinations"
echo
./db "insert into test ( 'not_null', 1, '11' )"
./db "insert into test values ( 'not_null', 1 )"
echo ------------------------------------------------------------------------------------------
echo #6 "data type mismatch"
echo
./db "insert into test values ( 'not_null', 1, 'string' )"
echo ------------------------------------------------------------------------------------------
echo #7 NOT NULL on inserts
echo
./db "insert into test values ( 'not_null', 1, NULL )"
echo ------------------------------------------------------------------------------------------
echo End of test1.sh
