#!/bin/sh

exec > test.out 2>&1         #redirect all output to test.out

echo TEST1.sh - test driver for CS157B project
echo Compile your code
gcc db.cpp -m32 -o db         #-m32 for 32bit code
echo Clean up
rm dbfile.bin *.tab
echo ==========================================================================================
echo 01 Test setup - create table + insert data
./db "create table class ( Student char(20) NOT NULL, Gender char(1), Exams int, Quiz_Total int, Total int NOT NULL)"
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
echo END FUNCTIONAL QUERY - BEGIN ERROR CHECKING
echo ==========================================================================================
echo ==========================================================================================
echo INSERT ERROR
echo ==========================================================================================
echo "Check syntax errors in various INSERT statement combinations"
echo "insert into class values ('Siu', M, 480, 80, 560)"
./db "insert into class values ('Siu', M, 480, 80, 560)"
echo
echo "insert into class values ('Zeno', 600, 100, 700)"
./db "insert into class values ('Zeno', 600, 100, 700)"
echo
echo "insert into class values ('Zeno', M, '600', 100, 700)"
./db "insert into class values ('Zeno', M, '600', 100, 700)"
echo
echo "insert into table values"
./db "insert into table values"
echo
echo "insert into class values ('Zeno', 'M', 600, 100, 700) hi"
./db "insert into class values ('Zeno', 'M', 600, 100, 700) hi"
echo ==========================================================================================
echo UPDATE ERROR
echo ==========================================================================================
echo "Check syntax errors in various UPDATE statement combinations"
echo "update hi SET Student_Name = 'Hi'"
./db "update hi SET Student_Name = 'Hi'"
echo
echo "update class SET Student_Name = Hi"
./db "update class SET Student_Name = Hi"
echo
echo "update class Student_Name = NULL"
./db "update class Student_Name = NULL"
echo
echo "update class SET Student_Name = 'Hi' hi"
./db "update class SET Student_Name = 'Hi' hi"
echo
echo "update class SET Student_Name = 'Three' WHERE"
./db "update class SET Student_Name = 'Three' WHERE"
echo
echo "update class SET Student_Name = Three WHERE Student_Name IS NULL"
./db "update class SET Student_Name = Three WHERE Student_Name IS NULL"
echo
echo "update class SET Student_Name = 'Three' WHERE Student_Name IS NOT NULL"
./db "update class SET Student_Name = 'Three' WHERE Student_Name IS NOT NULL"
echo
echo ==========================================================================================
echo SELECT ERROR
echo ==========================================================================================
echo "select * from"
./db "select * from"
echo
echo "select * from class WHERE"
./db "select * from class WHERE"
echo
echo "select * from class ORDER BY"
./db "select * from class ORDER BY"
echo
echo "select * from class WHERE Gender = M"
./db "select * from class WHERE Gender = M"
echo
echo "select * from class WHERE Gender = 'M' AND Student_Name = N"
./db "select * from class WHERE Gender = 'M' AND Student_Name = N"
echo
echo "select * from class WHERE Gender = 'M' BUT Student_Name = 'N'"
./db "select * from class WHERE Gender = 'M' BUT Student_Name = 'N'"
echo
echo "select * from class WHERE Gender = 'M' AND  = 'N'"
./db "select * from class WHERE Gender = 'M' AND  = 'N'"
echo
echo "SELECT * FROM CLASS WHERE STUDENT = 'HI'"
./db "SELECT * FROM CLASS WHERE STUDENT = 'HI'"
echo
echo "select * from class where Exams IS NULL or Gender = 'M' ORDER BY LOVE"
./db "select * from class where Exams IS NULL or Gender = 'M' ORDER BY LOVE"
echo
echo ==========================================================================================
echo MISMATCH ERROR ON INSERT
echo ==========================================================================================
echo "insert into class values ('Zeno', 'M', '590', 86, 482)"
./db "insert into class values ('Zeno', 'M', '590', 86, 482)"
echo
echo "insert into class values ('Zeno', M, 590, 86, 482)"
./db "insert into class values ('Zeno', M, 590, 86, 482)"
echo
echo ==========================================================================================
echo NOT NULL ERROR ON INSERT / UPDATE
echo ==========================================================================================
echo "insert into class values (NULL, 'M', '590', 86, 482)"
./db "insert into class values (NULL, 'M', '590', 86, 482)"
echo
echo "insert into class values ('Goku', 'M', '590', 86, NULL)"
./db "insert into class values ('Goku', 'M', '590', 86, NULL)"
echo
echo "update class SET Student_Name = 'Five' where Quiz_Total IS NULL"
./db "update class SET Student_Name = 'Five' where Quiz_Total IS NULL"
echo
echo ==========================================================================================
echo DATATYPE MISMATCH ON WHERE
echo ==========================================================================================
echo "delete from class where Student_Name = Zeno"
./db "delete from class where Student_Name = Zeno"
echo
echo "update class SET Student_Name = 'Five' where Quiz_Total = 'Nhu'"
./db "update class SET Student_Name = 'Five' where Quiz_Total = 'Nhu'"
echo
echo ==========================================================================================
echo INVALID DATA VALUE ERRORS
echo ==========================================================================================
echo "update class SET Student_Name = Four"
./db "update class SET Student_Name = Four"
echo
echo "update from class WHERE Student_Name = Four"
./db "update from class WHERE Student_Name = Four"
echo
echo ==========================================================================================
echo INVALID RELATIONAL OPERATOR
echo INVALID AGGREGATE FUNCTION PARAMETER
echo ==========================================================================================
echo "select AVG(Student_Name) from class"
./db "select AVG(Student_Name) from class"
echo
echo "select SUM(Student_Name) from class"
./db "select SUM(Student_Name) from class"
echo
echo "select TOTAL(Total) from class"
./db "select TOTAL(Total) from class"
echo
echo "select (Total) from class"
./db "select (Total) from class"
echo
echo "select SUM(*) from class"
./db "select SUM(*) from class"
echo
echo "select AVG(*) from class"
./db "select AVG(*) from class"
echo
echo "select * from class WHERE Student_Name ? 'Siu' Or Quiz_Total > 80 ORDER BY Quiz_Total"
./db "select * from class WHERE Student_Name ? 'Siu' Or Quiz_Total > 80 ORDER BY Quiz_Total"
echo
echo "delete from class WHERE Student_Name * 'Siu'"
./db "delete from class WHERE Student_Name * 'Siu'"
echo
echo "update class SET Exams = NULL WHERE Student_Name * 'Siu'"
./db "update class SET Exams = NULL WHERE Student_Name * 'Siu'"
echo
echo "update class SET Exams = NULL WHERE Student_Name * 'Siu'"
./db "update class SET Exams = NULL WHERE Student_Name * 'Siu'"
echo
