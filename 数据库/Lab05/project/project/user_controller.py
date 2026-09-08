import mysql.connector
from db_config import get_connection
from handle_error import handle_db_error

def update_user_profile(current_user_id: str, new_name: str, new_gender: str, new_birth_date_str: str) -> dict:
    connection = get_connection()
    if connection is None:
        return {"status": False, "msg": "Database connection failed."}

    try:
        from datetime import datetime
        new_birth_date = datetime.strptime(new_birth_date_str, '%Y-%m-%d').date()
        if new_birth_date > datetime.now().date():
            return {"status": False, "msg": "Update profile failed: Birth date cannot be in the future."}

        cursor = connection.cursor()
        cursor.execute("SET ROLE Role_user;")
        
        # convert an empty string to None to violate the NOT NULL constraint.
        if new_name == "": new_name = None
        
        sql = "UPDATE User SET name = %s, gender = %s, birth_date = %s WHERE user_id = %s"
        cursor.execute(sql, (new_name, new_gender, new_birth_date_str, current_user_id))
        connection.commit()
        
        if cursor.rowcount == 0:
            return {"status": False, "msg": "Update user's profile failed: User does not exist."}
            
        return {"status": True, "msg": "Profile updated successfully."}
    
    except mysql.connector.Error as err:
        return handle_db_error(err, "Update user's profile")
    
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()

def add_friend(current_user_id: str, friend_id: str) -> dict:
    if current_user_id == friend_id:
        return {"status": False, "msg": "You cannot add yourself as a friend."}

    connection = get_connection()
    if connection is None:
         return {"status": False, "msg": "Database connection failed."}

    try:
        connection.start_transaction()
        cursor = connection.cursor()
        cursor.execute("SET ROLE Role_user;")

        sql = "INSERT INTO Friendship (user_id, friend_id) VALUES (%s, %s)"
        
        cursor.execute(sql, (current_user_id, friend_id))
        cursor.execute(sql, (friend_id, current_user_id))

        connection.commit()
        return {"status": True, "msg": "Friend added successfully."}

    except mysql.connector.Error as err:
        connection.rollback()
        return handle_db_error(err, "Add friend")
            
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()

def delete_friend(current_user_id: str, friend_id: str) -> dict:
    if current_user_id == friend_id:
        return {"status": False, "msg": "You cannot delete yourself."}

    connection = get_connection()
    if connection is None:
         return {"status": False, "msg": "Database connection failed."}

    try:
        connection.start_transaction()
        cursor = connection.cursor()
        cursor.execute("SET ROLE Role_user;")

        sql = "DELETE FROM Friendship WHERE user_id = %s AND friend_id = %s"
        
        cursor.execute(sql, (current_user_id, friend_id))
        cursor.execute(sql, (friend_id, current_user_id))

        connection.commit()

        if cursor.rowcount == 0:
            return {"status": False, "msg": "Delete friend failed: friend ID does not exist."}
        else:
            return {"status": True, "msg": "Friend deleted successfully"}

    except mysql.connector.Error as err:
        connection.rollback()
        return handle_db_error(err, "Delete friend")
            
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()


def view_friends(current_user_id: str) -> dict:
    connection = get_connection()
    if connection is None:
         return {"status": False, "msg": "Database connection failed."}
    
    try:
        cursor = connection.cursor(dictionary=True)
        cursor.execute("SET ROLE Role_user;")
        sql = "SELECT f.friend_id, f.group_id, u.name, u.gender, u.birth_date, u.age, g.group_name " \
        "FROM Friendship f JOIN User u ON (f.friend_id = u.user_id) LEFT JOIN FriendGroup g ON (f.group_id = g.group_id) " \
        "WHERE f.user_id = %s" 
        cursor.execute(sql, (current_user_id,))
        results = cursor.fetchall()

        if results is None:
            print("You dont't have any friends currently!")

        return {"status": True, "msg": "Friends viewed successfully", "data": results}

    except mysql.connector.Error as err:
        return handle_db_error(err, "View friends")
    
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()

def find_friend(current_user_id: str, friend_name: str) -> dict:
    connection = get_connection()
    if connection is None:
         return {"status": False, "msg": "Database connection failed."}
    
    try:
        cursor = connection.cursor(dictionary=True)
        cursor.execute("SET ROLE Role_user;")
        sql = "SELECT f.friend_id, f.group_id, u.name, u.gender, u.birth_date, u.age, g.group_name " \
        "FROM Friendship f JOIN User u ON (f.friend_id = u.user_id) LEFT JOIN FriendGroup g ON (f.group_id = g.group_id) " \
        "WHERE f.user_id = %s AND u.name = %s" 
        cursor.execute(sql, (current_user_id, friend_name))
        result = cursor.fetchall()

        if not result:
            return {"status": False, "msg": f"You don't have a friend whose name is {friend_name}"}

        return {"status": True, "msg": "Friends found successfully", "data": result}

    except mysql.connector.Error as err:
        return handle_db_error(err, "View friends")
    
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()

def manage_friend(group_id: int, current_user_id: str, friend_id: str) -> dict:
    connection = get_connection()
    if connection is None:
        return {"status": False, "msg": "Database connection failed."}

    try:
        cursor = connection.cursor()
        cursor.execute("SET ROLE Role_user;")

        check_group_sql = "SELECT group_id FROM FriendGroup WHERE group_id = %s AND created_by = %s"
        cursor.execute(check_group_sql, (group_id, current_user_id))
        if cursor.fetchone() is None:
            return {"status": False, "msg": "Manage friend failed: Friend group does not exist."}
        
        update_friendship_sql = "UPDATE Friendship SET group_id = %s WHERE user_id = %s AND friend_id = %s"
        cursor.execute(update_friendship_sql, (group_id, current_user_id, friend_id))
        connection.commit()

        if cursor.rowcount == 0:
            return {"status": False, "msg": "Manage friend failed: Friend ID does not exist in your contact list."}
        
        return {"status": True, "msg": "Friend added to group successfully."}

    except mysql.connector.Error as err:
        return handle_db_error(err, "Manage friend")
    
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()

def create_group(group_name: str, current_user_id: str) -> dict:
    connection = get_connection()

    if group_name == "": group_name = None

    if connection is None:
         return {"status": False, "msg": "Database connection failed."}
    
    try:
        cursor = connection.cursor()
        cursor.execute("SET ROLE Role_user;")

        # Check if a group with the same name already exists for this user
        check_sql = "SELECT 1 FROM FriendGroup WHERE group_name = %s AND created_by = %s"
        cursor.execute(check_sql, (group_name, current_user_id))
        if cursor.fetchone():
            return {"status": False, "msg": f"Create group failed: Group '{group_name}' already exists."}

        sql = "INSERT INTO FriendGroup (group_name, created_by) VALUES (%s, %s)"
        cursor.execute(sql, (group_name, current_user_id))
        connection.commit()
        return {"status": True, "msg": "Friend group created successfully."}
    
    except mysql.connector.Error as err:
        return handle_db_error(err, "Create friend group")
    
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()

def delete_group(group_name: str, current_user_id: str) -> dict:
    connection = get_connection()
    if connection is None:
         return {"status": False, "msg": "Database connection failed."}

    try:
        cursor = connection.cursor()
        cursor.execute("SET ROLE Role_user;")

        sql = "DELETE FROM FriendGroup WHERE group_name = %s AND created_by = %s"
        cursor.execute(sql, (group_name, current_user_id))
        connection.commit()
        if cursor.rowcount == 0:
            return {"status": False, "msg": "Delete friendgroup failed: friendgroup name does not exist."}
        else:
            return {"status": True, "msg": "Friendgroup deleted successfully"}
    
    except mysql.connector.Error as err:
        return handle_db_error(err, "Delete friend group")
    
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()

def publish_moment(moment_content: str, current_user_id: str) -> dict:
    connection = get_connection()
    if connection is None:
         return {"status": False, "msg": "Database connection failed."}
    
    if moment_content == "": moment_content = None

    try:
        cursor = connection.cursor()
        cursor.execute("SET ROLE Role_user;")

        sql = "INSERT INTO Moment (moment_content, user_id) VALUES (%s, %s)"
        cursor.execute(sql, (moment_content, current_user_id))
        connection.commit()
        return {"status": True, "msg": "Moment published successfully."}
    
    except mysql.connector.Error as err:
        return handle_db_error(err, "Publish moment")
    
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()

def view_my_moments(current_user_id: str) -> dict:
    connection = get_connection()
    if connection is None:
         return {"status": False, "msg": "Database connection failed."}
    
    try:
        cursor = connection.cursor(dictionary=True)
        cursor.execute("SET ROLE Role_user;")
        sql = "SELECT m.moment_id, m.moment_content, m.user_id AS moment_user_id, m.updated_at, " \
        "c.comment_id, c.comment_content, c.user_id AS comment_user_id, c.created_at " \
        "FROM Moment m LEFT JOIN Comment c ON (m.moment_id = c.moment_id) " \
        "WHERE m.user_id = %s"
        cursor.execute(sql, (current_user_id,))
        results = cursor.fetchall()

        if results is None:
            print("You have not published any moments yet!")

        return {"status": True, "msg": "Moments viewed successfully", "data": results}

    except mysql.connector.Error as err:
        return handle_db_error(err, "View moments")
    
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()

def delete_moment(moment_id: int, current_user_id: str) -> dict:
    connection = get_connection()
    if connection is None:
         return {"status": False, "msg": "Database connection failed."}

    try:
        cursor = connection.cursor()
        cursor.execute("SET ROLE Role_user;")

        sql = "DELETE FROM Moment WHERE moment_id = %s AND user_id = %s"
        cursor.execute(sql, (moment_id, current_user_id))
        connection.commit()

        if cursor.rowcount == 0:
            return {"status": False, "msg": "Delete moment failed: moment ID does not exist."}
        else:
            return {"status": True, "msg": "Moments deleted successfully"}
    
    except mysql.connector.Error as err:
        return handle_db_error(err, "Delete moment")
    
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()

def update_moment(moment_id: int, current_user_id: str, new_moment_content: str) -> tuple:
    connection = get_connection()
    if connection is None:
         return {"status": False, "msg": "Database connection failed."}

    try:
        if new_moment_content == "": new_moment_content = None

        cursor = connection.cursor()
        cursor.execute("SET ROLE Role_user;")

        sql = "UPDATE Moment SET moment_content = %s " \
        "WHERE moment_id = %s AND user_id = %s"

        cursor.execute(sql, (new_moment_content, moment_id, current_user_id))
        connection.commit()

        if cursor.rowcount == 0:
            return {"status": False, "msg": "Update Moment failed: moment does not exist."}
            
        return {"status": True, "msg": "Update moment successfully."}
    
    except mysql.connector.Error as err:
        return handle_db_error(err, "Update moment")
    
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()

def view_friends_moments(current_user_id: str) -> dict:
    connection = get_connection()
    if connection is None:
         return {"status": False, "msg": "Database connection failed."}
    
    try:
        cursor = connection.cursor(dictionary=True)
        cursor.execute("SET ROLE Role_user;")
        sql = "SELECT m.moment_id, m.moment_content, m.user_id AS moment_user_id, m.updated_at, " \
        "c.comment_id, c.comment_content, c.user_id AS comment_user_id, c.created_at " \
        "FROM Moment m LEFT JOIN Comment c ON (m.moment_id = c.moment_id) " \
        "WHERE m.user_id IN (" \
        "SELECT friend_id FROM Friendship WHERE Friendship.user_id = %s);"
        cursor.execute(sql, (current_user_id,))
        results = cursor.fetchall()

        if results is None:
            print("Your friends have not published any moments yet!")

        return {"status": True, "msg": "Moment viewed successfully", "data": results}

    except mysql.connector.Error as err:
        return handle_db_error(err, "View moments")
    
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()

def publish_comment(current_user_id: str, moment_id: int, comment: str) -> dict:
    connection = get_connection()
    if connection is None:
         return {"status": False, "msg": "Database connection failed."}
    
    try:
        cursor = connection.cursor(dictionary=True)
        cursor.execute("SET ROLE Role_user;")

        cursor.execute("SELECT user_id FROM Moment WHERE moment_id = %s", (moment_id,))
        moment = cursor.fetchone()
        if not moment:
            return {"status": False, "msg": "Publish comment failed: Target moment does not exist."}

        moment_author_id = moment['user_id']

        if moment_author_id != current_user_id:
            cursor.execute("SELECT 1 FROM Friendship WHERE user_id = %s AND friend_id = %s", (current_user_id, moment_author_id))
            if not cursor.fetchone():
                return {"status": False, "msg": "Publish comment failed: You can only comment on your own or your friends' moments."}

        if not comment or comment.strip() == "":
            return {"status": False, "msg": "Publish comment failed: Comment content cannot be empty."}

        sql = "INSERT INTO Comment (comment_content, user_id, moment_id) VALUES (%s, %s, %s)"
        cursor.execute(sql, (comment, current_user_id, moment_id))
        connection.commit()

        return {"status": True, "msg": "Comment published successfully."}

    except mysql.connector.Error as err:
        return handle_db_error(err, "Publish comment")
    
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()
        
def view_groups(current_user_id: str) -> dict:
    connection = get_connection()
    if connection is None:
        return {"status": False, "msg": "Database connection failed."}
    try:
        cursor = connection.cursor(dictionary=True)
        cursor.execute("SET ROLE Role_user;")
        sql = "SELECT group_id, group_name FROM FriendGroup WHERE created_by = %s"
        cursor.execute(sql, (current_user_id,))
        results = cursor.fetchall()
        return {"status": True, "msg": "Groups retrieved successfully", "data": results}
    except mysql.connector.Error as err:
        return handle_db_error(err, "View groups")
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()

def get_user_profile(current_user_id: str) -> dict:
    connection = get_connection()
    if connection is None:
        return {"status": False, "msg": "Database connection failed."}

    try:
        cursor = connection.cursor(dictionary=True)
        cursor.execute("SET ROLE Role_user;")
        
        sql = "SELECT user_id, name, gender, birth_date, age FROM User WHERE user_id = %s"
        cursor.execute(sql, (current_user_id,))
        result = cursor.fetchone()
        
        if result is None:
            return {"status": False, "msg": "User not found."}
            
        return {"status": True, "msg": "Profile retrieved successfully.", "data": result}
    
    except mysql.connector.Error as err:
        return handle_db_error(err, "Get user's profile")
    
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()

def send_message(sender_id: str, receiver_id: str, content: str) -> dict:
    if not content or content.strip() == "":
        return {"status": False, "msg": "Message content cannot be empty."}
    
    if len(content) > 200:
        return {"status": False, "msg": "Message content exceeds 200 characters."}

    connection = get_connection()
    if connection is None:
        return {"status": False, "msg": "Database connection failed."}

    try:
        cursor = connection.cursor()
        cursor.execute("SET ROLE Role_user;")

        # Check if the receiver is a friend
        check_sql = "SELECT 1 FROM Friendship WHERE user_id = %s AND friend_id = %s"
        cursor.execute(check_sql, (sender_id, receiver_id))
        if not cursor.fetchone():
            return {"status": False, "msg": "You can only send messages to your friends."}

        sql = "INSERT INTO Message (sender_id, receiver_id, content) VALUES (%s, %s, %s)"
        cursor.execute(sql, (sender_id, receiver_id, content))
        connection.commit()
        return {"status": True, "msg": "Message sent successfully."}

    except mysql.connector.Error as err:
        return handle_db_error(err, "Send message")
    
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()

def view_messages_with_friend(user_id: str, friend_id: str) -> dict:
    connection = get_connection()
    if connection is None:
        return {"status": False, "msg": "Database connection failed."}

    try:
        cursor = connection.cursor(dictionary=True)
        cursor.execute("SET ROLE Role_user;")
        
        sql = """
            SELECT message_id, sender_id, receiver_id, content, created_at 
            FROM Message 
            WHERE (sender_id = %s AND receiver_id = %s) 
               OR (sender_id = %s AND receiver_id = %s)
            ORDER BY created_at ASC
        """
        cursor.execute(sql, (user_id, friend_id, friend_id, user_id))
        results = cursor.fetchall()

        return {"status": True, "msg": "Messages retrieved successfully.", "data": results}

    except mysql.connector.Error as err:
        return handle_db_error(err, "View messages")
    
    finally:
        if 'cursor' in locals():
            cursor.close()
        connection.close()
