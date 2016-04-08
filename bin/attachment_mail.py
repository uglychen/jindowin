#/usr/bin/env python
# coding=utf-8
import os
import sys
from mail import Mail

"""
python attachment_mail.py 412552696@qq.com 1.txt emtf
"""


def main(args):
    """["attachment_mail.py", "412552696@qq.com", "1.txt", "this is a test args"]"""
    param_count = len(args)
    filename = ''
    content = ''
    if param_count == 1:
        print 'do not have email params'
        return
    if param_count == 2:
        print 'do not have attachments params'
        return
    if param_count >= 3:
        """file path"""
        filename = args[2]
        if not os.path.exists(filename):
            print 'dattachments not exist'
            return
    if param_count == 4:
        content = args[3]
    to_mail_list = args[1].split(',')
    mail = Mail()
    mail.set_to_list(to_mail_list)
    mail.set_subject("详情")
    mail.send_attachment(content, filename)


if __name__ == '__main__':
    main(sys.argv)
