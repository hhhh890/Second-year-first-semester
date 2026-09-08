# Test 用例
## 用户注册
* duplicate user_id
* max size password
* empty password
* invalid gender
* empty name

## 用户/管理员登陆
* password failed
* user type unknown

## 用户端
### update_profile
* invalid gender
* empty new_name
* non-existent user

### add_friend
* already added
* add yourseld
* non-existent user

### view_friends

### find_friend
* friend not found

### manage_friend
* group not found
* not your friend

### delete_friend
* delete yourself
* non-existent friend

### create_group
* non-existent user
* empty group_name 

### delete_group
* non-existent group

### publish_moment
* empty content
* max-size content

### update_moment
* non-existent moment
* empty content

### delete_moment
* non-existent moment

### view_my_moments

### view_friends_moments

### publish_comment
* non-existent moment
* not your friend
* empty content

## 管理员端
* non-existent id
* empty name
* empty phone number
* maxsize name
* maxsize email
* maxsize phone number
