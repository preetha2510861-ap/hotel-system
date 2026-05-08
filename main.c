#include <stdio.h>
#include <string.h>

#define MAX 10
#define USERS 50

/* ---------------- MENU DATA ---------------- */
char item_name[MAX][30] = {
    "Paneer Tikka", "Veg Biryani", "Chicken Biryani",
    "Dal Tadka", "Gulab Jamun", "Ice Cream"
};

float price[MAX] = {120, 120, 160, 90, 50, 40};
int available[MAX] = {1,1,1,1,1,1};

int total_items = 6;
int token = 0;

/* ---------------- CUSTOMER DATA ---------------- */
struct Customer {
    char username[50];
    char password[50];
};

struct Customer users[USERS];
int userCount = 0;

/* ---------------- FUNCTIONS ---------------- */

/* DISPLAY MENU */
void show_menu() {
 int i;
    printf("\n--- MENU ---\n");
    printf("ID Name Price Status\n");

    for( i=0; i<total_items; i++) {
        printf("%d %-15s %.2f %s\n",
               i+1,
               item_name[i],
               price[i],
               available[i] ? "Available" : "Not Available");
    }
}

/* PLACE ORDER */
void place_order() {
    int id, qty;
    float total = 0;
    char more;

    show_menu();

    do {
        printf("\nEnter Item ID: ");
        scanf("%d", &id);

        if(id < 1 || id > total_items) {
            printf("Invalid ID\n");
            continue;
        }

        if(!available[id-1]) {
            printf("Item not available\n");
            continue;
        }

        printf("Enter quantity: ");
        scanf("%d", &qty);

        total += price[id-1] * qty;

        printf("Add more? (y/n): ");
        scanf(" %c", &more);

    } while(more == 'y' || more == 'Y');

    token++;
    printf("\nOrder Placed!\nToken: T%03d\nTotal Bill: %.2f\n", token, total);

    printf("Payment Successful!\n");
    printf("Notification sent to Customer and Manager.\n");
}

/* REGISTER */
void registerUser() {
    printf("\n--- REGISTER ---\n");

    printf("Enter Username: ");
    scanf("%s", users[userCount].username);

    printf("Enter Password: ");
    scanf("%s", users[userCount].password);

    userCount++;

    printf("Registration Successful!\n");
}

/* LOGIN */
int loginUser() {
 int i;
    char uname[50], pass[50];

    printf("\n--- LOGIN ---\n");

    printf("Enter Username: ");
    scanf("%s", uname);

    printf("Enter Password: ");
    scanf("%s", pass);

    for ( i = 0; i < userCount; i++) {
        if (strcmp(uname, users[i].username) == 0 &&
            strcmp(pass, users[i].password) == 0) {
            printf("Login Successful! Welcome %s\n", uname);
            return 1;
        }
    }

    printf("Invalid Credentials!\n");
    return 0;
}

/* CUSTOMER SESSION */
void customerSession() {
    int choice;
    int orderDone = 0; // ?? prevents multiple orders

    while(1) {
        printf("\n--- CUSTOMER PANEL ---\n");

        if(orderDone == 0) {
            printf("1. Place Order\n");
        }

        printf("2. View Menu\n");
        printf("0. Logout\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                if(orderDone == 0) {
                    place_order();
                    orderDone = 1;

                    printf("\nOrder completed. You cannot place another order.\n");
                    printf("Logging out...\n");
                    return; // ?? exit after order
                } else {
                    printf("Order already placed!\n");
                }
                break;

            case 2:
                show_menu();
                break;

            case 0:
                printf("Logging out...\n");
                return;

            default:
                printf("Invalid choice\n");
        }
    }
}

/* CUSTOMER ENTRY */
void customerMenu() {
    int choice;

    printf("\n--- CUSTOMER ---\n");
    printf("1. Continue as Guest\n");
    printf("2. Login\n");
    printf("3. Register\n");
    printf("Choice: ");
    scanf("%d", &choice);

    switch(choice) {
        case 1:
            printf("Proceeding as Guest...\n");
            customerSession();
            break;

        case 2:
            if(loginUser()) {
                customerSession();
            }
            break;

        case 3:
            registerUser();
            printf("Logged in successfully!\n");
            customerSession();
            break;

        default:
            printf("Invalid choice\n");
    }
}

/* MANAGER - UPDATE AVAILABILITY */
void update_item() {
    int id, choice;

    show_menu();

    printf("\nEnter item ID to change availability: ");
    scanf("%d", &id);

    if(id < 1 || id > total_items) {
        printf("Invalid ID\n");
        return;
    }

    printf("1. Available\n0. Not Available\nEnter choice: ");
    scanf("%d", &choice);

    available[id-1] = choice;
    printf("Updated successfully!\n");
}

/* MANAGER LOGIN */
void managerLogin() {
    int pass;

    printf("Enter Manager Password: ");
    scanf("%d", &pass);

    if(pass == 1234) {
        printf("Access Granted!\n");

        int choice;
        while(1) {
            printf("\n--- MANAGER PANEL ---\n");
            printf("1. Update Item Availability\n");
            printf("2. View Menu\n");
            printf("0. Logout\n");
            printf("Choice: ");
            scanf("%d", &choice);

            switch(choice) {
                case 1:
                    update_item();
                    break;
                case 2:
                    show_menu();
                    break;
                case 0:
                    printf("Logging out...\n");
                    return;
                default:
                    printf("Invalid choice\n");
            }
        }

    } else {
        printf("Wrong Password!\n");
    }
}

/* ---------------- MAIN ---------------- */
int main() {
    int choice;

    while(1) {
        printf("\n--- HOTEL SYSTEM ---\n");
        printf("1. Customer\n");
        printf("2. Manager\n");
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                customerMenu();
                break;

            case 2:
                managerLogin();
                break;

            case 0:
                return 0;

            default:
                printf("Invalid choice\n");
        }
    }
}