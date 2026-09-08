# lab05实验运行说明
## 配置安装
```bash
cd project
pip install -r project/requirements.txt
```
## 数据库准备
project\db_config.py文件的DB_PASSWORD改成运行的电脑的mysql密码
然后进入mysql，运行db_scheme.sql
```bash
cd project
mysql -u root -p
mysql> source db_scheme.sql; 
```
## 命令行运行
```bash
cd project
python main.py
```
## 网页端运行
```bash
cd project
streamlit run web_main.py
```
要键入email时按回车enter可以跳过，然后打开网页进行测试
## 测试用例
详细请看test.md
已经准备两个管理员和三个用户，并且初始化相应朋友圈、评论、好友分组
普通用户
u001 / 123456
u002 / 123456
u003 / 123456
管理员
admin001 / admin123
admin002 / admin123