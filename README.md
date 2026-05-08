# Piezo IV – Hotel Takeaway Management System

## Architecture
```
Frontend (HTML/CSS/Bootstrap/Minimal JS)
        ↓  fetch() only
Flask (app.py) — pure bridge, no logic
        ↓  ctypes
C Backend (backend.c / backend.so)
  ├── Array         → Menu storage (18 items)
  ├── Struct        → MenuItem, Order, OrderItem
  ├── Queue         → Normal order FIFO queue
  ├── Priority Queue→ Min-heap for ≤3-item orders
  ├── Linked List   → Notification chain (newest first)
  └── BST           → Item-wise sales (in-order traversal)
```

---

## Folder Structure
```
piezo_iv/
├── backend.c          ← ALL business logic (C)
├── backend.so         ← Compiled shared library
├── app.py             ← Flask bridge (ctypes only)
├── templates/
│   └── index.html     ← Full luxury frontend
└── README.md
```

---

## Step-by-Step Setup

### 1. Compile C backend
```bash
cd piezo_iv
gcc -O2 -shared -fPIC -o backend.so backend.c
```

### 2. Install Flask
```bash
pip install flask
# OR on system Python:
pip install flask --break-system-packages
```

### 3. Run Flask
```bash
python3 app.py
```
The server starts at: **http://127.0.0.1:5000**

---

## Flask API Routes

| Method | Route                    | Description                         |
|--------|--------------------------|-------------------------------------|
| GET    | `/`                      | Serve index.html                    |
| GET    | `/api/menu`              | Get all menu items (array from C)   |
| POST   | `/api/place_order`       | Place order → Queue/PQ, BST, notif  |
| GET    | `/api/orders`            | All orders (manager view)           |
| GET    | `/api/customer_orders`   | Orders by phone number              |
| POST   | `/api/mark_ready`        | Mark order ready + notify           |
| POST   | `/api/mark_collected`    | Mark order collected                |
| POST   | `/api/toggle_availability`| Toggle menu item on/off            |
| GET    | `/api/notifications`     | Linked list of notifications        |
| GET    | `/api/sales`             | BST in-order sales analytics        |
| GET    | `/api/stats`             | Order count + revenue stats         |

---

## Data Structures Used (in C)

### Array — Menu
```c
MenuItem menu[MAX_MENU];   // 18 pre-loaded food items
```

### Queue — Normal orders (FIFO)
```c
typedef struct QueueNode { int token; struct QueueNode *next; } QueueNode;
typedef struct { QueueNode *front, *rear; int size; } Queue;
```

### Priority Queue — Orders ≤ 3 items (min-heap on token)
```c
typedef struct { int tokens[PQ_MAX]; int size; } PriorityQueue;
```

### Linked List — Notifications (newest at head)
```c
typedef struct NotifNode {
    int order_token; char message[256]; char time_str[32]; int type;
    struct NotifNode *next;
} NotifNode;
```

### BST — Sales analytics (keyed by item_id)
```c
typedef struct BSTNode {
    int item_id; char item_name[64]; int qty_sold; int revenue;
    struct BSTNode *left, *right;
} BSTNode;
```

---

## Features

### Customer Side
- ✅ View full menu with images and category filters
- ✅ Add/remove items with quantity control
- ✅ Floating cart bar with live total
- ✅ Order modal with checkout form
- ✅ Token generation (T001, T002, …)
- ✅ Priority flag for ≤ 3 item orders
- ✅ Notifications via linked list
- ✅ Order tracking (Preparing / Ready / Collected)
- ✅ Bill summary

### Manager Side
- ✅ Stats: total orders, preparing, ready, revenue
- ✅ Full order table with customer details
- ✅ Mark Ready button → sends notification to customer
- ✅ Mark Collected button
- ✅ Item-wise sales (BST in-order traversal with bar chart)
- ✅ Availability toggles (updates menu in real time)
- ✅ Priority order highlighting

---

## JavaScript Rule Compliance
JavaScript ONLY calls `fetch()` and updates the DOM. Zero business logic.
All logic (queue management, BST, priority detection, token generation, etc.) runs in `backend.c`.
