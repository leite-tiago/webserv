#!/usr/bin/env python3

import os
import sys

print("Content-Type: text/html\r")
print("\r")
print("<html>")
print("<head><title>CGI POST Test</title></head>")
print("<body style='font-family: monospace; background: #1e1e1e; color: #00ff00; padding: 50px;'>")
print("<h1>POST Data Received</h1>")

if os.environ.get('REQUEST_METHOD') == 'POST':
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    if content_length > 0:
        post_data = sys.stdin.read(content_length)
        print(f"<p><strong>Data:</strong> {post_data}</p>")
    else:
        print("<p>No POST data received</p>")
else:
    print("<p>This script expects POST method</p>")

print("<p><a href='/' style='color: #00aaff;'>← Back to Home</a></p>")
print("</body>")
print("</html>")
