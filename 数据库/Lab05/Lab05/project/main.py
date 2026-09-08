from auth import login, register_user
from admin_controller import * 
from user_controller import * 

import sys
from auth import login, register_user
from user_controller import *
from admin_controller import *

def main_menu():
    while True:
        print("\n===== Mini-WeChat Started =====")
        print("1. User Register")
        print("2. User Login")
        print("3. Administrator Login")
        print("4. Exit")
        choice = input("Please input your option: ")

        if choice == '1':
            handle_register()
        elif choice == '2':
            user_auth = handle_login("Role_user")
            if user_auth['status']:
                user_loop(user_auth['id'])
        elif choice == '3':
            admin_auth = handle_login("Role_admin")
            if admin_auth['status']:
                admin_loop(admin_auth['id'])
        elif choice == '4':
            print("Thanks for using Mini-WeChat, goodbye!")
            break

def handle_register():
    print("\n--- User Register ---")
    uid = input("ID(No more than 50 characters): ")
    pwd = input("Password(No more than 20 characters): ")
    name = input("Name(No more than 40 characters): ")
    gender = input("Gender (male/female): ")
    birth = input("生日 (YYYY-MM-DD): ")
    res = register_user(uid, pwd, name, gender, birth)
    print(res['msg'])

def handle_login(role):
    uid = input("ID: ")
    pwd = input("密码: ")
    res = login(uid, pwd, role)
    print(res['msg'])
    return res

def user_loop(user_id):
    while True:
        print(f"\n--- User Menu (ID: {user_id}) ---")
        print("1. Update Personal Profile")
        print("2. Friend Management")
        print("3. Moments Management")
        print("4. Logout")
        choice = input("Please select: ")
        
        if choice == '1':
            name = input("New Name: ")
            gender = input("New Gender (male/female): ")
            birth = input("New Birth Date (YYYY-MM-DD): ")
            print(update_user_profile(user_id, name, gender, birth)['msg'])
        elif choice == '2':
            friend_management_menu(user_id)
        elif choice == '3':
            moments_management_menu(user_id)
        elif choice == '4':
            break

def friend_management_menu(user_id):
    while True:
        print("\n--- Friend Management ---")
        print("1. View All Friends")
        print("2. Find Friend by Name")
        print("3. Add Friend (by ID)")
        print("4. Delete Friend")
        print("5. Create Friend Group")
        print("6. Delete Friend Group")
        print("7. Move Friend to Group")
        print("8. Back")
        choice = input("Please select: ")

        if choice == '1':
            res = view_friends(user_id)
            if res['status']:
                for f in res['data']: print(f)
            else: print(res['msg'])
        elif choice == '2':
            name = input("Input friend name to search: ")
            res = find_friend(user_id, name)
            if res['status']:
                for f in res['data']: print(f)
            else: print(res['msg'])
        elif choice == '3':
            fid = input("Input friend ID to add: ")
            print(add_friend(user_id, fid)['msg'])
        elif choice == '4':
            fid = input("Input friend ID to delete: ")
            print(delete_friend(user_id, fid)['msg'])
        elif choice == '5':
            gname = input("New group name: ")
            print(create_group(gname, user_id)['msg'])
        elif choice == '6':
            gname = input("Group name to delete: ")
            print(delete_group(gname, user_id)['msg'])
        elif choice == '7':
            fid = input("Friend ID: ")
            gid = input("Group ID: ")
            print(manage_friend(int(gid), user_id, fid)['msg'])
        elif choice == '8':
            break

def moments_management_menu(user_id):
    while True:
        print("\n--- Moments Management ---")
        print("1. Publish a Moment")
        print("2. Delete a Moment")
        print("3. View My Moments")
        print("4. Update my Moment")
        print("5. View Friends' Moments")
        print("6. Post a Comment")
        print("7. Back")
        choice = input("Please select: ")

        if choice == '1':
            content = input("Content (Max 600 chars): ")
            print(publish_moment(content, user_id)['msg'])
        elif choice == '2':
            mid = input("Moment ID to delete: ")
            print(delete_moment(int(mid), user_id)['msg'])
        elif choice == '3':
            res = view_my_moments(user_id)
            if res['status']:
                for m in res['data']: print(m)
            else: print(res['msg'])
        elif choice == '4':
            mid = input("Moment ID to update: ")
            new_content = input("New content: ")
            print(update_moment(int(mid), user_id, new_content)['msg'])
        elif choice == '5':
            res = view_friends_moments(user_id)
            if res['status']:
                for m in res['data']: print(m)
            else: print(res['msg'])
        elif choice == '6':
            mid = input("Moment ID to comment on: ")
            content = input("Comment content: ")
            print(publish_comment(user_id, int(mid), content)['msg'])
        elif choice == '7':
            break

def admin_loop(admin_id):
    while True:
        print(f"\n--- Administrator Menu (ID: {admin_id}) ---")
        print("1. Update your profile: ")
        print("2. Inspect all moments")
        print("3. Delete certain user")
        print("4. Logout")
        choice = input("Please input your choice: ")
        
        if choice == '1':
            name = input("New name(No more than 40 characters): ")
            phone = input("New phone number(No more than 11 characters): ")
            email = input("New email(No more than 20 characters): ")
            print(update_admin_profile(admin_id, name, phone, email)['msg'])
        elif choice == '2':
            res = view_moment()
            if res['status']:
                for m in res['data']: print(m)
        elif choice == '3':
            target_uid = input("Input the user's ID you are going to delete: ")
            print(delete_user(target_uid)['msg'])
        elif choice == '4':
            break

if __name__ == "__main__":
    main_menu()