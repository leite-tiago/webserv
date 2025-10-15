#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os

print("Content-Type: text/html")
print()

print("<!DOCTYPE html>")
print("<html><head><title>Hello CGI</title></head>")
print("<body>")
print("<h1>Hello from Python CGI!</h1>")
print(f"<p>Request Method: {os.environ.get('REQUEST_METHOD', 'N/A')}</p>")
print(f"<p>Query String: {os.environ.get('QUERY_STRING', 'N/A')}</p>")
print("</body></html>")
