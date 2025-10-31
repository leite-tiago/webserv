#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import datetime

# CGI Header
print("Content-Type: text/html")
print()  # Empty line to end headers

# HTML Content
print("<!DOCTYPE html>")
print("<html>")
print("<head>")
print("    <title>Python CGI Test</title>")
print("    <style>")
print("        body { font-family: Arial, sans-serif; max-width: 800px; margin: 50px auto; padding: 20px; }")
print("        h1 { color: #2563eb; }")
print("        .info { background: #f0f9ff; padding: 15px; border-left: 4px solid #2563eb; margin: 10px 0; }")
print("        table { width: 100%; border-collapse: collapse; margin: 20px 0; }")
print("        th, td { padding: 10px; text-align: left; border-bottom: 1px solid #ddd; }")
print("        th { background: #2563eb; color: white; }")
print("        .success { color: #16a34a; font-weight: bold; }")
print("    </style>")
print("</head>")
print("<body>")
print("    <h1>🐍 Python CGI Script</h1>")
print("    <p class='success'>✅ CGI execution successful!</p>")
print("    <div class='info'>")
print("        <strong>Server Time:</strong> " + str(datetime.datetime.now()))
print("    </div>")

print("    <h2>Environment Variables</h2>")
print("    <table>")
print("        <tr><th>Variable</th><th>Value</th></tr>")

# Important CGI variables
cgi_vars = [
    'REQUEST_METHOD', 'QUERY_STRING', 'PATH_INFO', 'SCRIPT_NAME',
    'SCRIPT_FILENAME', 'SERVER_NAME', 'SERVER_PORT', 'SERVER_PROTOCOL',
    'CONTENT_TYPE', 'CONTENT_LENGTH', 'GATEWAY_INTERFACE'
]

for var in cgi_vars:
    value = os.environ.get(var, '(not set)')
    print(f"        <tr><td><strong>{var}</strong></td><td>{value}</td></tr>")

print("    </table>")

# Query string parsing
query_string = os.environ.get('QUERY_STRING', '')
if query_string:
    print("    <h2>Query Parameters</h2>")
    print("    <table>")
    print("        <tr><th>Parameter</th><th>Value</th></tr>")
    pairs = query_string.split('&')
    for pair in pairs:
        if '=' in pair:
            key, value = pair.split('=', 1)
            print(f"        <tr><td><strong>{key}</strong></td><td>{value}</td></tr>")
    print("    </table>")

# POST data (if any)
if os.environ.get('REQUEST_METHOD') == 'POST':
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    if content_length > 0:
        post_data = sys.stdin.read(content_length)
        print("    <h2>POST Data</h2>")
        print("    <div class='info'>")
        print(f"        <strong>Length:</strong> {content_length} bytes<br>")
        print(f"        <strong>Data:</strong> {post_data}")
        print("    </div>")

print("    <h2>All Environment Variables</h2>")
print("    <table>")
print("        <tr><th>Variable</th><th>Value</th></tr>")
for key in sorted(os.environ.keys()):
    if key.startswith('HTTP_') or key in cgi_vars:
        value = os.environ.get(key, '')
        print(f"        <tr><td><strong>{key}</strong></td><td>{value}</td></tr>")
print("    </table>")

print("    <p><a href='/'>← Back to Home</a></p>")
print("</body>")
print("</html>")
