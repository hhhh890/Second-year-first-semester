import streamlit as st
from auth import login, register_user
from user_controller import *
from admin_controller import *
import pandas as pd
from datetime import datetime

# Page Configuration
st.set_page_config(page_title="Mini-WeChat", page_icon="💬", layout="wide")

# Custom CSS for a clean UI
st.markdown("""
    <style>
    .main { background-color: #f8f9fa; }
    .stApp { background-color: #f8f9fa; }
    
    /* Moment Styling */
    .moment-card {
        background: white;
        padding: 1.5rem;
        border-radius: 15px;
        box-shadow: 0 4px 12px rgba(0,0,0,0.05);
        margin-bottom: 1.5rem;
        border-left: 5px solid #07C160;
    }
    .moment-header {
        display: flex;
        justify-content: space-between;
        align-items: center;
        margin-bottom: 0.8rem;
    }
    .moment-author {
        color: #1e88e5;
        font-weight: bold;
        font-size: 1.1rem;
    }
    .moment-time {
        color: #9e9e9e;
        font-size: 0.85rem;
    }
    .moment-content {
        font-size: 1rem;
        line-height: 1.6;
        color: #333;
        margin-bottom: 1rem;
    }
    
    /* Comment Styling */
    .comment-section {
        background: #fdfdfd;
        border-top: 1px solid #eee;
        padding-top: 0.8rem;
        margin-top: 0.5rem;
    }
    .comment-item {
        padding: 0.5rem 0.8rem;
        border-radius: 8px;
        background: #f0f2f5;
        margin-bottom: 0.4rem;
        font-size: 0.9rem;
        position: relative;
    }
    .comment-author {
        font-weight: 600;
        color: #555;
    }
    .comment-time {
        float: right;
        color: #9e9e9e;
        font-size: 0.75rem;
    }
    
    /* Sidebar user profile */
    .nav-header {
        text-align: center;
        padding: 1rem 0;
        border-bottom: 1px solid #ddd;
        margin-bottom: 1rem;
    }
    
    /* WeChat Green Color */
    .stButton>button {
        border-radius: 20px;
    }
    </style>
""", unsafe_allow_html=True)

def main():
    if 'logged_in' not in st.session_state:
        st.session_state.logged_in = False
        st.session_state.user_id = None
        st.session_state.role = None

    if not st.session_state.logged_in:
        show_auth_page()
    else:
        show_main_app()

def show_auth_page():
    st.markdown("<h1 style='text-align: center; color: #07C160;'>Mini-WeChat 💬</h1>", unsafe_allow_html=True)
    col1, col2, col3 = st.columns([1, 2, 1])
    
    with col2:
        tab1, tab2 = st.tabs(["Login", "Register"])
        
        with tab1:
            with st.form("login_form"):
                role_choice = st.selectbox("Identity", ["User", "Administrator"])
                uid = st.text_input("User ID / Admin ID")
                pwd = st.text_input("Password", type="password")
                submit = st.form_submit_button("Sign In", use_container_width=True)
                if submit:
                    target_role = "Role_user" if role_choice == "User" else "Role_admin"
                    res = login(uid, pwd, target_role)
                    if res['status']:
                        st.session_state.logged_in = True
                        st.session_state.user_id = uid
                        st.session_state.role = target_role
                        st.rerun()
                    else:
                        st.error(f"❌ {res['msg']}")

        with tab2:
            st.info("Join the Mini-WeChat community!")
            with st.form("reg_form"):
                new_uid = st.text_input("Choose ID")
                new_pwd = st.text_input("Choose Password", type="password")
                new_name = st.text_input("Full Name")
                new_gender = st.selectbox("Gender", ["male", "female", "other"])
                new_birth = st.date_input("Birth Date", min_value=datetime(1940, 1, 1), max_value=datetime.now())
                reg_submit = st.form_submit_button("Create Account", use_container_width=True)
                if reg_submit:
                    res = register_user(new_uid, new_pwd, new_name, new_gender, str(new_birth))
                    if res['status']:
                        st.success("✅ Registration successful! Please head to Login tab.")
                    else:
                        st.error(f"❌ {res['msg']}")

def show_main_app():
    with st.sidebar:
        st.markdown(f"""
        <div class='nav-header'>
            <h2 style='margin:0;'>👤 {st.session_state.user_id}</h2>
            <small style='color:#07C160;'>{st.session_state.role.replace('Role_', '').capitalize()}</small>
        </div>
        """, unsafe_allow_html=True)
        
        if st.session_state.role == "Role_user":
            menu = st.radio("Menu", ["🏠 Moments Feed", "👥 Friends & Groups", "💬 Private Messages", "⚙️ My Profile"])
        else:
            menu = st.radio("Menu", ["🛡️ Admin Audit", "👤 Manage Users", "⚙️ My Profile"])
            
        st.divider()
        if st.button("🚪 Logout", use_container_width=True):
            st.session_state.logged_in = False
            st.rerun()

    if st.session_state.role == "Role_user":
        handle_user_logic(menu)
    else:
        handle_admin_logic(menu)

def handle_user_logic(menu):
    uid = st.session_state.user_id
    if menu == "🏠 Moments Feed":
        st.title("Moments Feed")
        
        # New Post Section
        with st.container():
            st.markdown("<div class='moment-card'>", unsafe_allow_html=True)
            content = st.text_area("What's on your mind today?", label_visibility="collapsed", placeholder="What's on your mind today?")
            if st.button("🚀 Publish Moment", use_container_width=True):
                if content:
                    res = publish_moment(content, uid)
                    st.toast(res['msg'])
                    if res['status']: st.rerun()
                else: st.warning("Content cannot be empty")
            st.markdown("</div>", unsafe_allow_html=True)
            
        st.divider()
        feed_tab, my_tab = st.tabs(["🌎 Friends' Moments", "👤 My Moments"])
        with feed_tab: 
            display_moments_grouped(view_friends_moments(uid), uid)
        with my_tab: 
            display_moments_grouped(view_my_moments(uid), uid, show_actions=True)

    elif menu == "👥 Friends & Groups":
        col1, col2 = st.columns([2, 1])
        
        with col2:
            st.subheader("▶️ Actions")
            action = st.selectbox("Action", ["Add Friend", "Create Group", "Manage Friend Group"])
            
            if action == "Add Friend":
                fid = st.text_input("Target User ID")
                if st.button("Add Friend", use_container_width=True):
                    res = add_friend(uid, fid)
                    if res['status']: st.success(res['msg'])
                    else: st.error(res['msg'])
            
            elif action == "Create Group":
                gn = st.text_input("Group Name")
                if st.button("Create Group", use_container_width=True):
                    res = create_group(gn, uid)
                    if res['status']: st.success(res['msg'])
                    else: st.error(res['msg'])
            
            elif action == "Manage Friend Group":
                friends_res = view_friends(uid)
                groups_res = view_groups(uid)
                if friends_res['status'] and friends_res['data']:
                    f_list = {f['friend_id']: f['name'] for f in friends_res['data']}
                    g_list = {g['group_id']: g['group_name'] for g in groups_res['data']} if groups_res['status'] and groups_res['data'] else {}
                    
                    target_f = st.selectbox("Select Friend", options=list(f_list.keys()), format_func=lambda x: f"{f_list[x]} ({x})")
                    
                    if not g_list:
                        st.warning("You haven't created any groups yet.")
                    else:
                        target_g = st.selectbox("Select Group", options=list(g_list.keys()), format_func=lambda x: g_list[x])
                        if st.button("Move to Group", use_container_width=True):
                            res = manage_friend(target_g, uid, target_f)
                            if res['status']: st.success(res['msg'])
                            else: st.error(res['msg'])
                else:
                    st.info("No friends available to manage.")

        with col1:
            st.subheader("🔍 Search & List")
            s_name = st.text_input("Search friends by name...")
            if s_name:
                display_friends_rich(find_friend(uid, s_name))
            else:
                display_friends_rich(view_friends(uid), deletable=True)

    elif menu == "💬 Private Messages":
        st.title("Messages")
        friends_res = view_friends(uid)
        
        if not friends_res['status'] or not friends_res['data']:
            st.info("You don't have any friends to message yet. Go to 'Friends & Groups' to add some!")
        else:
            friend_list = {f['friend_id']: f['name'] for f in friends_res['data']}
            
            # WeChat-like layout: Contacts on left, chat on right
            col_list, col_chat = st.columns([1, 2.5])
            
            with col_list:
                st.write("### 👥 Chats")
                selected_friend_id = st.radio(
                    "Select Friend", 
                    options=list(friend_list.keys()), 
                    format_func=lambda x: f"{friend_list[x]}",
                    label_visibility="collapsed"
                )
                
            with col_chat:
                if selected_friend_id:
                    st.subheader(f"💬 {friend_list[selected_friend_id]}")
                    st.divider()
                    
                    # Scrollable chat history container
                    chat_container = st.container(height=450)
                    
                    msg_res = view_messages_with_friend(uid, selected_friend_id)
                    with chat_container:
                        if msg_res['status'] and msg_res['data']:
                            for m in msg_res['data']:
                                is_me = m['sender_id'] == uid
                                align = "flex-end" if is_me else "flex-start"
                                bg_color = "#95ec69" if is_me else "#ffffff" # WeChat Green / White
                                
                                st.markdown(f"""
                                    <div style="display: flex; flex-direction: column; align-items: {align}; margin-bottom: 12px;">
                                        <div style="background-color: {bg_color}; padding: 8px 12px; border-radius: 6px; position: relative; max-width: 80%; box-shadow: 0 1px 2px rgba(0,0,0,0.1); word-wrap: break-word; color: #000;">
                                            {m['content']}
                                        </div>
                                        <small style="color: #999; margin: 2px 5px; font-size: 0.7rem;">
                                            {m['created_at'].strftime('%H:%M') if hasattr(m['created_at'], 'strftime') else str(m['created_at'])}
                                        </small>
                                    </div>
                                """, unsafe_allow_html=True)
                        else:
                            st.caption("No messages yet. Send a message to start the conversation!")

                    # Message Input Form
                    with st.form("send_msg_form", clear_on_submit=True):
                        # Use a text area for better input, WeChat style
                        new_msg_text = st.text_area("Type your message...", placeholder="Enter message here (max 200 chars)", label_visibility="collapsed", height=80)
                        c1, c2 = st.columns([5, 1])
                        with c2:
                            send_btn = st.form_submit_button("Send", use_container_width=True)
                        
                        if send_btn:
                            if new_msg_text.strip():
                                res = send_message(uid, selected_friend_id, new_msg_text)
                                if res['status']:
                                    st.rerun()
                                else:
                                    st.error(res['msg'])
                            else:
                                st.warning("Cannot send empty message")

    elif menu == "⚙️ My Profile":
        st.subheader("Update Personal Information")
        profile_res = get_user_profile(uid)
        if profile_res['status']:
            user_data = profile_res['data']
            with st.form("profile_form"):
                new_name = st.text_input("Name", value=user_data['name'])
                
                gender_options = ["male", "female", "other"]
                try:
                    gender_index = gender_options.index(user_data['gender'])
                except (ValueError, KeyError):
                    gender_index = 0
                new_gender = st.selectbox("Gender", gender_options, index=gender_index)
                
                birth_date = user_data['birth_date']
                # streamlit date_input expects date object
                if isinstance(birth_date, str):
                    birth_date = datetime.strptime(birth_date, '%Y-%m-%d').date()
                
                new_birth = st.date_input("Birth Date", value=birth_date, min_value=datetime(1900, 1, 1), max_value=datetime.now())
                
                if st.form_submit_button("Save Changes"):
                    res = update_user_profile(uid, new_name, new_gender, str(new_birth))
                    if res['status']: 
                        st.success(res['msg'])
                        st.rerun()
                    else: 
                        st.error(res['msg'])
            
            st.info(f"User ID: {user_data['user_id']} | Current Age: {user_data['age']}")
        else:
            st.error(profile_res['msg'])

def display_friends_rich(res, deletable=False):
    if not res['status'] or not res['data']:
        st.info("No friends found.")
        return
    
    for f in res['data']:
        with st.expander(f"👤 {f['name']} (@{f['friend_id']})"):
            c1, c2 = st.columns(2)
            with c1:
                st.write(f"**Gender:** {f['gender']}")
                st.write(f"**Age:** {f['age']}")
            with c2:
                st.write(f"**Birth:** {f['birth_date']}")
                st.write(f"**Group:** {f['group_name'] if f['group_name'] else 'Unassigned'}")
            
            if deletable:
                if st.button(f"🗑️ Unfriend {f['name']}", key=f"del_{f['friend_id']}"):
                    del_res = delete_friend(st.session_state.user_id, f['friend_id'])
                    if del_res['status']: 
                        st.success("Deleted!")
                        st.rerun()

def display_moments_grouped(res, current_uid, show_actions=False):
    if not res['status'] or not res['data']:
        st.info("Nothing here yet.")
        return
    
    # Group results by moment_id
    moments = {}
    for row in res['data']:
        mid = row['moment_id']
        if mid not in moments:
            moments[mid] = {
                'id': row['moment_id'],
                'content': row['moment_content'],
                'author': row['moment_user_id'],
                'time': row['updated_at'],
                'comments': []
            }
        if row['comment_id']:
            moments[mid]['comments'].append({
                'author': row['comment_user_id'],
                'content': row['comment_content'],
                'time': row['created_at']
            })
            
    for mid in sorted(moments.keys(), reverse=True):
        m = moments[mid]
        # Format moment time
        m_time_str = m['time'].strftime('%m-%d %H:%M') if hasattr(m['time'], 'strftime') else str(m['time'])
        
        st.markdown(f"""
            <div class='moment-card'>
                <div class='moment-header'>
                    <span class='moment-author'>@{m['author']}</span>
                    <span class='moment-time'>{m_time_str}</span>
                </div>
                <div class='moment-content'>{m['content']}</div>
        """, unsafe_allow_html=True)
        
        # Actions for own moments
        if show_actions:
            c1, c2 = st.columns([5, 1])
            with c2:
                if st.button("🗑️", key=f"del_{mid}", help="Delete Moment"):
                    delete_moment(mid, current_uid)
                    st.rerun()
            with c1:
                with st.expander("📝 Update Content"):
                    new_c = st.text_area("New content", value=m['content'], key=f"upd_txt_{mid}")
                    if st.button("Update", key=f"upd_btn_{mid}"):
                        update_moment(mid, current_uid, new_c)
                        st.rerun()
        
        # Comments
        if m['comments']:
            st.markdown("<div class='comment-section'>", unsafe_allow_html=True)
            for c in m['comments']:
                c_time_str = c['time'].strftime('%m-%d %H:%M') if hasattr(c['time'], 'strftime') else str(c['time'])
                st.markdown(f"""
                    <div class='comment-item'>
                        <span class='comment-author'>@{c['author']}:</span> {c['content']}
                        <div class='comment-time'>{c_time_str}</div>
                    </div>
                """, unsafe_allow_html=True)
            st.markdown("</div>", unsafe_allow_html=True)
            
        # Add Comment
        with st.expander("💬 Add Comment"):
            c_text = st.text_input("Comment", key=f"c_in_{mid}", label_visibility="collapsed")
            if st.button("Reply", key=f"c_btn_{mid}"):
                res = publish_comment(current_uid, mid, c_text)
                if res['status']: st.rerun()
                else: st.error(res['msg'])
                
        st.markdown("</div>", unsafe_allow_html=True)

def handle_admin_logic(menu):
    if menu == "🛡️ Admin Audit":
        st.title("Admin Moment Audit")
        res = view_moment()
        if res['status'] and res['data']:
            df = pd.DataFrame(res['data'], columns=['ID', 'Content', 'User ID', 'Last Update', 'Sample Comment'])
            st.dataframe(df, use_container_width=True)
            
            st.subheader("Perform Deletion")
            target_mid = st.number_input("Enter Moment ID to delete", step=1)
            if st.button("Delete Illicit Moment", use_container_width=True):
                from admin_controller import delete_moment as adm_del
                res = adm_del(target_mid)
                if res['status']: st.success(res['msg'])
                else: st.error(res['msg'])
        else:
            st.info("No moments to review.")
            
    elif menu == "👤 Manage Users":
        st.title("User Management")
        u_id = st.text_input("Enter User ID to remove from system")
        st.warning("⚠️ This action is irreversible and will delete all user data.")
        if st.button("Confirm Delete User", use_container_width=True):
            if u_id:
                res = delete_user(u_id)
                if res['status']: st.success(res['msg'])
                else: st.error(res['msg'])
            else:
                st.info("Please enter a User ID.")
                
    elif menu == "⚙️ My Profile":
        st.subheader("Update Admin Information")
        admin_id = st.session_state.user_id
        profile_res = get_admin_profile(admin_id)
        
        if profile_res['status']:
            data = profile_res['data']
            with st.form("admin_profile_form"):
                new_name = st.text_input("Name", value=data['name'])
                new_phone = st.text_input("Phone Number", value=data['phone_number'])
                new_email = st.text_input("Email", value=data['email'] if data['email'] else "")
                
                if st.form_submit_button("Save Changes"):
                    res = update_admin_profile(admin_id, new_name, new_phone, new_email)
                    if res['status']: 
                        st.success(res['msg'])
                        st.rerun()
                    else: 
                        st.error(res['msg'])
        else:
            st.error(profile_res['msg'])

if __name__ == "__main__":
    main()

