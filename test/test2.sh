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
./db "insert into class values ('euna', 'F', 620, 90, 535)"
./db "insert into class values ('Victor', 'M', 475, 60, 535)"
./db "insert into class values ('Linawati', 'F', 490, 86, 576)"
./db "insert into class values ('Stephen', 'M', 520, 76, 596)"
./db "insert into class values ('Melody', 'F', 560, 83, 633)"
./db "insert into class values ('Travis', 'M', 600, 75, 534)"
./db "insert into class values ('Sherry', 'F', 610, 94, 647)"
./db "insert into class values ('Hector', 'M', 590, 86, 482)"
./db "insert into class values ('Natalie', 'F', 500, 54, 596)"
echo ==========================================================================================
echo SIMPLE SELECT
echo ==========================================================================================
echo Select All
echo "Simple select & verify headings & verify NULL display with"
./db "select * from class"
echo
echo ------------------------------------------------------------------------------------------
echo Projection
echo "select Student_Name from class"
./db "select Student_Name from class"
echo
echo ------------------------------------------------------------------------------------------
echo Projection
echo "select Student_Name, Quiz_Total from class"
./db "select Student_Name, Quiz_Total from class"
echo

echo ==========================================================================================
echo DELETE
echo ==========================================================================================

echo Delete from class where Student_Name = 'Bad_Student'
echo delete from class where Student_Name = 'Stephen'
./db "delete from class where Student_Name = 'Stephen'"
./db "select * from class"
echo

echo ------------------------------------------------------------------------------------------
echo Delete from class where no row is found
echo "delete from class where Student_Name = 'Hi'"
./db "delete from class where Student_Name = 'Hi'"
./db "select * from class"
echo

echo ------------------------------------------------------------------------------------------
echo Delete multi-row 3 rows
echo "delete from class where Exams < 500"
./db "delete from class where Exams < 500"
./db "select * from class"
echo

echo ==========================================================================================
echo UPDATE
echo ==========================================================================================
echo "Test single row update, e.g. update class set Quiz_Total=350 where Student_Name=’David’"
echo "update class set Quiz_Total = 350 where Student_Name = 'Natalie'"
./db "update class set Quiz_Total = 350 where Student_Name = 'Natalie'"
./db "select * from class"
echo

echo ------------------------------------------------------------------------------------------
echo Test update when no rows is found
echo "update class set Quiz_Total = 350 where Student_Name = 'Nhu'"
./db "update class set Quiz_Total = 350 where Student_Name = 'Nhu'"
./db "select * from class"
echo
echo ------------------------------------------------------------------------------------------
echo Multi-row update 4 rows
echo "update class set Quiz_Total = 200 where Quiz_Total < 85"
./db "update class set Quiz_Total = 200 where Quiz_Total < 85"
./db "select * from class"
echo
echo ==========================================================================================
echo COMPLEX SELECT
echo ==========================================================================================
echo Test SELECT with WHERE clause with a single condition.
echo "select * from class where Exams < 600"
./db "select * from class where Exams < 600"
echo

echo Test SELECT with WHERE clause with a single condition.
echo "select Student_Name from class where Exams < 600"
./db "select Student_Name from class where Exams < 600"
echo
echo ------------------------------------------------------------------------------------------
echo "Test SELECT with WHERE clause for case sensitive comparison, e.g. Student_Name < 'Good_Student'"
echo "Nhu: There is a bug. I will fix mine to validate these stuffs earlier! "
echo "select * from class where Student_Name < 'Frank'"
./db "select * from class where Student_Name < 'Frank'"
echo
echo "select * from class where Student_Name = 'frank'"
./db "select * from class where Student_Name = 'frank'"
echo
echo ------------------------------------------------------------------------------------------
echo Test SELECT with WHERE clause with columns that is NULL and NOT NULL
echo "update class set Quiz_Total = NULL where Student_Name = 'Natalie'"
./db "update class set Exams = NULL where Student_Name = 'Natalie'"
echo
echo "select Student_Name, Gender from class where Exams IS NULL"
./db "select Student_Name, Gender from class where Exams IS NULL"
echo
echo ------------------------------------------------------------------------------------------
echo Test SELECT with two conditions separated by the AND keyword.
echo "select * from class where Total > 400 AND Exams IS NULL"
./db "select * from class where Total > 400 AND Exams IS NULL"
echo
echo ------------------------------------------------------------------------------------------
echo Test SELECT with two conditions separated by the OR keyword
echo "select * from class where Exams IS NULL OR Student_Name ='Frank'"
./db "select * from class where Exams IS NULL OR Student_Name ='Frank'"
echo
echo ------------------------------------------------------------------------------------------
echo Test SELECT with ORDER BY clause.
echo "select * from class ORDER BY Student_Name DESC"
./db "select * from class ORDER BY Student_Name DESC"
echo
echo "select * from class ORDER BY Student_Name"
./db "select * from class ORDER BY Student_Name"
echo
echo "select * from class ORDER BY Exams DESC"
./db "select * from class ORDER BY Exams DESC"
echo
echo "select * from class ORDER BY Exams"
./db "select * from class ORDER BY Exams"
echo
echo ------------------------------------------------------------------------------------------
echo Test SELECT with WHERE and ORDER BY clause.
echo "select * from class WHERE Student_Name = 'Siu' Or Quiz_Total > 80 ORDER BY Quiz_Total"
./db "select * from class WHERE Student_Name = 'Siu' Or Quiz_Total > 80 ORDER BY Quiz_Total"
echo
echo ------------------------------------------------------------------------------------------
echo "Test SELECT SUM() function."
echo "SELECT SUM(Exams) FROM CLASS"
./db "SELECT SUM(Exams) FROM CLASS"
echo
echo ------------------------------------------------------------------------------------------
echo "Test SELECT SUM() function with WHERE clause"
echo "SELECT SUM(Exams) from class WHERE Gender='F'"
./db "SELECT SUM(Exams) from class WHERE Gender='F'"
echo
echo ------------------------------------------------------------------------------------------
echo "Test SELECT AVG() function"
echo "SELECT AVG(Exams) FROM CLASS"
./db "SELECT AVG(Exams) FROM CLASS"
echo
echo "SELECT AVG(Total) FROM CLASS"
./db "SELECT AVG(Total) FROM CLASS"
echo
echo ------------------------------------------------------------------------------------------
echo "Test SELECT AVG() function with WHERE clause"
echo "SELECT AVG(Total) FROM CLASS WHERE GENDER = 'M'"
./db "SELECT AVG(Total) FROM CLASS WHERE GENDER = 'M'"
echo
echo "SELECT AVG(Total) FROM CLASS WHERE GENDER = 'f'"
./db "SELECT AVG(Total) FROM CLASS WHERE GENDER = 'f'"
echo
echo ------------------------------------------------------------------------------------------
echo "Test SELECT COUNT() function"
echo "SELECT COUNT(Exams) FROM CLASS"
./db "SELECT COUNT(Exams) FROM CLASS"
echo
echo ------------------------------------------------------------------------------------------
echo "Test SELECT COUNT() function with WHERE clause"
echo "SELECT COUNT(Exams) FROM CLASS WHERE GENDER = 'M'"
./db "SELECT COUNT(Exams) FROM CLASS WHERE GENDER = 'M'"
echo
echo ------------------------------------------------------------------------------------------
echo "Test SUM(), AVG() when there are NULLs"
echo "--------WHEN THERE ARE NULLS----------"
echo "SELECT SUM(Exams) FROM CLASS"
./db "SELECT SUM(Exams) FROM CLASS"
echo
echo "SELECT AVG(Exams) FROM CLASS"
./db "SELECT AVG(Exams) FROM CLASS"
echo
echo "UPDATE CLASS SET EXAMS = 600 WHERE STUDENT_NAME = 'Natalie'"
./db "UPDATE CLASS SET EXAMS = 600 WHERE STUDENT_NAME = 'Natalie'"
echo
echo "--------WHEN THERE ARE NO NULLS----------"
echo "SELECT SUM(Exams) FROM CLASS"
./db "SELECT SUM(Exams) FROM CLASS"
echo
echo "SELECT AVG(Exams) FROM CLASS"
./db "SELECT AVG(Exams) FROM CLASS"
echo
echo ------------------------------------------------------------------------------------------
echo "Test COUNT(*), COUNT(Quiz_Total) when there are NULLs. – this should not include NULLS"
echo "--------WHEN THERE ARE NO NULLS IN EXAM----------"
echo "SELECT COUNT(EXAMS) FROM CLASS"
./db "SELECT COUNT(EXAMS) FROM CLASS"
echo
echo "--------WHEN THERE ARE NULLS IN EXAM----------"
echo "UPDATE CLASS SET EXAMS = null WHERE STUDENT_NAME = 'Natalie'"
./db "UPDATE CLASS SET EXAMS = null WHERE STUDENT_NAME = 'Natalie'"
echo
echo "SELECT COUNT(EXAMS) FROM CLASS"
./db "SELECT COUNT(EXAMS) FROM CLASS"
echo
echo "SELECT COUNT(*) FROM CLASS"
./db "SELECT COUNT(*) FROM CLASS"
echo
echo ------------------------------------------------------------------------------------------
echo ==========================================================================================
echo END FUNCTIONAL QUERY
echo ==========================================================================================
