CREATE SCHEMA `student_course` ;
use student_course;
CREATE TABLE borrow_record (
    student_id INT COMMENT '学号',
    book_id INT COMMENT '书籍ID',
    borrow_date DATE COMMENT '借阅日期',
    return_date DATE COMMENT '归还日期',
    -- 联合主键+外键关联，保证数据合法性
    PRIMARY KEY (student_id, book_id, borrow_date),
    FOREIGN KEY (student_id) REFERENCES students(id),
    FOREIGN KEY (book_id) REFERENCES books(idbooks)
) COMMENT '借阅记录表';

SELECT DATABASE();
SHOW TABLES;
-- 1. 插入学生数据（6条）
INSERT INTO students (id, students_name, age, gender)
VALUES
(1001, '张三', 20, '男'),   -- 18岁以上，有借阅记录
(1002, '李四', 19, '女'),
(1003, '王五', 17, '男'),
(1004, '赵六', 18, '女'),
(1005, '孙七', 21, '男'),
(1006, '周八', 16, '女');

-- 2. 插入书籍数据（6条）
INSERT INTO books (idbooks, book, author, category)
VALUES
(2001, '三国演义', '罗贯中', '历史'),
(2002, '红楼梦', '曹雪芹', '文学'),
(2003, '三体', '刘慈欣', '科幻'),
(2004, 'Python从入门到精通', '张三峰', '计算机'),
(2005, '西游记', '吴承恩', '古典'),
(2006, '水浒传', '施耐庵', '历史');

-- 3. 插入借阅记录数据（6条）
INSERT INTO borrow_record (student_id, book_id, borrow_date, return_date)
VALUES
(1001, 2001, '2025-12-01', NULL),        -- 张三借三国演义
(1001, 2003, '2025-12-10', '2026-01-05'),-- 张三借三体
(1002, 2002, '2026-01-02', NULL),        -- 李四借红楼梦
(1003, 2005, '2026-01-05', NULL),        -- 王五借西游记
(1005, 2004, '2026-02-01', NULL),        -- 孙七借Python
(1004, 2006, '2026-02-10', NULL);        -- 赵六借水浒传

describe students;
SELECT * FROM students;
describe books;
select * from books;
describe borrow_record;
select * from borrow_record;

-- 展示所有学生的姓名、年龄、性别，按年龄降序
select students_name,age,gender from students
order by age desc;
-- 关闭安全更新模式
SET SQL_SAFE_UPDATES = 0;
-- 用完后，重新开启安全模式（推荐）
SET SQL_SAFE_UPDATES = 1;


-- 修改书名为《三国演义》的书籍种类将历史修改为古典文学
update books 
set category = "古典文学" 
where book = "三国演义";

-- 为学生表增加一个字段telephone
alter table students
add column telephone varchar(11) comment '电话号码';

-- 为姓名为“张三”的学生增加电话号码：19912344321
update students 
set telephone = '19912344321'
where students_name = '张三';

-- 按性别分组，分别列出目前年龄最小的男生和女生
select s1.gender, s1.students_name, s1.age
from students s1
join (
    select gender, MIN(age) as min_age
    from students
    group by  gender
) s2 on s1.gender = s2.gender and s1.age = s2.min_age;

-- 删除18岁以上学生的借阅记录
delete from borrow_record
where student_id in (
	select id from students 
    where age > '18'
    );