import sys
sys.path.append("..")  
from user_controller import *
from admin_controller import * 
from auth import register_user
from db_config import get_connection

def setup_test_db():
    print("\n--- Cleaning up and Setting up test environment ---")
    conn = get_connection()
    if conn:
        try:
            cursor = conn.cursor()
            # Clean up all users to ensure an empty state initially
            cursor.execute("DELETE FROM User WHERE user_id IN ('test_u1', 'test_u2', 'test_u3', 'test_u4');")
            
            # Ensure admin001 exists as required by tests
            cursor.execute("""
                INSERT IGNORE INTO Admin (admin_id, password, name, phone_number, email) 
                VALUES ('admin001', 'admin123', 'Zhao Xinrui', '00000000000', 'admin@system.com');
            """)
            
            conn.commit()
            print("Cleanup and Admin setup successful. Database is ready.")

            # Create test users
            register_user('test_u1', 'pass1', 'User One', 'male', '2000-01-01')
            register_user('test_u2', 'pass2', 'User Two', 'female', '2001-02-02')
            register_user('test_u3', 'pass3', 'User Three', 'male', '2002-03-03')
            print("Inserted test users: test_u1, test_u2, test_u3.")

        except Exception as e:
            print(f"Setup failed: {e}")

        finally:
            cursor.close()
            conn.close()
    else:
        print("Failed to connect for setup.")
    print("---------------------------------------------------\n")

def test_update_admin_profile_success():
    print("\n--- Test update_admin_profile_success ---")
    print("Normal update (Should succeed):", update_admin_profile('admin001', 'New Name', '13812345678', 'new@email.com'))

def test_update_admin_profile_fail():
    print("\n--- Test update_admin_profile_fail ---")
    # Admin table schema: name VARCHAR(40) NOT NULL, phone_number VARCHAR(11) NOT NULL, email VARCHAR(20)
    print("Update with empty phone number (Should fail):", update_admin_profile('admin001', 'New Name', '', 'new@email.com'))
    print("Update with empty name (Should fail):", update_admin_profile('admin001', '', '13812345678', 'new@email.com'))
    print("Update with too long name (Should fail):", update_admin_profile('admin001', 'A' * 41, '13812345678', 'new@email.com'))
    print("Update with too long phone number (Should fail):", update_admin_profile('admin001', 'New Name', '1' * 12, 'new@email.com'))
    print("Update with too long email (Should fail):", update_admin_profile('admin001', 'New Name', '13812345678', 'e' * 21))
    print("Update non-existent user (Should fail):", update_admin_profile('test_ghost', 'Ghost', '13812345678', 'ghost@email.com'))


def test_delete_user_success():
    print("\n--- Test delete_user_success ---")
    print("Delete existing user (Should succeed):", delete_user('test_u1'))

def test_delete_user_fail():
    print("\n--- Test delete_user_fail ---")
    print("Delete non-existent user (Should fail):", delete_user('test_u1'))

def test_view_moment_success():
    print("\n--- Test view_moment_success ---")
    # Need to ensure some moments exist
    conn = get_connection()
    cursor = conn.cursor()
    cursor.execute("INSERT IGNORE INTO Moment (moment_content, user_id) VALUES ('Hello world', 'test_u2')")
    conn.commit()
    cursor.close()
    conn.close()
    
    result = view_moment()
    print("View moments (Should succeed):", result['status'], result['msg'])
    if result['status'] and result['data']:
        print("First moment entry:", result['data'][0])

def test_delete_moment_success():
    print("\n--- Test delete_moment_success ---")
    # Insert a moment to delete
    conn = get_connection()
    cursor = conn.cursor()
    cursor.execute("INSERT INTO Moment (moment_content, user_id) VALUES ('Delete me', 'test_u3')")
    conn.commit()
    moment_id = cursor.lastrowid
    cursor.close()
    conn.close()

    print(f"Delete existing moment ID {moment_id} (Should succeed):", delete_moment(moment_id))

def test_delete_moment_fail():
    print("\n--- Test delete_moment_fail ---")
    print("Delete non-existent moment (Should fail):", delete_moment(999999))

def run_tests():
    setup_test_db()

    test_update_admin_profile_success()
    test_update_admin_profile_fail()
    test_delete_user_success()
    test_delete_user_fail()
    test_view_moment_success()
    test_delete_moment_success()
    test_delete_moment_fail()

if __name__ == "__main__":
    run_tests()