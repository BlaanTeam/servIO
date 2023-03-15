#!/usr/local/bin/python3
import cgi
import cgitb
import os
import sys

# Enable CGI error reporting
cgitb.enable()

# Get form data from CGI request
form = cgi.FieldStorage()


cookies = {}

httpCookie = os.environ.get("HTTP_Cookie", None)
if  httpCookie:
    for cookie in httpCookie.split(";"):
        (key, value) = cookie.split("=")
        cookies[key.strip(" ")] = value

responseHeaders = []

name = cookies.get("name", "Unknown")
color = cookies.get("color", "white")

httpMethod = os.environ.get("REQUEST_METHOD")
if httpMethod and httpMethod == "POST":
    # Get value of name parameter from form
    nameValue = form.getvalue('name', None)
    if nameValue:
        responseHeaders += ["Set-Cookie: name=%s; Path=/" % nameValue]
        name = nameValue
    colorValue = form.getvalue("color", None)
    if colorValue:
        responseHeaders += ["Set-Cookie: color=%s; Path=/" % colorValue]
        color = colorValue



options = ""

for i in ["white", "black", "red", "green", "yellow", "blue"]:
    options += "<option value=\"%s\" %s>%s</option>" % (i, "selected" if i == color else "", i.capitalize())


body = '''
<head>
    <title>CGI - Cookies </title>

    <style>
        * {
            background-color: %s;
        }
    </style>
</head>

<body>
    <h1>Welcome, %s</h1>
    <form action="/cgi/script.py" method="POST">
        <input type="text" name="name" value="%s">
        <select name="color">
            %s
        </select>
        <input type="submit" value="Send">
    </form>
</body>

</html>
''' % (color, name, name, options)


print("Content-Length: %d" % len(body), end="\r\n")
print("Content-Type: text/html", end="\r\n")
print("Status: 404", end="\r\n")

for header in responseHeaders:
    print(header, end="\r\n")
print(end="\r\n")

print(body)