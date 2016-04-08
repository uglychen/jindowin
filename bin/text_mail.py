#/usr/bin/python
# coding=utf-8
import sys
from mail import Mail

"""
call like this:
 python text_mail.py 412552696@qq.com 456789100
params:  email list like 415@qq.com,456@qq.com,345@qq.com
         hotkeys like money,oil,bank will be show in email main body
"""
def main(args):
    print args
    global content
    param_count = len(args)
    if param_count < 2:
        print 'do not have email params'
        return
    if param_count < 3:
        print 'do not have content words'
        return
    to_mail_list = args[2].split(',')
    mail = Mail()
    mail.set_to_list(to_mail_list)
    mail.set_subject("热词")
    mail.send_text_mail(content)


if __name__ == '__main__':
    main(sys.argv)
