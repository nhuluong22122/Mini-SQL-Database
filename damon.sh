echo ha ha noobie

exec > testnhu.out 2>&1         #redirect all output to test.out

echo "\n*\nyou have found the golden easter egg, oasis is yours\n*\n"

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
./db "insert into class values ('Test1', null, 560, 83, 633)"
./db "insert into class values ('Test2', 'M', null, 75, 534)"
./db "insert into class values ('Test3', 'F', 610, null, 647)"
./db "insert into class values ('Test4', 'M', 590, 86, 9999)"
echo ==========================================================================================
echo SELECT
echo ==========================================================================================
echo Select All
echo "Simple select & verify headings & verify NULL display with"
./db "select * from class"
echo
echo ------------------------------------------------------------------------------------------
echo Projection of two of the same columns
echo "select Student_Name, Student_Name from class"
./db "select Student_Name, Student_Name from class"
echo
echo ------------------------------------------------------------------------------------------
echo Projection of columns in different order
echo "select Total, Quiz_Total, exams, gender, Student_Name from class"
./db "select Total, Quiz_Total, exams, gender, Student_Name from class"
echo
echo ------------------------------------------------------------------------------------------
echo Error Testing **THESE SHOULD ALL BE BLOCKED**
echo "select * from class where gender = 123"
./db "select * from class where gender = 123"
echo

echo "select * from class where exams = hi"
./db "select * from class where exams = hi"
echo

echo "select Total, Quiz_Total, exams, gender, Student_Name from class hi"
./db "select Total, Quiz_Total, exams, gender, Student_Name from class hi"
echo

echo "select Total Quiz_Total from class"
./db "select Total Quiz_Total from class"
echo

echo "select Total, Quiz_Total from class ORDER BY exams hi"
./db "select Total, Quiz_Total from class ORDER BY exams hi"
echo

echo "select Total, Quiz_Total from class ORDER BY FAKECOLUMN"
./db "select Total, Quiz_Total from class ORDER BY FAKECOLUMN"
echo

echo "select Total, Quiz_Total, exams, gender, Student_Name from class where student > 'A' hi"
./db "select Total, Quiz_Total, exams, gender, Student_Name from class where student > 'A' hi"
echo

echo "select Total, Quiz_Total, exams, gender, Student_Name from class where student > 'A' AND student > 'A' hi"
./db "select Total, Quiz_Total, exams, gender, Student_Name from class where student > 'A' OR student > 'A' hi"
echo

echo "select FAKECOLUMN from class where student > 'A'"
./db "select FAKECOLUMN from class where student > 'A'"
echo

echo "select FAKECOLUMN, Quiz_Total, exams, gender, Student_Name from class where student > 'A'"
./db "select FAKECOLUMN, Quiz_Total, exams, gender, Student_Name from class where student > 'A'"
echo

echo "select sum(gender) from class"
./db "select sum(gender) from class"
echo

echo "select sum(*) from class"
./db "select sum(*) from class"
echo

echo "select avg(gender) from class"
./db "select avg(gender) from class"
echo

echo "select avg(*) from class"
./db "select avg(*) from class"
echo

echo ==========================================================================================
echo DELETE
echo ==========================================================================================

echo Error Testing **THESE SHOULD ALL BE BLOCKED AND YOUR TABLE SHOULDN\'T CHANGE**
echo "delete from class hi"
./db "delete from class hi"
echo

echo "delete from class where hi"
./db "delete from class where hi"
echo

echo "delete from class where gender ="
./db "delete from class where gender ="
echo

echo "delete from class where gender = null"
./db "delete from class where gender = null"
echo

echo "delete from class where gender = not null"
./db "delete from class where gender = not null"
echo

echo "delete from class where gender = 123"
./db "delete from class where gender = 123"
echo

echo "delete from class where gender = NoQuoteString"
./db "delete from class where gender = NoQuoteString"
echo

echo "delete from class where exams = 'CannotBeString'"
./db "delete from class where exams = 'CannotBeString'"
echo

echo "YOUR TABLE SHOULD NOT HAVE CHANGED"
./db "select * from class"

echo ==========================================================================================
echo UPDATE
echo ==========================================================================================
echo Error Testing **THESE SHOULD ALL BE BLOCKED AND YOUR TABLE SHOULDN\'T CHANGE**

echo "update class set STUDENT_NAME = null"
./db "update class set STUDENT_NAME = null"
echo

echo "update class set STUDENT_NAME = 123"
./db "update class set STUDENT_NAME = 123"
echo

echo "update class set STUDENT_NAME = 'damon' hi"
./db "update class set STUDENT_NAME = 'damon' hi"
echo

echo "update class set exams = 'string'"
./db "update class set exams = 'string'"
echo

echo "update class set exams"
./db "update class set exams"
echo

echo "update class set exams = "
./db "update class set exams = "
echo

echo "update class set FAKECOLUMN = 'string'"
./db "update class set FAKECOLUMN = 'string'"
echo

echo "update class set STUDENT_NAME = 'stephanie' where"
./db "update class set STUDENT_NAME = 'stephanie' where"
echo

echo "update class set STUDENT_NAME = 'stephanie' where exams = 'string'"
./db "update class set STUDENT_NAME = 'stephanie' where exams = 'string'"
echo

echo "update class set STUDENT_NAME = 'christine' where gender = 123"
./db "update class set STUDENT_NAME = 'christine' where gender = 123"
echo

echo "update class set STUDENT_NAME = 'ryan' where STUDENT_NAME = 'Frank' hi"
./db "update class set STUDENT_NAME = 'ryan' where STUDENT_NAME = 'Frank' hi"
echo

echo "update class STUDENT_NAME = 'ryan' where STUDENT_NAME = 'Frank'"
./db "update class STUDENT_NAME = 'ryan' where STUDENT_NAME = 'Frank'"
echo

echo "update class set gender 'hi' where STUDENT_NAME = 'test4'"
./db "update class set gender 'hi' where STUDENT_NAME = 'test4'"
echo

echo "update class set total = 3000000000 where STUDENT_NAME = 'Test1'"
./db "update class set total = 3000000000 where STUDENT_NAME = 'Test1'"
echo

echo "YOUR TABLE SHOULD NOT HAVE CHANGED"
./db "select * from class"
echo

echo ==========================================================================================
echo More fun breaking stuff
echo ==========================================================================================
echo Test SELECT with WHERE clause with a single condition.
echo "update class set total = 2000000000 where STUDENT_NAME = 'Test2'"
./db "update class set total = 2000000000 where STUDENT_NAME = 'Test2'"
echo

echo "update class set total = 2000000000 where STUDENT_NAME = 'Test3'"
./db "update class set total = 2000000000 where STUDENT_NAME = 'Test3'"
echo

echo "select * from class"
./db "select * from class"
echo 

echo SELECT SUM SHOULD RETURN 4 BILLION SOMETHING
echo "select sum(total) from class"
./db "select sum(total) from class"
echo
echo ------------------------------------------------------------------------------------------
echo ==========================================================================================
echo END FUNCTIONAL QUERY
echo ==========================================================================================
