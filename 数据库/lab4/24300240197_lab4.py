try:
    import mysql.connector
    from mysql.connector import Error
except ImportError as e:
    print("MySQL Python driver not found. Install with: pip install mysql-connector-python")
    raise


def main() -> None:
    host = "127.0.0.1"
    port = 3306
    user = "root"
    password = "123456"

    conn = None
    try:
        # 1. 建立与MySQL服务连接
        conn = mysql.connector.connect(
            host=host,
            port=port,
            user=user,
            password=password
        )

        print("Connected to MySQL successfully.")

        # with conn.cursor() as cursor:
        #     cursor.execute("SELECT VERSION()")
        #     rows = cursor.fetchone()
        #     version = rows[0] if rows else None
        #     print(f"MySQL version: {version}")

        # 2. 为图书馆借阅系统创建数据库和3张数据表
        mycursor = conn.cursor()
        mycursor.execute("CREATE DATABASE IF NOT EXISTS library")
        mycursor.execute("USE library")
        # 学生表至少包含学号、姓名、年龄、性别，
        # 书籍表至少包含书籍id，书名、作者、书籍种类，
        # 借阅表至少包含学号、书籍id，借阅日期、归还日期
        # 删除表，解决重复插入问题
        # mycursor.execute("drop table if exists borrow_records")
        # mycursor.execute("drop table if exists books")
        # mycursor.execute("drop table if exists students")
        mycursor.execute(
            "CREATE TABLE IF NOT EXISTS students "
            "(id INT PRIMARY KEY AUTO_INCREMENT," \
            " student_id int(20) UNIQUE, " \
            "name VARCHAR(50), age INT, gender VARCHAR(10))")
        mycursor.execute(
            "CREATE TABLE IF NOT EXISTS books "
            "(id INT PRIMARY KEY AUTO_INCREMENT," \
            " book_id int(20) UNIQUE, " \
            "title VARCHAR(100), author VARCHAR(50), category VARCHAR(50))")
        mycursor.execute(
            "CREATE TABLE IF NOT EXISTS borrow_records "
            "(id INT PRIMARY KEY AUTO_INCREMENT," \
            " student_id int(20), " \
            " book_id int(20), " \
            " borrow_date DATE, " \
            " return_date DATE, " \
            " FOREIGN KEY (student_id) REFERENCES students(student_id), " \
            " FOREIGN KEY (book_id) REFERENCES books(book_id))")
        print("Database and tables created successfully.")

        # 3. 为3张数据表分别插入5条数据
        sql1 = "INSERT INTO students (student_id, name, age, gender) VALUES (%s, %s, %s, %s)"
        sql2 = "INSERT INTO books (book_id, title, author, category) VALUES (%s, %s, %s, %s)"
        sql3 = "INSERT INTO borrow_records (student_id, book_id, borrow_date, return_date) VALUES (%s, %s, %s, %s)"
        
        val1 = [
            (101, "张三", 20, "男"),
            (102, "李四", 19, "女"),
            (103, "王五", 21, "男"),
            (104, "赵六", 20, "女"),
            (105, "钱七", 19, "男")
        ]

        val2 = [
            (201, "Python编程从入门到实践", "埃里克·马瑟斯", "计算机编程"),
            (202, "三国演义", "罗贯中", "文学名著"),
            (203, "深度学习", "伊恩·古德费洛", "人工智能"),
            (204, "红楼梦", "曹雪芹", "文学名著"),
            (205, "数据结构与算法", "严蔚敏", "计算机基础")
        ]

        val3 = [
            (101, 201, "2025-01-10", "2025-01-20"),
            (102, 202, "2025-01-11", "2025-01-21"),
            (103, 203, "2025-01-12", None),
            (104, 204, "2025-01-13", "2025-01-23"),
            (105, 205, "2025-01-14", None)
        ]
        mycursor.executemany(sql1, val1)
        mycursor.executemany(sql2, val2)
        mycursor.executemany(sql3, val3)
        conn.commit()

        # 4. 查询学生表中的所有数据，打印输出查询结果（例如使用print函数）
        mycursor.execute("SELECT * FROM students")
        rows = mycursor.fetchall()
        for row in rows:
            print(row)
        # 5. 自定义一个多表查询，打印输出查询结果（例如使用print函数）
        mycursor.execute("SELECT s.student_id, s.name, b.title, r.borrow_date, r.return_date " \
        "FROM students s, books b, borrow_records r " \
        "WHERE s.student_id = r.student_id AND b.book_id = r.book_id")
        rows = mycursor.fetchall()
        for row in rows:
            print(row)
        # 6. 修改学生表中某位学生的年龄字段，并打印输出修改后的学生表所有信息（例如使用print函数）
        mycursor.execute("UPDATE students SET age = 22 WHERE student_id = 101")
        conn.commit()
        mycursor.execute("SELECT * FROM students")
        rows = mycursor.fetchall()
        for row in rows:
            print(row)
        #7. 自定义一个针对某个表删除部分元组的操作，并打印输出删除操作后的该表所有信息（例如使用print函数）
        mycursor.execute("DELETE FROM borrow_records WHERE student_id = 102")
        mycursor.execute("DELETE FROM students WHERE student_id = 102")
        conn.commit()
        mycursor.execute("SELECT * FROM students")
        rows = mycursor.fetchall()
        for row in rows:
            print(row)

    except Error:
        print("Connection failed. Check MySQL service/user/password.")
        raise
    finally:
        if conn is not None and conn.is_connected():
            conn.close()
            print("Connection closed.")


if __name__ == "__main__":
    main()


