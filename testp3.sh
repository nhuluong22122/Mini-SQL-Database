# you have found the easter egg

# exec > testp3.out 2>&1    #redirect output to testp3.out

echo Project Part 3 test batch file
echo
echo cleanup
echo
rm dbfile.bin
rm db.log*
rm *.log
rm *.tab
rm *.obj
rm *.pdb
rm *.ilk
rm first
rm second
rm third
gcc -m32 -o db db.cpp

echo
echo "01. Setup and inserts"
echo
./db "create table tab1(name char(16), quizzes int, midterm int, final int)"
echo
./db "create table tab2(student_name char(16), college char(20), zipcode char(5), rank int)"
echo
./db "insert into tab1 values('Siu', 11, 80, 560)"
echo
./db "insert into tab1 values('Frank', 22, 100, 700)"
echo
./db "insert into tab1 values('Butcher', 33, 75, 525)"
echo
./db "insert into tab1 values('Jaina', 44, 90, 600)"
echo
./db "insert into tab1 values('Nazeebo', 55, 92, 624)"
echo
./db "insert into tab1 values('Sylvanas', 66, 60, 420)"
echo
./db "insert into tab1 values('Jordon', 77, 79, 580)"
echo
./db "insert into tab2 values('Siu','UCLA', '11111', 3)"
echo
./db "insert into tab2 values('Frank','SJSU', '22222', 10)"
echo
./db "insert into tab2 values('Jordon','Stanford', '33333', 2)"
echo
./db "insert into tab2 values('Sylvanas','UW', '44444', 5)"
echo
./db "select * from tab1"
echo
./db "select * from tab2"
echo
./db "select * from tab1, tab2 where name = student_name"
echo
./db "select * from tab1, tab2 where name = student_name AND name = 'Jordon'"
echo
./db "select * from tab1, tab2 where name = student_name AND quizzes < 50"
echo
./db "select * from tab1, tab2 where name = 'Jordon' and name = student_name"
echo
./db "select * from tab1, tab2 where quizzes > 50 and name = student_name"
# echo
# echo Check transaction log
# echo
# cat db.log
# echo
# echo "02. Take backup, check image size and db.log"
# echo
# ./db "backup to first"
# echo
# echo '**Size** first=584; dbfile.bin=336; tab1=120 (3 rows); tab2=120 (3 rows)'
# ls -l first dbfile.bin *.tab
# cat db.log
# echo
# echo "03. Do more I/U/D"
# echo
# ./db "insert into tab1 values('Jeff', 44, 60, 515)"
# echo
# ./db "insert into tab2 values('UC Berkley', '44444', 1)"
# echo
# echo Wait a few seconds
# echo Pause here, press enter to continue; read dummy;
# ./db "insert into tab1 values('Ying', 55, 85, 625)"
# echo
# ./db "insert into tab2 values('USC', '55555', 4)"
# echo
# ./db "delete from tab2 where college = 'UCLA'"
# echo
# echo Wait a few seconds
# echo Pause here, press enter to continue; read dummy;
# ./db "delete from tab2 where college = 'SJSU'"
# echo
# ./db "update tab1 set final = 999 where name = 'Siu'"
# echo
# ./db "select * from tab1"
# echo
# ./db "select * from tab2"
# echo
# cat db.log
# echo
# echo "04. Take 2nd backup, check image size and db.log"
# echo
# ./db "backup to second"
# echo
# echo '**Size** first=648; dbfile.bin=336; tab1=184 (5 rows); tab2=120 (3 rows)'
# ls -l second dbfile.bin *.tab
# cat db.log
# echo
# echo "05. drop tab2, restore from second, check tab2 and RF_START in log"
# echo
# ./db "drop table tab2"
# echo
# ls tab2.tab
# echo
# ./db "restore from second"
# echo
# ls tab2.tab
# echo THIS SHOULD FAIL because it should be RF Pending
# ./db "create table tab3(c1 int)"
# echo
# cat db.log
# echo
# echo "06. Do rollforward, tab2 should be dropped again and RF_START is removed"
# echo
# ./db "rollforward"
# echo
# ls tab2.tab
# echo
# cat db.log
# echo
# echo "07. Do restore from second without RF, check db.log1 before prune,"
# echo "    check tab2 contents"
# echo
# ./db "create table tab3(c1 int)"
# echo
# ./db "insert into tab3 values(911)"
# echo
# cat db.log
# echo
# ./db "restore from second without rf"
# echo
# ls db.log1
# echo tab 3 should be gone from PRUNING without rf
# cat db.log
# echo
# ./db "select * from tab2"
# echo
# echo "08. restore from fisrt, check tab1 & tab2 contents"
# echo
# ./db "restore from first"
# echo
# ./db "select * from tab1"
# echo
# ./db "select * from tab2"
# echo
# cat db.log
# echo
# echo "09. Do rollforward to timestamp  -  Manual step from a different window"
# echo "In this case the timestamp is between the deletion of the 1st the 2nd row from tab2 e.g. db "rollforward to 20030531123030""
# echo Pause here, press enter to continue; read dummy;
# echo
# echo "10. Copy the db.log to db.log3, copy db.log1 to db.log, restore from second,"
# echo "    rollforward, verify tab2 is dropped again."
# echo
# cp db.log db.log3
# echo
# cp db.log1 db.log
# echo
# ./db "restore from second"
# echo Pause here, press enter to continue; read dummy;
# echo
# ./db "rollforward"
# ls tab2.tab
# echo
# echo "11 - 13. Errors - dupicate backup image name, bad image name, bad timestamp, wrong state"
# echo
# ./db "backup to third"
# echo
# ./db "backup to third"
# echo
# ./db "restore from nothing"
# echo
# ./db "insert into tab1 values('new', 55, 85, 625)"
# echo
# ./db "rollforward"
# echo
# ./db "restore from third"
# echo
# ./db "rollforward to 20030531123030"
# echo
# ./db "rollforward"
# echo
# ./db "select * from tab1"
# echo
# echo End of test1.bat
# echo
