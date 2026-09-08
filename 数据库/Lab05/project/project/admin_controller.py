from db_config import get_connection
from handle_error import handle_db_error
import mysql.connector

def update_admin_profile(admin_id: str, name: str, phone_number: str, email: str) -> dict:
    connection = get_connection()
    if connection is None:
        return {"status": False, "msg": "Database connection failed."}

    try:
        cursor = connection.cursor()
        cursor.execute("SET ROLE Role_admin;")
        
        if name == "": name = None
        if phone_number == "": phone_number = None

        sql = "UPDATE Admin SET name = %s, phone_number = %s, email = %s WHERE admin_id = %s"
        cursor.execute(sql, (name, phone_number, email, admin_id))
        connection.commit()
        
        cursor.execute("SELECT 1 FROM Admin WHERE admin_id = %s", (admin_id,))
        if not cursor.fetchone():
            return {"status": False, "msg": "Update admin's profile failed: Admin does not exist."}
            
        return {"status": True, "msg": "Profile updated successfully."}
    
    except mysql.connector.Error as err:
        return handle_db_error(err, "Update admin's profile")
    
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()

def delete_user(user_id: str) -> dict:
    connection = get_connection()
    if connection is None:
        return {"status": False, "msg": "Database connection failed."}

    try:
        cursor = connection.cursor()
        cursor.execute("SET ROLE Role_admin;")

        sql = "DELETE FROM User WHERE user_id = %s"
        cursor.execute(sql, (user_id,))
        connection.commit()
        
        if cursor.rowcount == 0:
            return {"status": False, "msg": "Delete user failed: User does not exist."}
            
        return {"status": True, "msg": "User deleted successfully."}
    
    except mysql.connector.Error as err:
        return handle_db_error(err, "Delete user")
    
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()

def view_moment() -> dict:
    connection = get_connection()
    if connection is None:
        return {"status": False, "msg": "Database connection failed."}

    try:
        cursor = connection.cursor()
        cursor.execute("SET ROLE Role_admin;")

        sql = "SELECT * FROM Admin_Moment_View"
        cursor.execute(sql)
        results = cursor.fetchall()
        if results is None:
            print("There are no moments currently!")
        return {"status": True, "msg": "Moment viewed successfully.", "data": results}
    
    except mysql.connector.Error as err:
        return handle_db_error(err, "View moment")
    
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()

def delete_moment(moment_id: int) -> dict:
    connection = get_connection()
    if connection is None:
        return {"status": False, "msg": "Database connection failed."}

    try:
        cursor = connection.cursor()
        cursor.execute("SET ROLE Role_admin;")

        sql = "DELETE FROM Moment WHERE moment_id = %s"
        cursor.execute(sql, (moment_id,))
        connection.commit()
        
        if cursor.rowcount == 0:
            return {"status": False, "msg": "Delete moment failed: Moment does not exist."}
            
        return {"status": True, "msg": "Moment deleted successfully."}
    
    except mysql.connector.Error as err:
        return handle_db_error(err, "Delete moment")
    
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()

def get_admin_profile(admin_id: str) -> dict:
    connection = get_connection()
    if connection is None:
        return {"status": False, "msg": "Database connection failed."}

    try:
        cursor = connection.cursor(dictionary=True)
        cursor.execute("SET ROLE Role_admin;")
        sql = "SELECT admin_id, name, phone_number, email FROM Admin WHERE admin_id = %s"
        cursor.execute(sql, (admin_id,))
        res = cursor.fetchone()
        if res:
            return {"status": True, "data": res}
        return {"status": False, "msg": "Admin not found."}
    except mysql.connector.Error as err:
        return handle_db_error(err, "Get Admin Profile")
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()