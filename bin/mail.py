#/usr/bin/env python
# coding=utf-8
import smtplib
import time
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.header import Header


class Mail:
    """
    :@sender
    :@username
    :@password
    :smtp_server
    """
    to_list = ["412552696@qq.com", "candroid@126.com"]
    sender = 'info@kunyan-inc.com'
    username = 'info@kunyan-inc.com'
    password = 'kunyan@2015@'
    smtp_server = 'smtp.mxhichina.com'
    subject = ''

    # , send='fanglei@kunyan-inc.com', to='fanglei@kunyan-inc.com', passwd='Danteng001'
    def __init__(self):
        pass

    def send_text_mail(self, content):
        str_time = time.strftime(' %Y年%m月%d日 %X', time.localtime())
        print str_time
        msg = MIMEText(content, 'plain', 'utf-8')
        msg['Subject'] = Header(self.subject + str_time, 'utf-8')
        msg['from'] = self.sender
        self.__send__(msg.as_string())

    def send_attachment(self, content, file_name):
        str_time = time.strftime(' %Y年%m月%d日 %X', time.localtime())
        msg_root = MIMEMultipart('mixed')
        msg_root['Subject'] = Header(self.subject + str_time, 'utf-8')
        msg_root['from'] = self.sender
        msg_text = MIMEText(content, 'plain', 'utf-8')
        msg_root.attach(msg_text)
        msg_attch = MIMEText(open('%s' % file_name, 'rb').read(), 'base64', 'utf-8')
        msg_attch["Content-Type"] = 'application/octet-stream'
        msg_attch["Content-Disposition"] = 'attachment; filename="%s"' % file_name
        msg_root.attach(msg_attch)
        self.__send__(msg_root.as_string())

    def __send__(self, msg):
        smtp = smtplib.SMTP()
        smtp.connect(self.smtp_server)
        smtp.login(self.username, self.password)
        print self.to_list
        print msg
        smtp.sendmail(self.username, self.to_list, msg)
        smtp.quit()

    def set_to_list(self, list_mail):
        self.to_list = list_mail

    def set_subject(self, sub):
        # 邮件主题
        self.subject = sub
