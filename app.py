<<<<<<< HEAD
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
=======
"""
Piezo IV – Flask Bridge
Connects HTML/JS frontend to C backend via ctypes.
NO business logic here — pure pass-through + JSON serialisation.
"""

import ctypes, json, os, datetime
from flask import Flask, jsonify, request, render_template, abort

# ── Load shared library ──────────────────────────────────────────────────────
LIB_PATH = os.path.join(os.path.dirname(__file__), "backend.dll")
lib = ctypes.CDLL(LIB_PATH)

# Configure argument / return types for every exported function
BUF = 65536          # general output buffer size
NOTIF_BUF = 131072   # notifications buffer (can grow)

def _fn(name, argtypes, restype=None):
    f = getattr(lib, name)
    f.argtypes = argtypes
    f.restype  = restype if restype else None
    return f

_get_menu          = _fn("get_menu",          [ctypes.c_char_p, ctypes.c_int])
_place_order       = _fn("place_order",        [ctypes.c_char_p, ctypes.c_char_p,
                                                ctypes.c_char_p, ctypes.c_char_p,
                                                ctypes.c_char_p, ctypes.c_int])
_get_orders        = _fn("get_orders",         [ctypes.c_char_p, ctypes.c_int])
_get_cust_orders   = _fn("get_customer_orders",[ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int])
_mark_ready        = _fn("mark_ready",         [ctypes.c_int,    ctypes.c_char_p,
                                                ctypes.c_char_p, ctypes.c_int])
_mark_collected    = _fn("mark_collected",     [ctypes.c_int,    ctypes.c_char_p, ctypes.c_int])
_toggle_avail      = _fn("toggle_availability",[ctypes.c_int,    ctypes.c_char_p, ctypes.c_int])
_get_notifs        = _fn("get_notifications",  [ctypes.c_int,    ctypes.c_char_p, ctypes.c_int])
_get_sales         = _fn("get_sales",          [ctypes.c_char_p, ctypes.c_int])
_get_stats         = _fn("get_stats",          [ctypes.c_char_p, ctypes.c_int])

def _call(fn, *args):
    """Call a C function that writes JSON to a char buffer. Returns parsed dict/list."""
    buf = ctypes.create_string_buffer(BUF)
    fn(*args, buf, BUF)
    raw = buf.value.decode("utf-8", errors="replace")
    return json.loads(raw)

def _call_big(fn, *args):
    """Same but with a larger buffer for notifications."""
    buf = ctypes.create_string_buffer(NOTIF_BUF)
    fn(*args, buf, NOTIF_BUF)
    raw = buf.value.decode("utf-8", errors="replace")
    return json.loads(raw)

def _now():
    return datetime.datetime.now().strftime("%H:%M")

# ── Flask app ────────────────────────────────────────────────────────────────
app = Flask(__name__)

@app.route("/")
def index():
    return render_template("index.html")

# ── Menu ────────────────────────────────────────────────────────────────────
@app.route("/api/menu")
def api_menu():
    data = _call(_get_menu)
    return jsonify(data)

# ── Place order ──────────────────────────────────────────────────────────────
@app.route("/api/place_order", methods=["POST"])
def api_place_order():
    body = request.get_json(force=True)
    items_json  = json.dumps(body.get("items", []))
    cust_name   = body.get("customer_name", "")
    phone       = body.get("phone", "")
    time_str    = _now()

    result = _call(_place_order,
                   items_json.encode(),
                   cust_name.encode(),
                   phone.encode(),
                   time_str.encode())
    if "error" in result:
        return jsonify(result), 400
    return jsonify(result)

# ── Get all orders (manager) ─────────────────────────────────────────────────
@app.route("/api/orders")
def api_orders():
    data = _call_big(_get_orders)
    return jsonify(data)

# ── Get customer orders by phone ─────────────────────────────────────────────
@app.route("/api/customer_orders")
def api_customer_orders():
    phone = request.args.get("phone", "")
    data  = _call_big(_get_cust_orders, phone.encode())
    return jsonify(data)

# ── Mark ready ───────────────────────────────────────────────────────────────
@app.route("/api/mark_ready", methods=["POST"])
def api_mark_ready():
    body  = request.get_json(force=True)
    token = int(body.get("token", 0))
    result = _call(_mark_ready, token, _now().encode())
    if "error" in result:
        return jsonify(result), 400
    return jsonify(result)

# ── Mark collected ───────────────────────────────────────────────────────────
@app.route("/api/mark_collected", methods=["POST"])
def api_mark_collected():
    body  = request.get_json(force=True)
    token = int(body.get("token", 0))
    result = _call(_mark_collected, token)
    if "error" in result:
        return jsonify(result), 400
    return jsonify(result)

# ── Toggle availability ───────────────────────────────────────────────────────
@app.route("/api/toggle_availability", methods=["POST"])
def api_toggle_availability():
    body    = request.get_json(force=True)
    item_id = int(body.get("item_id", 0))
    result  = _call(_toggle_avail, item_id)
    if "error" in result:
        return jsonify(result), 400
    return jsonify(result)

# ── Notifications ────────────────────────────────────────────────────────────
@app.route("/api/notifications")
def api_notifications():
    data = _call_big(_get_notifs, 50)
    return jsonify(data)

# ── Sales analytics (BST in-order) ───────────────────────────────────────────
@app.route("/api/sales")
def api_sales():
    data = _call(_get_sales)
    return jsonify(data)

# ── Stats ─────────────────────────────────────────────────────────────────────
@app.route("/api/stats")
def api_stats():
    data = _call(_get_stats)
    return jsonify(data)

if __name__ == "__main__":
    app.run(debug=True, port=5000)
>>>>>>> 6f7db5c32b7f9ca48b8178178f6813b5232c970f
