from flask import Flask
from flask import render_template
from flask import request
from flask import jsonify

import subprocess

app = Flask(__name__)

@app.route("/")
def home():

    return render_template("index.html")

# ================= PLACE ORDER =================

@app.route("/place_order",
methods=["POST"])

def place_order():

    data = request.json

    customer = data["customer"]

    item = data["item"]

    qty = str(data["qty"])

    price = str(data["price"])

    result = subprocess.run(

        [
            "backend.exe",

            "place",

            customer,

            item,

            qty,

            price
        ],

        capture_output=True,

        text=True
    )

    return jsonify(
    {
        "message":
        result.stdout
    })

# ================= READY =================

@app.route("/mark_ready",
methods=["POST"])

def mark_ready():

    data = request.json

    token = str(data["token"])

    result = subprocess.run(

        [
            "backend.exe",

            "ready",

            token
        ],

        capture_output=True,

        text=True
    )

    return jsonify(
    {
        "message":
        result.stdout
    })

if __name__ == "__main__":

    app.run(debug=True)
