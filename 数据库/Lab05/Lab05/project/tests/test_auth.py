import sys
sys.path.append("..")  

from auth import register_user, login
from db_config import get_connection

def setup_test_db():
    print("\n--- Cleaning up and Setting up test environment ---")
    conn = get_connection()
    if conn:
        try:
            cursor = conn.cursor()
            # Clean up test users
            cursor.execute("DELETE FROM User WHERE user_id LIKE 'test_user%';")
            conn.commit()
            print("Cleanup successful. Database is ready.")
        except Exception as e:
            print(f"Setup failed: {e}")
        finally:
            cursor.close()
            conn.close()
    else:
        print("Failed to connect for setup.")
    print("---------------------------------------------------\n")

def test_registration_success():
    print("\n--- Test Registration Success ---")
    res = register_user('test_user1', 'pass123', 'Tom', 'male', '2000-01-01')
    print("Normal reg (Should succeed):", res)

def test_registration_fail():
    print("\n--- Test Registration Fail ---")
    # 1. Primary key conflict
    res1 = register_user('test_user1', 'pass456', 'Tommy', 'male', '2000-01-01')
    print("Duplicate reg (Should fail):", res1)
    
    # 2. Data too long (Error 1406)
    res2 = register_user('test_user2', 'this_password_is_way_too_long_for_db_column', 'Jerry', 'male', '2001-01-01')
    print("Password too long (Should fail):", res2)
    
    # 3. Check constraint violation (Error 3819)
    res3 = register_user('test_user3', 'pass789', 'Alice', 'alien', '2002-01-01')
    print("Invalid gender (Should fail):", res3)

    # 4. Null violation (Error 1048)
    res4 = register_user('test_user4', None, 'Bob', 'male', '2003-01-01')
    print("Null password (Should fail):", res4)

    # 4. Null violation (Error 1048)
    res5 = register_user('test_user5', 'pass789', '', 'male', '2003-01-01')
    print("Null password (Should fail):", res5)

def test_login_success():
    print("\n--- Test Login Success ---")
    # Ensure user exists (in case it wasn't created by reg success)
    register_user('test_user_logged', 'pass123', 'LoginUser', 'male', '1990-05-05')
    res = login('test_user_logged', 'pass123', 'Role_user')
    print("Valid login (Should succeed):", res)

def test_login_fail():
    print("\n--- Test Login Fail ---")
    # 1. Invalid password
    res1 = login('test_user1', 'wrong_pass', 'Role_user')
    print("Invalid password (Should fail):", res1)
    
    # 2. Invalid user_type
    res2 = login('test_user1', 'pass123', 'wrong_user')
    print("Invalid user_type (Should fail):", res2)

    # 3. Non-existent user
    res3 = login('test_ghost', 'pass123', 'Role_user')
    print("Non-existent user (Should fail):", res3)

def run_tests():
    setup_test_db()
    
    test_registration_success()
    test_registration_fail()
    
    test_login_success()
    test_login_fail()

if __name__ == '__main__':
    run_tests()
