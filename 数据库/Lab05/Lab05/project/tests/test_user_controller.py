import sys
sys.path.append("..")  
from user_controller import *
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
            conn.commit()
            print("Cleanup successful. Database is ready.")

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

def test_update_user_profile_success():
    print("\n--- Test update_user_profile_success ---")
    print("Normal update (Should succeed):", update_user_profile('test_u1', 'New Name', 'male', '2000-01-01'))

def test_update_user_profile_fail():
    print("\n--- Test update_profile_fail ---")
    print("Update with invalid gender (Should fail):", update_user_profile('test_u1', 'New Name', 'alien', '2000-01-01'))
    print("Update with empty name (Should fail):", update_user_profile('test_u1', '', 'male', '2000-01-01'))
    print("Update non-existent user (Should fail):", update_user_profile('test_ghost', 'Ghost', 'male', '2000-01-01'))

def test_add_friend_success():
    print("\n--- Test add_friend_success ---")
    print("Normal add friend (Should succeed):", add_friend('test_u1', 'test_u2'))

def test_add_friend_fail():
    print("\n--- Test add_friend_fail ---")
    print("Add friend already added (Should fail):", add_friend('test_u1', 'test_u2'))
    print("Add yourself (Should fail):", add_friend('test_u1', 'test_u1'))
    print("Add non-existent user (Should fail):", add_friend('test_u1', 'test_ghost'))

def test_view_friends_success():
    print("\n--- Test view_friends_success ---")
    print("View friends:", view_friends('test_u1'))

def test_find_friend_success():
    print("\n--- Test find_friend_success ---")
    print("Find friend (should succeed)", find_friend('test_u1', 'User Two'))

def test_find_friend_fail():
    print("\n--- Test find_friend_fail ---")
    print("Find friend not exists (should fail)", find_friend('test_u1', 'User Three'))

def test_manage_friend_success():
    print("\n--- Test manage_friend_success ---")
    # Setup: ensure u1 and u2 are friends and create group
    add_friend('test_u1', 'test_u2')
    create_group("Management Group", "test_u1")
    conn = get_connection()
    cursor = conn.cursor(dictionary=True)
    cursor.execute("SELECT group_id FROM FriendGroup WHERE group_name = 'Management Group' AND created_by = 'test_u1' LIMIT 1")
    group_id = cursor.fetchone()['group_id']
    cursor.close()
    conn.close()
    
    print(f"Manage friend (add u2 to group {group_id}):", manage_friend(group_id, 'test_u1', 'test_u2'))

def test_manage_friend_fail():
    print("\n--- Test manage_friend_fail ---")
    print("Manage friend to invalid group (Should fail)", manage_friend(9999, 'test_u1', 'test_u2'))

    create_group("Management Group", "test_u1")
    conn = get_connection()
    cursor = conn.cursor(dictionary=True)
    cursor.execute("SELECT group_id FROM FriendGroup WHERE group_name = 'Management Group' AND created_by = 'test_u1' LIMIT 1")
    group_id = cursor.fetchone()['group_id']
    print("Manage friend who is not your friend (Should fail)", manage_friend(group_id, 'test_u1', 'test_u3'))

def test_delete_friend_success():
    print("\n--- Test delete_friend_success ---")
    print("Normal delete (Should succeed):", delete_friend('test_u1', 'test_u2'))

def test_delete_friend_fail():
    print("\n--- Test delete_friend_fail ---")
    print("Delete non-existent friend (Should fail):", delete_friend('test_u1', 'test_u3'))
    print("Delete yourself (Should fail):", delete_friend('test_u1', 'test_u1'))

def test_create_group_success():
    print("\n--- Test create_group_success ---")
    print("Normal create (Should succeed):", create_group("My Besties", "test_u1"))

def test_create_group_fail():
    print("\n--- Test create_group_fail ---")
    print("Create group by non-existent user (Should fail):", create_group("Ghost Group", "test_ghost"))
    print("Create group with empty group name (Should fail):", create_group("", "test_u1"))

def test_delete_group_success():
    print("\n--- Test delete_group_success ---")
    try:
        print("Normal delete group (Should succeed)", delete_group("My Besties", 'test_u1')) 
    except NameError:
        print("delete_group is not implemented yet.")

def test_delete_group_fail():
    print("\n--- Test delete_group_fail ---")
    try:
        print("Delete non-existent group (Should fail)", delete_group("Empty Group", 'test_u1')) 
    except NameError:
        print("delete_group is not implemented yet.")

def test_publish_moment_success():
    print("\n--- Test publish_moment_success ---")
    try:
        print("Normal publish moment (Should succeed)", publish_moment('Hello World!', 'test_u1'))
    except NameError:
        print("publish_moment is not implemented yet.")

def test_publish_moment_fail():
    print("\n--- Test publish_moment_fail ---")
    try:
        print("Publish empty moment (Should fail):", publish_moment('', 'test_u1'))
        print("Publish too long moment (Should fail):", publish_moment('A' * 601, 'test_u1'))
    except NameError:
        print("publish_moment is not implemented yet.")

def test_update_moment_success():
    print("\n--- Test update_moment_success ---")
    try:
        conn = get_connection()
        cursor = conn.cursor(dictionary=True)
        cursor.execute("SELECT moment_id FROM Moment WHERE user_id = 'test_u1' LIMIT 1")
        res = cursor.fetchone()
        if res:
            moment_id = res['moment_id']
            print(f"Normal update moment (ID {moment_id}) (Should succeed)", update_moment(moment_id, 'test_u1', 'Updated content!'))
        else:
            print("No moment found to update.")
        cursor.close()
        conn.close()
    except NameError:
        print("update_moment is not implemented yet.")

def test_update_moment_fail():
    print("\n--- Test update_moment_fail ---")
    try:
        print("Update non-existent moment (Should fail)", update_moment(999, 'test_u1', 'New content'))

        conn = get_connection()
        cursor = conn.cursor(dictionary=True)
        cursor.execute("SELECT moment_id FROM Moment WHERE user_id = 'test_u1' LIMIT 1")
        res = cursor.fetchone()
        if res:
            moment_id = res['moment_id']
            print("Update moment by wrong user (Should fail)", update_moment(moment_id, 'test_ghost', 'New content'))
            print("Update with empty content (Should fail):", update_moment(moment_id, 'test_u1', ''))
        else:
            print("No moment found to update.")

        cursor.close()
        conn.close()
    except NameError:
        print("update_moment is not implemented yet.")

def test_delete_moment_success():
    print("\n--- Test delete_moment_success ---")
    try:
        # We need to find the actual moment_id we just created, as it's AUTO_INCREMENT (won't be 0 or 1 always properly aligned on test run).
        conn = get_connection()
        cursor = conn.cursor(dictionary=True)
        cursor.execute("SELECT moment_id FROM Moment WHERE user_id = 'test_u1' LIMIT 1")
        res = cursor.fetchone()
        if res:
            moment_id = res['moment_id']
            print(f"Normal delete moment (ID {moment_id}) (Should succeed)", delete_moment(moment_id, 'test_u1'))
        else:
            print("No moment found to delete (did publish fail?).")
        cursor.close()
        conn.close()
    except NameError:
        print("delete_moment is not implemented yet.")

def test_delete_moment_fail():
    print("\n--- Test delete_moment_fail ---")
    try:
        print("Delete non-existent moment (Should fail)", delete_moment(999, 'test_u1'))
    except NameError:
        print("delete_moment is not implemented yet.")

def test_view_my_moments_success():
    print("\n--- Test view_my_moments_success ---")
    print("View my moments (Should succeed)", view_my_moments('test_u1'))

def test_view_friends_moments_success():
    print("\n--- Test view_friends_moments_success ---")
    add_friend('test_u1', 'test_u2')
    publish_moment('Hello from u2!', 'test_u2')
    print("View moments of friends (Should succeed)", view_friends_moments('test_u1'))

def test_publish_comment_success():
    print("\n--- Test publish_comment_success ---")
    try:
        publish_moment('Moment to comment on', 'test_u1')
        
        conn = get_connection()
        cursor = conn.cursor(dictionary=True)
        cursor.execute("SELECT moment_id FROM Moment WHERE user_id = 'test_u1' ORDER BY updated_at DESC LIMIT 1")
        res = cursor.fetchone()
        if res:
            moment_id = res['moment_id']
            print("Normal publish comment (Should succeed):", publish_comment('test_u2', moment_id, 'Nice boat!'))
        else:
            print("No moment found to comment on.")
        cursor.close()
        conn.close()
    except NameError:
        print("publish_comment is not implemented yet.")

def test_publish_comment_fail():
    print("\n--- Test publish_comment_fail ---")
    try:
        print("Publish comment for non-existent moment (Should fail):", publish_comment('test_u1', 9999, 'Nice!'))

        publish_moment('Private moment', 'test_u3')
        conn = get_connection()
        cursor = conn.cursor(dictionary=True)
        cursor.execute("SELECT moment_id FROM Moment WHERE user_id = 'test_u3' ORDER BY updated_at DESC LIMIT 1")
        moment_id = cursor.fetchone()['moment_id']
        cursor.close()
        conn.close()
        print("Publish comment for non-friend moment (Should fail):", publish_comment('test_u1', moment_id, 'Stranger!'))

        conn = get_connection()
        cursor = conn.cursor(dictionary=True)
        cursor.execute("SELECT moment_id FROM Moment WHERE user_id = 'test_u1' LIMIT 1")
        my_moment_id = cursor.fetchone()['moment_id']
        cursor.close()
        conn.close()
        print("Publish empty comment (Should fail):", publish_comment('test_u1', my_moment_id, ''))

    except NameError:
        print("publish_comment is not implemented yet.")

def run_tests():
    setup_test_db()

    test_update_user_profile_success()
    test_update_user_profile_fail()

    test_add_friend_success()
    test_add_friend_fail()

    test_view_friends_success()
    
    test_find_friend_success()
    test_find_friend_fail()

    test_manage_friend_success()
    test_manage_friend_fail()

    test_delete_friend_success()
    test_delete_friend_fail()

    test_create_group_success()
    test_create_group_fail()

    test_delete_group_success()
    test_delete_group_fail()

    test_publish_moment_success()
    test_publish_moment_fail()

    test_update_moment_success()
    test_update_moment_fail()

    test_view_my_moments_success()
    test_delete_moment_success()
    test_delete_moment_fail()

    test_view_friends_moments_success()

    test_publish_comment_success()
    test_publish_comment_fail()

if __name__ == "__main__":
    run_tests()
