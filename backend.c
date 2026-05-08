#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX 100

/* ================= MENU ================= */

struct Menu
{
    int id;
    char name[50];
    int price;
    int available;
};

struct Menu menu[15] =
{
    {1,"Chicken Biryani",280,1},
    {2,"Paneer Tikka",180,1},
    {3,"Pizza",250,1},
    {4,"Burger",120,1},
    {5,"French Fries",90,1},
    {6,"Veg Fried Rice",190,1},
    {7,"Noodles",170,1},
    {8,"Butter Chicken",260,1},
    {9,"Butter Naan",40,1},
    {10,"Shawarma",160,1},
    {11,"Brownie",120,1},
    {12,"Ice Cream",90,1},
    {13,"Cold Coffee",110,1},
    {14,"Mojito",130,1},
    {15,"Lime Soda",60,1}
};

/* ================= ORDER ================= */

struct Order
{
    int token;
    char customer[50];
    char item[50];
    int qty;
    int total;
    char status[20];
};

/* ================= QUEUE ================= */

struct Order queue[MAX];
int front = -1;
int rear = -1;

/* ================= PRIORITY QUEUE ================= */

struct Order pqueue[MAX];
int pfront = -1;
int prear = -1;

/* ================= LINKED LIST ================= */

struct Notification
{
    char msg[200];
    struct Notification *next;
};

struct Notification *head = NULL;

/* ================= BST ================= */

struct BST
{
    char item[50];
    int sold;
    struct BST *left;
    struct BST *right;
};

struct BST *root = NULL;

/* ================= TOKEN ================= */

int tokenCounter = 1;

/* ================= ENQUEUE ================= */

void enqueue(struct Order o)
{
    if(front == -1)
    {
        front = 0;
    }

    rear++;

    queue[rear] = o;
}

/* ================= PRIORITY ENQUEUE ================= */

void priorityEnqueue(struct Order o)
{
    if(pfront == -1)
    {
        pfront = 0;
    }

    prear++;

    pqueue[prear] = o;
}

/* ================= LINKED LIST ================= */

void addNotification(char text[])
{
    struct Notification *newnode;

    newnode =
    (struct Notification*)
    malloc(sizeof(struct Notification));

    strcpy(newnode->msg,text);

    newnode->next = NULL;

    if(head == NULL)
    {
        head = newnode;
    }

    else
    {
        struct Notification *temp = head;

        while(temp->next != NULL)
        {
            temp = temp->next;
        }

        temp->next = newnode;
    }
}

/* ================= BST ================= */

struct BST* createNode(char item[],int qty)
{
    struct BST *newnode;

    newnode =
    (struct BST*)
    malloc(sizeof(struct BST));

    strcpy(newnode->item,item);

    newnode->sold = qty;

    newnode->left = NULL;

    newnode->right = NULL;

    return newnode;
}

struct BST* insertBST(struct BST *root,
                      char item[],
                      int qty)
{
    if(root == NULL)
    {
        return createNode(item,qty);
    }

    int cmp = strcmp(item,root->item);

    if(cmp == 0)
    {
        root->sold += qty;
    }

    else if(cmp < 0)
    {
        root->left =
        insertBST(root->left,item,qty);
    }

    else
    {
        root->right =
        insertBST(root->right,item,qty);
    }

    return root;
}

/* ================= PLACE ORDER ================= */

void placeOrder(char customer[],
                char item[],
                int qty,
                int price)
{
    struct Order o;

    o.token = tokenCounter++;

    strcpy(o.customer,customer);

    strcpy(o.item,item);

    o.qty = qty;

    o.total = qty * price;

    strcpy(o.status,"Preparing");

    if(qty <= 3)
    {
        priorityEnqueue(o);
    }

    else
    {
        enqueue(o);
    }

    root =
    insertBST(root,item,qty);

    char notif[100];

    sprintf(notif,
            "Token T%d confirmed",
            o.token);

    addNotification(notif);

    printf("ORDER SUCCESS\n");

    printf("TOKEN:T%d\n",o.token);

    printf("TOTAL:%d\n",o.total);

    if(qty <= 3)
    {
        printf("PRIORITY:YES\n");
    }

    else
    {
        printf("PRIORITY:NO\n");
    }
}

/* ================= READY ================= */

void markReady(int token)
{
    int i;

    for(i=pfront;i<=prear;i++)
    {
        if(i >= 0 &&
           pqueue[i].token == token)
        {
            strcpy(pqueue[i].status,
                   "Ready");

            printf("READY UPDATED");

            return;
        }
    }

    for(i=front;i<=rear;i++)
    {
        if(i >= 0 &&
           queue[i].token == token)
        {
            strcpy(queue[i].status,
                   "Ready");

            printf("READY UPDATED");

            return;
        }
    }

    printf("TOKEN NOT FOUND");
}

/* ================= MAIN ================= */

int main(int argc,char *argv[])
{
    if(strcmp(argv[1],"place") == 0)
    {
        placeOrder(
            argv[2],
            argv[3],
            atoi(argv[4]),
            atoi(argv[5])
        );
    }

    else if(strcmp(argv[1],"ready") == 0)
    {
        markReady(
            atoi(argv[2])
        );
    }

    return 0;
}
