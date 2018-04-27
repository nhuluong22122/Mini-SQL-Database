#!/bin/sh

exec > test.out 2>&1         #redirect all output to test.out

echo TEST1.sh - test driver for CS157B project
echo Compile your code
gcc db.cpp -m32 -o db         #-m32 for 32bit code
echo Clean up
rm dbfile.bin *.tab
echo ==========================================================================================
echo 01 Test setup - create table + insert data
./db "create table class ( Student_Name char(20) NOT NULL, Gender char(1), Exams int, Quiz_Total int, Total int NOT NULL)"
echo ==========================================================================================
echo 02 Insert 15 rows of data
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
./db "insert into class values ('Melody', 'F', 560, 83, 633)"
./db "insert into class values ('Travis', 'M', 600, 75, 534)"
./db "insert into class values ('Sherry', 'F', 610, 94, 647)"
./db "insert into class values ('Hector', 'M', 590, 86, 482)"
./db "insert into class values ('Natalie', 'F', 500, 54, 596)"
echo ------------------------------------------------------------------------------------------

echo 01: Simple select * & verify headings & verify NULL display with 
./db "select * from class"

echo ==========================================================================================
echo 02: Delete from class where Student_Name = 'Bad_Student'
./db "delete from class where Student_Name = 'Stephen'"
./db "select * from class"
echo delete from class where Student_Name = 'Stephen'

echo ==========================================================================================
echo 03: Delete from class where no row is found
./db "detele from class where Student_Name = 'Hi'"
./db "select * from class"
echo delte from class where Student_Name = 'Hi'
echo ==========================================================================================
echo 04: Delete multi-row 3 rows
./db "delete from class where Total < 75"
./db "select * from class"
echo delete from class where Total < 75


echo ==========================================================================================
echo 05: Test single row update, e.g. update class set Quiz_Total=350 where Student_Name=’David’.
./db "update class set Quiz_Total = 350 where Student_Name = 'Natalie'"
./db "select * from class"
echo update class set Quiz_Total = 350 where Student_Name = 'Natalie'
echo ==========================================================================================
echo 06: Test update when no rows is found
./db "update class set Quiz_Total = 350 where Student_Name = 'Nhu'"
./db "select * from class"
echo update class set Quiz_Total = 350 where Student_Name = 'Nhu'
echo ==========================================================================================
echo 07: Multi-row update 4 rows
./db "update class set Quiz_Total = 350 where Quiz_Total > 95" 
./db "select * from class"
echo update class set Quiz_Total = 350 where Quiz_Total > 95
