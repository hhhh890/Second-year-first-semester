from socket import *
from email.base64mime import body_encode
import ssl


def beforeSend(clientSocket):
    # 发送 HELO 命令并且打印服务端回复
    heloCommand = 'HELO Alice\r\n'
    clientSocket.send(heloCommand.encode()) # 随时注意对信息编码和解码
    recv1 = clientSocket.recv(1024).decode()
    print(recv1)
    if '250' != recv1[:3]:
        print('250 reply not received from server.')
def tlspass(clientSocket,mailServer,username,password):
    # 关键：启用TLS加密（587端口需要通过STARTTLS升级加密）
    clientSocket.send('STARTTLS\r\n'.encode())
    recv_tls = clientSocket.recv(1024).decode()
    print(recv_tls)
    if '220' != recv_tls[:3]:  # 服务器返回220表示可以开始TLS
        print('TLS not supported')
        tryquit(clientSocket)
    # 用SSLContext替代过时的ssl.wrap_socket()
    context = ssl.create_default_context()
    clientSocket = context.wrap_socket(clientSocket, server_hostname=mailServer)

    # 发送"AUTH PLAIN"命令，验证身份.服务器将返回状态码334（服务器等待用户输入验证信息）
    user_pass_encode64 = body_encode(f"\0{username}\0{password}".encode('ascii'), eol='')
    clientSocket.sendall(f'AUTH PLAIN {user_pass_encode64}\r\n'.encode())
    recv2 = clientSocket.recv(1024).decode()
    print(recv2)
    return clientSocket

def buildOneMsg(frmAddress,toAddr,subject='Email',body='empty'):
    msg = f"From: <{frmAddress}>\r\n"  # From头部，必须包含
    msg += f"To: <{toAddr}>\r\n"    # To头部，必须包含
    msg += f"Subject: {subject}\r\n"   # 主题（可选，但建议添加）
    msg += "\r\n"                      # 头部与正文的分隔空行（必须）
    msg += body # 正文内容
    return msg
def buildmsg(frmAddress, toAddrs, subject='Email', body='empty'):
    """构建邮件内容，支持多个收件人（To头部用逗号分隔）"""
    msg = f"From: <{frmAddress}>\r\n"
    # 多个收件人在To头部用逗号分隔
    msg += f"To: {', '.join([f'<{addr}>' for addr in toAddrs])}\r\n"
    msg += f"Subject: {subject}\r\n"
    msg += "\r\n"  # 头部与正文的分隔空行（必须）
    msg += body
    return msg
def sendMsg(clientSocket,fromAddress,toAddrs,msg,all=False):

    endMsg = "\r\n.\r\n"

    frmCommand = f'MAIL FROM: <{fromAddress}>\r\n'
    clientSocket.send(frmCommand.encode())
    recv3 = clientSocket.recv(1024).decode()
    print(recv3)
    if '250'!=recv3[:3]:
        print("发送方邮箱不合理")
        tryquit(clientSocket)
    if all:
           # 循环发送收件人信息（每个收件人对应一个RCPT TO）
        success_recipients = []
        fail_recipients = []
        for toAddr in toAddrs:
            toCommand = f'RCPT TO: <{toAddr}>\r\n'
            clientSocket.send(toCommand.encode())
            recv4 = clientSocket.recv(1024).decode()
            print(f"处理收件人 {toAddr}：{recv4}")
            if '250' == recv4[:3]:
                success_recipients.append(toAddr)
            else:
                fail_recipients.append(toAddr)  # 记录失败的收件人，不中断后续发送

        # 如果所有收件人都失败，无需继续发送内容
        if not success_recipients:
            print("所有收件人都无法接收，终止发送")
            return 
        if fail_recipients:
            print("发送失败的有：")
            print(fail_recipients)
    else:
        toCommand =  f'RCPT TO: <{toAddrs}>\r\n'
        clientSocket.send(toCommand.encode())
        recv4 = clientSocket.recv(1024).decode()
        print(recv4)
        if '250'!=recv4[:3]:
            print("收件人邮箱不存在")
            tryquit(clientSocket)

    sendRdCommand =  'DATA\r\n'
    clientSocket.send(sendRdCommand.encode())
    recv5 = clientSocket.recv(1024).decode()
    print(recv5)
    if '354'!=recv5[:3]:
        print("服务器没有准备好")
        tryquit(clientSocket)

    clientSocket.send(msg.encode())
    clientSocket.send(endMsg.encode())
    recv6 = clientSocket.recv(1024).decode()
    print(recv6)
    if '250'!=recv6[:3]:
        print("send fail")
        tryquit(clientSocket)

    quitCommand = 'QUIT\r\n'
    clientSocket.send(quitCommand.encode())
    recv7 = clientSocket.recv(1024).decode()
    # 清理响应：只保留可见字符（去除控制字符）
    cleaned_recv7 = ''.join([c for c in recv7 if c.isprintable()])
    print(cleaned_recv7)  # 打印清理后的内容，避免乱码
    # 用startswith判断状态码，更可靠
    if not cleaned_recv7.startswith('221'):
        print("quit fail")

def toAllAddress():
    print("请输入要发送的目标邮箱地址,输入“q”结束或输入空行（仅按回车）结束")
    address = []
    while(True):
        addr = input("接受邮件：")
        if addr=="q":
            break 
        if not addr:
            break
        address.append(addr)
    return address
def inputBody():
    """获取多行输入，直到用户输入空行（仅按回车）结束"""
    print("请输入邮件正文（输入空行结束）：")
    body = []  # 用列表存储每行内容，避免频繁字符串拼接
    while True:
        line = input()  # 获取一行输入
        if not line:  # 如果输入为空行（用户仅按了回车）
            break  # 结束输入
        body.append(line)  # 将非空行添加到列表
    # 用换行符拼接所有行，形成完整正文（注意邮件协议中用\r\n换行）
    return '\r\n'.join(body)
def tryquit(clientSocket):
        # 尝试优雅断开连接
    try:
        clientSocket.send('QUIT\r\n'.encode())
        recv_quit = clientSocket.recv(1024).decode()
        print("尝试关闭连接：", recv_quit)
    except Exception as e:
        print("关闭连接时出错：", e)
    finally:
        clientSocket.close()  # 确保套接字被关闭
        print("出现错误，已经结束程序")
        exit(1)  # 处理完成后退出
def main():
        # 发送方地址和接收方地址，from 和 to
    fromAddress = input("输入发送方邮箱：")
    
    # 选择一个邮件服务
    mailServer = "smtp.qq.com"
    # 发送方，验证信息，由于邮箱输入信息会使用base64编码，因此需要进行编码
    username =fromAddress # 输入自己的用户名对应的编码
    password = input("输入开启SMTP服务时对应的授权码：") # 此处不是自己的密码，而是开启SMTP服务时对应的授权码
    # 创建客户端套接字并建立连接
    serverPort = 587 # SMTP使用587号端口
    clientSocket = socket(AF_INET, SOCK_STREAM)
    clientSocket.connect((mailServer, serverPort)) # connect只能接收一个参数
    # 从客户套接字中接收信息
    recv = clientSocket.recv(1024).decode()
    print(recv)
    clientSocket=tlspass(clientSocket,mailServer,username,password)
    
    if '220' != recv[:3]:
        print('220 reply not received from server.')
        tryquit(clientSocket)
    beforeSend(clientSocket)
    sendchoose=0
    while(True):
        choose = input("choose send a.all群发 or b.one单发,please input a or b,输入空格就关掉了:")
        if choose=='a':
            sendchoose = 1
            break
        elif choose=='b':
            sendchoose=0
            break
        elif not choose:
            print("end the program")
            tryquit(clientSocket)
            break
        else:
            print("please input again.")
    
    if sendchoose==1:
        toaddres = toAllAddress()
        subject = input("请输入主题:")
        body = inputBody()
        msg = buildmsg(fromAddress,toaddres,subject,body)
        sendMsg(clientSocket,fromAddress,toaddres,msg,1)
    else:
        toAddress = input("请输入一个邮件地址：")
        subject =input("请输入主题:")
        body = inputBody()
        msg = buildOneMsg(fromAddress,toAddress,subject,body)
        sendMsg(clientSocket,fromAddress,toAddress,msg)

if __name__ =='__main__':
    main()
