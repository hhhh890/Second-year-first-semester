	-- 8

-- 创建名为 "school" 的数据库
CREATE DATABASE IF NOT EXISTS school;

-- 选择 "school" 数据库
USE school;
	

    -- 10-12

-- 创建学生表格，包含学生 ID、姓名、年龄和性别
CREATE TABLE students (
  id INT PRIMARY KEY,      -- 学生 ID，主键
  name VARCHAR(50) NOT NULL, -- 学生姓名，非空
  age INT NOT NULL,        -- 学生年龄，非空
  gender VARCHAR(10) NOT NULL -- 学生性别，非空
);

-- 创建课程表格，包含课程 ID、课程名称和学分
CREATE TABLE courses (
  id INT PRIMARY KEY,      -- 课程 ID，主键
  name VARCHAR(50) NOT NULL, -- 课程名称，非空
  credit INT NOT NULL      -- 学分，非空
);

-- 创建选课表格，包含选课 ID、学生 ID、课程 ID 和选课时间
CREATE TABLE course_selection (
  id INT PRIMARY KEY,      -- 选课 ID，主键
  student_id INT NOT NULL, -- 学生 ID，非空
  course_id INT NOT NULL,  -- 课程 ID，非空
  selection_time DATETIME NOT NULL, -- 选课时间，非空
  FOREIGN KEY (student_id) REFERENCES students(id), -- 外键，引用学生表格的学生 ID
  FOREIGN KEY (course_id) REFERENCES courses(id)   -- 外键，引用课程表格的课程 ID
);

	-- 13

-- 显示所有表格
SHOW TABLES;

-- 显示 "students" 表格结构
DESCRIBE students;

-- 显示 "courses" 表格结构
DESCRIBE courses;

-- 显示 "course_selection" 表格结构
DESCRIBE course_selection;
	
	-- 14

-- 在 "students" 表格中增加 "address" 属性
ALTER TABLE students ADD address VARCHAR(50);

	-- 15

-- 选择 "school" 数据库
USE school;

-- 展示 "students" 表格中的所有数据
SELECT * FROM students;

-- 展示 "courses" 表格中的所有数据
SELECT * FROM courses;

-- 展示 "course_selection" 表格中的所有数据
SELECT * FROM course_selection;

	-- 16-18

-- 向 "students" 表格中插入数据
INSERT INTO students (id, name, age, gender)
VALUES
  (1, 'Tom', 20, 'Male'),
  (2, 'Jane', 21, 'Female'),
  (3, 'Bob', 22, 'Male'),
  (4, 'Alice', 23, 'Female');

-- 向 "courses" 表格中插入数据
INSERT INTO courses (id, name, credit)
VALUES
  (1, 'Math', 3),
  (2, 'English', 2),
  (3, 'Physics', 4),
  (4, 'Chemistry', 3);

-- 向 "course_selection" 表格中插入数据
INSERT INTO course_selection (id, student_id, course_id, selection_time)
VALUES
  (1, 1, 1, '2022-09-01 10:00:00'),
  (2, 1, 2, '2022-09-02 11:00:00'),
  (3, 2, 1, '2022-09-03 12:00:00'),
  (4, 2, 2, '2022-09-04 13:00:00'),
  (5, 3, 3, '2022-09-05 14:00:00'),
  (6, 3, 4, '2022-09-06 15:00:00'),
  (7, 4, 1, '2022-09-07 16:00:00');

```
-- 19
```

SHOW VARIABLES LIKE 'secure_file_priv';

LOAD DATA INFILE 'D:/MySQL/MySQL Server 8.0/Uploads/students.txt' 
INTO TABLE students
FIELDS TERMINATED BY ','
LINES TERMINATED BY '\n'
(id, name, gender, address);	
select * from students;

	-- 20

-- 修改 "students" 表格中 Alice 的地址
UPDATE students
SET address = '456 Oak St'
WHERE name = 'Alice';

	-- 21

-- 插入一条学生记录，id为100，姓名为temp，年龄为15，性别为F
INSERT INTO students (id, name, age, gender) VALUES ('100', 'temp', '15', 'F');

-- 删除 "students" 表格中名字为 temp 的记录
DELETE FROM students
WHERE name = 'temp';

	-- 22-26

-- 查询 "students" 表格中名字为 Alice 的记录的学号和年龄
SELECT id, age
FROM students
WHERE name = 'Alice';

-- 计算 "students" 表格中年龄的平均值
SELECT AVG(age) AS avg_age
FROM students;

-- 按照 "age" 属性升序排序，查询 "students" 表格中前 5 条记录的名字和年龄
SELECT name, age
FROM students
ORDER BY age ASC
LIMIT 5;

-- 按照 "gender" 属性分组，计算每个性别的年龄平均值
SELECT gender, AVG(age) AS avg_age
FROM students
GROUP BY gender;

-- 查询 "students" 表格中年龄大于 20 小于 25 的记录的学号和年龄
SELECT id, age
FROM students
WHERE age > 20 AND age < 25;

	-- 27

SELECT students.name AS student_name, courses.name AS courses_name -- 选择学生姓名和课程名称作为输出，并给输出列起别名
FROM students, courses, course_selection -- 从三个表中进行查询
WHERE students.id = course_selection.student_id -- 连接学生表格和选课表格
AND courses.id = course_selection.course_id; -- 连接课程表格和选课表格

	-- 28

-- 查询选修了数学课的学生姓名
SELECT name -- 查询结果列为学生姓名
FROM students -- 从学生表中查询
WHERE id IN ( -- 子查询，查询选课表中选修数学课的学生 ID
  SELECT student_id -- 查询结果列为学生 ID
  FROM course_selection -- 从选课表中查询
  WHERE course_id = ( -- 子查询，查询数学课程的 ID
    SELECT id -- 查询结果列为数学课程的 ID
    FROM courses -- 从课程表中查询
    WHERE name = "Math" -- 查询条件为课程名称为 Math
  )
);

	-- 29

-- 查询学生表中每个性别的选课人数
SELECT gender, COUNT(*) -- 查询结果包含性别和该性别的选课人数
FROM students -- 从学生表中查询
WHERE id IN ( -- 条件：学生 ID 存在于选课表中
  SELECT DISTINCT student_id -- 选课表中不重复的学生 ID
  FROM course_selection
) GROUP BY gender; -- 按性别分组
