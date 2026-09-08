-- 创建数据库
CREATE DATABASE IF NOT EXISTS library_system DEFAULT CHARACTER SET utf8mb4;
USE library_system;
-- drop table if exists students ;
-- drop table if exists borrow_record ;
-- 强制开启外键校验
SET FOREIGN_KEY_CHECKS = 1;
-- 学生表：主键id，年龄约束18/19/20/21/22/23
CREATE TABLE IF NOT EXISTS students (
    id INT PRIMARY KEY AUTO_INCREMENT  COMMENT '学生学号(主键)',
    students_name VARCHAR(45) NOT NULL COMMENT '学生姓名',
    age INT NOT NULL COMMENT '年龄',
    gender CHAR(2) COMMENT '性别',
    telephone VARCHAR(11) COMMENT '手机号',
    -- 约束：年龄只能是指定6个值
    CHECK (age IN (18,19,20,21,22,23))
)  COMMENT='学生信息表';
-- 书籍表：主键book_id
CREATE TABLE IF NOT EXISTS books (
    book_id INT PRIMARY KEY AUTO_INCREMENT  COMMENT '书籍编号(主键)',
    book_name VARCHAR(50) NOT NULL COMMENT '书名',
    author VARCHAR(30) COMMENT '作者',
    category VARCHAR(20) COMMENT '书籍分类'
)  COMMENT='图书信息表';
-- 借阅表：学生id、书籍id作为外键
CREATE TABLE IF NOT EXISTS borrow_record (
    borrow_id INT PRIMARY KEY AUTO_INCREMENT  COMMENT '借阅流水号',
    student_id INT NOT NULL COMMENT '关联学生学号',
    book_id INT NOT NULL COMMENT '关联书籍编号',
    borrow_date DATE NOT NULL COMMENT '借阅日期',
    return_date DATE COMMENT '归还日期',
    -- 外键：关联学生表
    FOREIGN KEY (student_id) REFERENCES students(id),
    -- 外键：关联书籍表
    FOREIGN KEY (book_id) REFERENCES books(book_id)
) COMMENT='借阅记录表';

INSERT INTO students (students_name, age, gender, telephone)
VALUES
('张三',20,'男','13800138001'),
('李四',19,'女','13800138002'),
('王五',21,'男','13800138003'),
('赵六',19,'女','13800138004'),
('孙七',22,'男','13800138005'),
('周八',23,'女','13800138006'),
('吴九',20,'男','13800138007'),
('郑十',19,'女','13800138008'),
('钱一',22,'男','13800138009'),
('冯二',21,'女','13800138010'),
('陈三',18,'男','13800138011'),
('褚四',23,'女','13800138012'),
('Lio', 19, '男', '13800138013');

INSERT INTO books (book_name, author, category)
VALUES
('西游记','吴承恩','古典文学'),
('西游记','吴承恩','古典文学'), -- 同名书，支持多人借
('三国演义','罗贯中','古典文学'),
('红楼梦','曹雪芹','古典文学'),
('三体','刘慈欣','科幻小说'),
('三体','刘慈欣','科幻小说'), -- 同名书
('Python编程','张三峰','计算机'),
('Java入门','李四光','计算机'),
('数据库原理','王五明','计算机'),
('平凡的世界','路遥','现代文学'),
('围城','钱钟书','现代文学'),
('解忧杂货店','东野圭吾','外国文学');

INSERT INTO borrow_record (student_id, book_id, borrow_date, return_date)
VALUES
(1,1,'2026-01-01',NULL),
(1,3,'2026-01-02','2026-02-10'),
(2,2,'2026-01-03',NULL),
(2,5,'2026-01-05','2026-03-01'),
(3,6,'2026-01-08',NULL),
(3,7,'2026-01-10',NULL),
(4,4,'2026-01-12','2026-02-20'),
(4,8,'2026-01-15',NULL),
(5,1,'2026-01-18',NULL),
(5,9,'2026-01-20','2026-03-05'),
(6,2,'2026-02-01',NULL),
(6,10,'2026-02-03','2026-03-10'),
(7,5,'2026-02-05',NULL),
(7,11,'2026-02-08',NULL),
(8,3,'2026-02-10','2026-03-15'),
(7,12,'2026-02-12',NULL),
(9,7,'2026-02-15',NULL),
(9,4,'2026-02-18','2026-03-20'),
(10,6,'2026-03-01',NULL),
(11,9,'2026-03-03',NULL);

DESCRIBE students;
DESCRIBE books;
DESCRIBE borrow_record;
select * from students;
select * from books;
select * from borrow_record;

-- 1. 查询没有借阅《西游记》的所有学生的姓名和年龄
select students_name,age from students s
where not exists (
	select id from borrow_record br
    join books b on b.book_id = br.book_id
    where b.book_name = '西游记'
    and s.id = br.student_id
	);
-- 2.查询借阅了书名为“西游记”的书籍的所有学生借阅的所有书籍名
select distinct book_name from books b
join borrow_record br on b.book_id = br.book_id
where br.student_id in (
	select distinct student_id
    from borrow_record br2
    join books b2 on br2.book_id = b2.book_id
    where b2.book_name = '西游记'
);

-- 3. 查询至少借阅了两本书的学生的姓名和性别
select  students_name,gender from students s
where exists (
	select student_id from borrow_record br
    where s.id = br.student_id
    group by br.student_id
    having count(book_id)>1
);

-- 4. 查询姓名为“张三”的学生没有借阅的书籍
select distinct book_name from books
where book_id not in (
		select book_id from borrow_record br
        join students s on s.id = br.student_id
        where s.students_name = "张三"
);
-- 5.查询借阅了姓名为“张三”的学生借阅的至少一本书籍的学生的学号和姓名
select id,students_name from students
where id in (
	select student_id from borrow_record 
    where book_id in (
		select book_id from borrow_record br
        join students s on s.id = br.student_id
        where s.students_name = '张三')
);

-- 6. 查询和姓名为“陈三”的同学借阅书籍集合相同的学生姓名
select distinct s.students_name from students s 
join borrow_record br1 on s.id = br1.student_id
where br1.book_id in (
	select book_id from borrow_record
    where student_id = (select id from students where students_name = "陈三")
)
and not exists (
	select 1 from borrow_record br2
    where  br2.student_id =s.id
    and br2.book_id not in (
    	select book_id from borrow_record
		where student_id = (select id from students where students_name = "陈三")
    )
);
-- 7. 检索所有学号比姓名为"占位符"的同学大，而年龄比ta小的学生姓名
select students_name from students
where id > (select id from students where students_name = "张三" )
and age < (select age from students where students_name = "张三");

-- 8. 检索姓名以L打头的所有学生的姓名和年龄
SELECT students_name, age
FROM students
WHERE students_name LIKE 'L%';

-- 9. 求年龄大于女同学平均年龄的男同学的姓名和年龄
select students_name,age from students
where gender = "男"
and age > (select avg(age) from students 
	where gender = "女"
	group by gender
    );
-- 10. 求年龄小于所有女同学年龄的男同学的姓名和年龄
select students_name,age from students
where gender = "男"
and age < (select min(age) from students 
	where gender = "女"
    );
    
-- 3 . 1. 在学生表中插入一条已有学生id的学生信息，观察执行结果
insert into students(id, students_name, age) 
values (1, '重复ID学生', 20);

-- 2. 在学生表中插入一条年龄为10的学生信息，观察执行结果
INSERT INTO students(id, students_name, age) 
VALUES (10, '小学生', 10);

-- 3. 在借阅表中将某条记录的学生id修改为学生表中不存在的学生id，观察执行结果
UPDATE borrow_record 
SET student_id = 999 
WHERE student_id = 1;

-- 4.基于“参照完整性约束”的知识，针对借阅表中的两个外键（书籍id和学生id），
-- 提供两个不满足参照完整性的插入操作
-- 学生表没有 id=888 的学生
INSERT INTO borrow_record(student_id, book_id) 
VALUES (888, 1);
-- 书籍表没有 book_id=666 的书
INSERT INTO borrow_record(student_id, book_id) 
VALUES (1, 666);

-- 4. 触发器
-- 1. 由于图书馆不允许学生借阅超过三本书，请创建触发器，
-- 使得向借阅表插入一条记录时，检查该学生当前借阅书籍数目，
-- 如果小于3，成功借出，否则拒绝执行
DELIMITER $$
create trigger borrow_check_trg before insert on borrow_record 
for each row
begin
	if (select count(*) from borrow_record br 
		where br.student_id = new.student_id and br.return_date is null ) >= 3 then
        signal sqlstate '45000' set message_text = "拒绝执行";
	end if;

end$$
DELIMITER ;
-- 成功插入
insert into borrow_record (student_id,book_id,borrow_date,return_date)
 value(3,4,'2026-04-03',null);
-- 失败插入
insert into borrow_record (student_id,book_id,borrow_date,return_date)
 value(7,10,'2026-04-03',null);

-- 5. 数据安全性
-- 1. 使用root创建用户teacher和student，赋予用户teacher对使用的数据库的所有权限
create user  'teacher'@'localhost' identified by '123456';
create user  'student'@'localhost' identified by '123456';
grant all privileges on library_system.* to 'teacher'@'localhost' with grant option;
flush privileges;
