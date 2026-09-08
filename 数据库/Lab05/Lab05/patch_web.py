import os

path = 'project/web_main.py'
if not os.path.exists(path):
    path = 'web_main.py'

with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

# Core menu definitions replacement
# User menu
content = content.replace(
    'menu = st.radio("Menu", ["馃彔 Moments Feed", "馃懃 Friends & Groups", "鈿欙笍 My Profile"])',
    'menu = st.radio("Menu", ["🏠 Moments Feed", "👥 Friends & Groups", "⚙️ My Profile"])'
)

# Admin menu
content = content.replace(
    'menu = st.radio("Menu", ["馃洝锔?Admin Audit", "馃懁 Manage Users", "鈿欙笍 My Profile"])',
    'menu = st.radio("Menu", ["🛡️ Admin Audit", "👤 Manage Users", "⚙️ My Profile"])'
)

# Logic checks replacement
# User logic
content = content.replace('if menu == "馃彔 Moments Feed":', 'if menu == "🏠 Moments Feed":')
content = content.replace('elif menu == "馃懃 Friends & Groups":', 'elif menu == "👥 Friends & Groups":')
content = content.replace('elif menu == "鈿欙笍 My Profile":', 'elif menu == "⚙️ My Profile":')

# Admin logic
content = content.replace('if menu == "馃洝锔?Admin Audit":', 'if menu == "🛡️ Admin Audit":')
content = content.replace('elif menu == "馃懁 Manage Users":', 'elif menu == "👤 Manage Users":')
content = content.replace('elif menu == "鈿欙笍 My Profile":', 'elif menu == "⚙️ My Profile":')

# Fix UI symbols
content = content.replace('page_icon="💬"', 'page_icon="💬"') # Ensure it's correct
content = content.replace('Mini-WeChat 馃挰', 'Mini-WeChat 💬')
content = content.replace('馃懁 {st.session_state.user_id}', '👤 {st.session_state.user_id}')
content = content.replace('st.button("馃毆 Logout"', 'st.button("🚪 Logout"')
content = content.replace('st.button("馃殌 Publish Moment"', 'st.button("🚀 Publish Moment"')
content = content.replace('tabs(["馃寧 Friends\' Moments"', 'tabs(["🌎 Friends\' Moments"')
content = content.replace('"馃懁 My Moments"', '"👤 My Moments"')
content = content.replace('st.subheader("鉃?Actions")', 'st.subheader("▶️ Actions")')
content = content.replace('st.subheader("馃攳 Search & List")', 'st.subheader("🔍 Search & List")')
content = content.replace('if st.button(f"馃棏锔?Unfriend', 'if st.button(f"🗑️ Unfriend')
content = content.replace('st.expander(f"馃懁 {f[\'name\']}', 'st.expander(f"👤 {f[\'name\']}')
content = content.replace('if st.button("馃棏锔?", key=f"del_{mid}"', 'if st.button("🗑️", key=f"del_{mid}"')
content = content.replace('with st.expander("馃摑 Update Content")', 'with st.expander("📝 Update Content")')
content = content.replace('with st.expander("馃挰 Add Comment")', 'with st.expander("💬 Add Comment")')
content = content.replace('st.warning("鈿狅笍 This action is irreversible")', 'st.warning("⚠️ This action is irreversible")')

# Success/Error fix
content = content.replace("st.error(f\"? {res['msg']}\")", "st.error(f\"❌ {res['msg']}\")")
content = content.replace('st.success("? Registration successful!', 'st.success("✅ Registration successful!')

with open(path, 'w', encoding='utf-8') as f:
    f.write(content)

print("Successfully patched web_main.py")
