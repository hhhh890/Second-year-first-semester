import mysql.connector
from db_config import get_connection
from handle_error import handle_db_error


def register_user(user_id: str, password: str, name: str, gender: str, birth_date_str: str) -> dict:
    connection = get_connection()

    if connection is None:
        return {"status": False, "msg": "Unable to connect to the database"}

    try:
        from datetime import datetime
        birth_date = datetime.strptime(birth_date_str, '%Y-%m-%d').date()
        if birth_date > datetime.now().date():
            return {"status": False, "msg": "Registration failed: Birth date cannot be in the future."}

        if password == "": password = None
        if name == "": name = None
        
        my_cursor = connection.cursor()
        sql = "INSERT INTO User(user_id, password, name, gender, birth_date) VALUES (%s, %s, %s, %s, %s);"
        my_cursor.execute(sql, (user_id, password, name, gender, birth_date_str))
        connection.commit()  
        return {"status": True, "msg": "Registration successful"}

    except mysql.connector.Error as err:
        return handle_db_error(err, "Registration")
        
    finally:
        if 'my_cursor' in locals():
            my_cursor.close()
        connection.close()

def login(user_id: str, password: str, login_type: str) -> dict:
    connection = get_connection()

    if connection is None:
        return {"status": False, "msg": "Unable to connect to the database"}

    try:
        my_cursor = connection.cursor(dictionary=True)
        if login_type == "Role_user":
            sql = "SELECT * FROM User WHERE user_id = %s AND password = %s;"
        elif login_type == "Role_admin":
            sql = "SELECT * FROM Admin WHERE admin_id = %s AND password = %s"
        else:
            return {"status": False, "msg": "Unknown user type"}
        
        my_cursor.execute(sql, (user_id, password))
        result = my_cursor.fetchone()

        if result is None:
            return {"status": False, "msg": "Incorrect account or password"}
        else:
            return {"status": True, "msg": "Login successful", "id": user_id, "role": login_type}

    except mysql.connector.Error as err:
        return handle_db_error(err, "Login query")
    
    finally:
        if 'my_cursor' in locals():
            my_cursor.close()
        connection.close()