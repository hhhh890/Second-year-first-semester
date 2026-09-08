import mysql.connector

def handle_db_error(err: mysql.connector.Error, action_name: str = "Operation") -> dict:
    """
    Abstracted database error handler to provide precise and user-friendly error messages.
    """
    msg_lower = err.msg.lower()

    if err.errno == 1062: 
        if 'user.primary' in msg_lower or 'admin.primary' in msg_lower:
            return {"status": False, "msg": f"{action_name} failed: User ID is already registered."}
        elif 'friendship.primary' in msg_lower:
            return {"status": False, "msg": f"{action_name} failed: Your friend is already added."}
        else:
            return {"status": False, "msg": f"{action_name} failed: Duplicate entry."}
        
    elif err.errno == 1406: 
        msg_lower = err.msg.lower()
        if 'password' in msg_lower:
            return {"status": False, "msg": f"{action_name} failed: Password exceeds the maximum length of 20 characters."}
        elif 'name' in msg_lower:
            return {"status": False, "msg": f"{action_name} failed: Name exceeds the maximum length of 40 characters."}
        elif 'user_id' in msg_lower or 'admin_id' in msg_lower:
            return {"status": False, "msg": f"{action_name} failed: ID exceeds the maximum length 50 characters."}
        elif 'group_name' in msg_lower:
            return {"status": False, "msg": f"{action_name} failed: Group name exceeds the maximum length of 40 characters."}
        elif 'phone_number' in msg_lower:
            return {"status": False, "msg": f"{action_name} failed: Phone number exceeds the maximum length of 11 characters."}
        elif 'email' in msg_lower:
            return {"status": False, "msg": f"{action_name} failed: Email exceeds the maximum length of 20 characters."}
        elif 'moment_content' in msg_lower:
            return {"status": False, "msg": f"{action_name} failed: Moment content exceeds the maximum limit of 600 characters."}
        elif 'comment_content' in msg_lower:
            return {"status": False, "msg": f"{action_name} failed: Comment exceeds the maximum limit of 200 characters."}
        else:
            return {"status": False, "msg": f"{action_name} failed: Input data exceeds the maximum length."}
            
    elif err.errno == 3819: 
        if 'chk_user_gender' in msg_lower:
            return {"status": False, "msg": f"{action_name} failed: Gender must be 'male' or 'female'."}
        else:
            return {"status": False, "msg": f"{action_name} failed: Check constraint violated."}
        
    elif err.errno == 1048:  
        if 'password' in msg_lower:
            return {"status": False, "msg": f"{action_name} failed: Password cannot be empty."}
        elif 'group_name' in msg_lower:
            return {"status": False, "msg": f"{action_name} failed: Group name cannot be empty."}
        elif 'name' in msg_lower:
            return {"status": False, "msg": f"{action_name} failed: Name cannot be empty."}
        elif 'phone_number' in msg_lower:
            return {"status": False, "msg": f"{action_name} failed: Phone number cannot be empty."}
        elif 'moment' in msg_lower:
            return {"status": False, "msg": f"{action_name} failed: Moment content cannot be empty."}
        elif 'comment' in msg_lower:
            return {"status": False, "msg": f"{action_name} failed: Comment content cannot be empty."}
        else:
            return {"status": False, "msg": f"{action_name} failed: A required field cannot be empty."}
        
    elif err.errno == 1452: 
        if 'moment_id' in msg_lower:
            return {"status": False, "msg": f"{action_name} failed: Target moment does not exist."}
        elif 'friendship' or 'friendgroup' or 'moment' in msg_lower:
            return {"status": False, "msg": f"{action_name} failed: Target user does not exist."}
        else:
            return {"status": False, "msg": f"{action_name} failed: Referenced record does not exist."}
        
    else:
        print(f"DEBUG: Unexpected error {err.errno}: {err.msg}")
        return {"status": False, "msg": f"{action_name} failed: System busy, please try again later.)"}