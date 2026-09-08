use student_course ;
describe students;
select * from students;
select * from books;
select * from borrow_record;

SET SQL_SAFE_UPDATES = 0;
-- 查询姓名为“王五”所借阅的所有书籍的书名
select book from books b1
where b1.idbooks in (
	select book_id from borrow_record
    where student_id in (
		select id from students
        where students_name = '王五'
        )
    );
    
-- 将姓名为“王五”所借阅的书籍的归还日期修改为2026-03-01
update  borrow_record 
set return_date = '2026-03-01'
where student_id in (
	select id from students
    where students_name = '王五'
    );
    
-- 将所有借阅了书籍id为”2005“的学生（即王五）的年龄增加2岁
update students s
set age = age+2
where exists (
	select *
    from borrow_record br
    where br.student_id = s.id
    and br.book_id = '2005'
    );
-- 查询所有借阅记录的信息，包括姓名、书名和归还时间
select 
	s.students_name as 姓名,
	b.book as 书名,
    br.return_date as 归还时间
from students s
join borrow_record br
 on s.id = br.student_id
join books b
 on b.idbooks = br.book_id;