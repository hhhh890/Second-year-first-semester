-- create the database
DROP DATABASE IF EXISTS Mini_WeChat;
CREATE DATABASE Mini_WeChat;
USE Mini_WeChat;

-- 重新创建（或者直接跑一遍 source）
-- source db_scheme.sql; 
-- create the User table
CREATE TABLE User (
    user_id VARCHAR(50) PRIMARY KEY,
    password VARCHAR(20) NOT NULL,
    name VARCHAR(40) NOT NULL,
    gender VARCHAR(6),
    CONSTRAINT chk_user_gender CHECK (gender IN ('male', 'female')),
    birth_date DATE,
    age INT
);

-- create the Admin table
CREATE TABLE Admin (
    admin_id VARCHAR(50) PRIMARY KEY,
    password VARCHAR(20) NOT NULL,
    name VARCHAR(40) NOT NULL,
    phone_number VARCHAR(11) NOT NULL,
    email VARCHAR(20)
);

-- create the FriendGroup table
CREATE TABLE FriendGroup (
    group_id INT PRIMARY KEY AUTO_INCREMENT,
    group_name VARCHAR(40) NOT NULL,
    created_by VARCHAR(50),
    FOREIGN KEY (created_by) REFERENCES User(user_id)
    ON DELETE CASCADE
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
    ON DELETE SET NULL
);

-- create the Moment table
CREATE TABLE Moment (
    moment_id INT PRIMARY KEY AUTO_INCREMENT,
    moment_content VARCHAR(600) NOT NULL,
    user_id VARCHAR(50),
    FOREIGN KEY (user_id) REFERENCES User(user_id)
    ON DELETE CASCADE,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
    ON UPDATE CURRENT_TIMESTAMP
);  

-- create the Comment table
CREATE TABLE Comment (
    comment_id INT PRIMARY KEY AUTO_INCREMENT,
    comment_content VARCHAR(200) NOT NULL,
    user_id VARCHAR(50),
    moment_id INT,
    FOREIGN KEY (user_id) REFERENCES User(user_id)
    ON DELETE CASCADE,
    FOREIGN KEY (moment_id) REFERENCES Moment(moment_id)
    ON DELETE CASCADE,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- create the trigger to calculate user's age automatically
DELIMITER //
CREATE TRIGGER Get_age_update 
BEFORE UPDATE ON User
FOR EACH ROW
BEGIN
    SET NEW.age = TIMESTAMPDIFF(YEAR, NEW.birth_date, CURDATE());
END //
DELIMITER ;

DELIMITER //
CREATE TRIGGER Get_age_insert
BEFORE INSERT ON User
FOR EACH ROW
BEGIN
    SET NEW.age = TIMESTAMPDIFF(YEAR, NEW.birth_date, CURDATE());
END //
DELIMITER ;

-- Enable the event scheduler and create a daily refresh event
SET GLOBAL event_scheduler = ON;

DROP EVENT IF EXISTS Daily_Age_Refresh;
CREATE EVENT Daily_Age_Refresh
ON SCHEDULE EVERY 1 DAY
STARTS (TIMESTAMP(CURRENT_DATE) + INTERVAL 1 DAY)
DO
    UPDATE User SET age = TIMESTAMPDIFF(YEAR, birth_date, CURDATE());

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

GRANT SELECT, UPDATE ON Mini_WeChat.Admin TO Role_admin;
GRANT SELECT ON Mini_WeChat.Admin_Moment_View TO Role_admin;
GRANT DELETE ON Mini_WeChat.Moment TO Role_admin;
GRANT DELETE ON Mini_WeChat.User TO Role_admin;

-- create the default Super Admin
INSERT IGNORE INTO Admin (admin_id, password, name, phone_number, email) 
VALUES ('admin001', 'admin123', 'Zhao Xinrui', '00000000000', 'admin@system.com');

INSERT IGNORE INTO Admin (admin_id, password, name, phone_number, email) 
VALUES ('admin002', 'admin123', 'Lin Jiahui', '00000000000', 'admin@system.com');

INSERT INTO User(user_id, password, name, gender, birth_date)
VALUES
('u001', '123456', 'Alice', 'female', '2002-03-15'),
('u002', '123456', 'Bob', 'male', '2001-08-20'),
('u003', '123456', 'Charlie', 'male', '2003-11-05');

INSERT INTO FriendGroup(group_name, created_by)
VALUES ('Classmates', 'u001');
SET @u001_classmates = LAST_INSERT_ID();

INSERT INTO FriendGroup(group_name, created_by)
VALUES ('Close Friends', 'u002');
SET @u002_close_friends = LAST_INSERT_ID();

INSERT INTO Friendship(user_id, friend_id, group_id)
VALUES
('u001', 'u002', @u001_classmates),
('u002', 'u001', @u002_close_friends),
('u001', 'u003', NULL),
('u003', 'u001', NULL);

INSERT INTO Moment(moment_content, user_id)
VALUES ('Alice: preparing the database lab demo.', 'u001');
SET @m_alice = LAST_INSERT_ID();

INSERT INTO Moment(moment_content, user_id)
VALUES ('Bob: this moment is visible to friends and admins.', 'u002');
SET @m_bob = LAST_INSERT_ID();

INSERT INTO Moment(moment_content, user_id)
VALUES ('Charlie: testing comments and cascade delete.', 'u003');
SET @m_charlie = LAST_INSERT_ID();

INSERT INTO Comment(comment_content, user_id, moment_id)
VALUES
('Bob comments on Alice moment.', 'u002', @m_alice),
('Alice comments on Bob moment.', 'u001', @m_bob),
('Alice comments on Charlie moment.', 'u001', @m_charlie);
GRANT Role_user, Role_admin TO 'root'@'localhost';
SET DEFAULT ROLE Role_user, Role_admin TO 'root'@'localhost';