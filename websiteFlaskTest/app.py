from flask import Flask, url_for, redirect

app = Flask(__name__)

@app.route("/")
def hello_world():
    return "<p>Hello, World!</p>"

@app.route("/index.html")
def index_file():
    return redirect(url_for('static', filename='index.html'))

@app.route("/style.css")
def style_file():
    return redirect(url_for('static', filename='style.css'))

@app.route("/index.js.download")
def index_file_js_download():
    return redirect(url_for('static', filename='index.js.download'))

@app.route("/webfontloader.js")
def webfontloader_file():
    return redirect(url_for('static', filename='webfontloader.js'))

@app.route("/iconfont.woff2")
def iconfont_file():
    return redirect(url_for('static', filename='iconfont.woff2'))

@app.route("/splashpage.html")
def splashpage_file():
    return redirect(url_for('static', filename='splashpage.html'))

print("Finished")