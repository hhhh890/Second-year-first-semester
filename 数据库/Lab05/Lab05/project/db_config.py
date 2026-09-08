import mysql.connector

# Database configuration constants
DB_HOST = "localhost"
DB_USER = "root"
DB_PASSWORD = "xxx"
DB_NAME = "Mini_WeChat"

def get_connection(role_name=None):
    """
    Establish and return a database connection.
    Optionally sets the role for the connection if role_name is provided.
    
    Args:
        role_name (str, optional): The role to set ('Role_user' or 'Role_admin').
        
    Returns:
        mysql.connector.connection.MySQLConnection: The database connection object.
        
    Raises:
        ValueError: If an invalid role_name is provided.
        mysql.connector.Error: If the database connection fails.
    """
    try:
        connection = mysql.connector.connect(
            host=DB_HOST,
            user=DB_USER,
            password=DB_PASSWORD,
            database=DB_NAME
        )

        if role_name is not None:
            if role_name not in ('Role_user', 'Role_admin'):
                # Close the connection before raising the error to prevent resource leaks
                connection.close()
                raise ValueError(f"Invalid argument: '{role_name}'. Role must be 'Role_user' or 'Role_admin'.")
                
            cursor = connection.cursor()
            sql = "SET ROLE %s;"
            cursor.execute(sql, (role_name))
            cursor.close()

        return connection

    except mysql.connector.Error as err:
        print(f"Database connection error: {err}")
        return None