from flask import Flask, request, jsonify
import requests

app = Flask(__name__)

@app.route('/ping')
def ping():
    return jsonify({"status": "ok"})

@app.route('/get_html', methods=["POST"])
def get_html():
    data = request.get_json()
    if (data[url] == "z-test"):
        return jsonify({"status": "ok", "objects": {
            "1": {"type": "title", "attr": ["centered", "white"]},
            "2": {"type": "text", "attr": ["left", "red"]},
            "3": {"type": "background", "attr": ["blue"]}
        }})
    


app.run(host="0.0.0.0", debug=True, port=5000)