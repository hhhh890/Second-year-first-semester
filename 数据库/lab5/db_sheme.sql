-- create the database
DROP DATABASE IF EXISTS Mini_WeChat;
CREATE DATABASE Mini_WeChat;
USE Mini_WeChat;

-- 重新创建（或者直接跑一遍 source）
-- source db_sheme.sql;
-- create the User table
CREATE TABLE User (
    user_id VARCHAR(50) PRIMARY KEY,
    password VARCHAR(20) NOT NULL,
    name VARCHAR(40) NOT NULL,
    gender VARCHAR(6),
    CONSTRAINT chk_user_gender CHECK (gender IN ('male', 'female')),
    birth_date DATE
);

-- create the Admin table
CREATE TABLE Admin (
    admin_id VARCHAR(50) PRIMARY KEY,
    password VARCHAR(20) NOT NULL,
    name VARCHAR(40) NOT NULL,
    phone_number VARCHAR(11) NOT NULL,
    email VARCHAR(50)
);

-- create the FriendGroup table
CREATE TABLE FriendGroup (
    group_id INT PRIMARY KEY AUTO_INCREMENT,
    group_name VARCHAR(40) NOT NULL,
    created_by VARCHAR(50) NOT NULL,
    FOREIGN KEY (created_by) REFERENCES User(user_id)
    ON DELETE CASCADE,
    UNIQUE (group_name, created_by)
);

-- create the Friendship table
CREATE TABLE Friendship (
    user_id VARCHAR(50),
    friend_id VARCHAR(50),
    group_id INT DEFAULT NULL,
    PRIMARY KEY (user_id, friend_id),
    FOREIGN KEY (user_id) REFERENCES User(user_id)
    ON DELETE CASCADE,
    FOREIGN KEY (friend_id) REFERENCES User(user_id)
    ON DELETE CASCADE,
    FOREIGN KEY (group_id) REFERENCES FriendGroup(group_id) 
    ON DELETE SET NULL,
    CHECK (user_id <> friend_id)
);

-- create the Moment table
CREATE TABLE Moment (
    moment_id INT PRIMARY KEY AUTO_INCREMENT,
    moment_content VARCHAR(600) NOT NULL,
    user_id VARCHAR(50) NOT NULL,
    FOREIGN KEY (user_id) REFERENCES User(user_id)
    ON DELETE CASCADE,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
    ON UPDATE CURRENT_TIMESTAMP
);  

-- create the Comment table
CREATE TABLE Comment (
    comment_id INT PRIMARY KEY AUTO_INCREMENT,
    comment_content VARCHAR(200) NOT NULL,
    user_id VARCHAR(50) NOT NULL,
    moment_id INT,
    FOREIGN KEY (user_id) REFERENCES User(user_id)
    ON DELETE CASCADE,
    FOREIGN KEY (moment_id) REFERENCES Moment(moment_id)
    ON DELETE CASCADE,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- create the Message table
CREATE TABLE Message (
    message_id INT PRIMARY KEY AUTO_INCREMENT,
    sender_id VARCHAR(50) NOT NULL,
    receiver_id VARCHAR(50),
    content VARCHAR(200) NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (sender_id) REFERENCES User(user_id)
    ON DELETE CASCADE,
    FOREIGN KEY (receiver_id) REFERENCES User(user_id)
    ON DELETE CASCADE
);
-- create view of all users' moments for administrator
DROP VIEW IF EXISTS Admin_Moment_View;
CREATE VIEW Admin_Moment_View AS
SELECT m.moment_id, m.moment_content, m.user_id, m.updated_at, c.comment_content 
FROM Moment m LEFT JOIN Comment c ON m.moment_id = c.moment_id;

-- create role of user and authorization
DROP ROLE IF EXISTS Role_user, Role_admin;
CREATE ROLE Role_user, Role_admin;

GRANT SELECT, UPDATE ON Mini_WeChat.User TO Role_user;
GRANT INSERT, DELETE, SELECT, UPDATE ON Mini_WeChat.Friendship TO Role_user;
GRANT INSERT, DELETE, SELECT, UPDATE ON Mini_WeChat.FriendGroup TO Role_user;
GRANT INSERT, DELETE, SELECT, UPDATE ON Mini_WeChat.Moment TO Role_user;
GRANT INSERT, DELETE, SELECT, UPDATE ON Mini_WeChat.Comment TO Role_user;
GRANT INSERT, DELETE, SELECT, UPDATE ON Mini_WeChat.Message TO Role_user;

GRANT SELECT, UPDATE ON Mini_WeChat.Admin TO Role_admin;
GRANT SELECT ON Mini_WeChat.Admin_Moment_View TO Role_admin;
GRANT DELETE ON Mini_WeChat.Moment TO Role_admin;
GRANT DELETE ON Mini_WeChat.User TO Role_admin;

-- create the default Super Admin
INSERT IGNORE INTO Admin (admin_id, password, name, phone_number, email) 
VALUES ('admin001', 'admin123', 'Zhao Xinrui', '00000000000', 'admin@system.com');

INSERT IGNORE INTO Admin (admin_id, password, name, phone_number, email) 
VALUES ('admin002', 'admin123', 'Lin Jiahui', '00000000000', 'admin@system.com');

