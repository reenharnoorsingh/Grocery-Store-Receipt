#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ItemStruct{
    int itemCode;
    int numItems;
    float itemPrice;
    float totalSales;
    char itemName[17];
    struct ItemStruct *nextItem;
} ITEM;

typedef struct CategoryStruct{
    int totItemSold;
    float totSales;
    char CategoryName[30];
    ITEM *items;
} Category;

void addItems(ITEM **head, ITEM *item){ //	Function that adds an item to linked list
    if (*head == NULL)
    {
        *head = item;
    }
    else if ((*head)->itemCode > item->itemCode)
    {
        item->nextItem = *head;
        *head = item;
    }
    else
    {
        ITEM *temp = *head;
        while (temp->nextItem && temp->nextItem->itemCode < item->itemCode)
        {
            temp = temp->nextItem;
        }
        item->nextItem = temp->nextItem;
        temp->nextItem = item;
    }
}
ITEM *ItemCreate(int code, char *name, float price){ //	Function creates a new item

    ITEM *temp = (ITEM *)malloc(sizeof(ITEM));
    temp->itemCode = code;
    strcpy(temp->itemName, name);
    temp->itemPrice = price;
    temp->numItems = 0;
    temp->totalSales = 0.0;

    temp->nextItem = NULL;

    return temp;
}

ITEM *ItemSearch(ITEM *head, int code){ //	Function that searches for an item in linked list
    while (head)
    {
        if (head->itemCode == code)
        {
            return head;
        }
        head = head->nextItem;
    }
    return head;
}

void PrintReceipt(FILE *file, ITEM *item_list){ //	Function that prints items list to descriptor specified
    while (item_list)
    {
        item_list->totalSales = ((item_list->itemPrice) * (item_list->numItems));
        fprintf(file, "%-10d%-20s%-10d%0.2f\n", item_list->itemCode, item_list->itemName, item_list->numItems, item_list->totalSales);
        item_list = item_list->nextItem;
    }
}

void deallocateMemory(ITEM *head){ //	Function that deallocates allocated memory for each item linked list
    ITEM *next;
    while (head)
    {
        next = head->nextItem;
        free(head);
        head = next;
    }
}
int main()
{
    //	Declaring variables and structures
    Category inventory[8];
    int code;
    char itemName[17];
    float itemPrice;
    int customerNo = 1;
    int ItemQuantity;
    int i;
    int totItemsSold = 0;
    float totSales = 0.0;

    ITEM *temp;

    //	Initializing variables and structures
    for (i = 0; i < 8; i++)
    {
        inventory[i].items = NULL;
        inventory[i].totItemSold = 0;
        inventory[i].totSales = 0.0;
    }
    //	Use Case 01
    FILE *catData = fopen("CategoryName.dat", "r"); //reads "CategoryName.dat"
    while (!feof(catData)){                         //tests the end of file for catName
        fscanf(catData, "%d\t", &code);
        if (feof(catData))
            continue;
        fgets(inventory[(code / 100) - 1].CategoryName, 30, catData);
        inventory[(code / 100) - 1].CategoryName[strlen(inventory[(code / 100) - 1].CategoryName) - 2] = '\0';
    }
    //	Use Case 02
    FILE *productData = fopen("CodeNamePrice.dat", "r"); // reads "CodeNamePrice.dat"
    while (!feof(productData)){
        fscanf(productData, "%d\t", &code);
        if (feof(productData))
            continue;
        fscanf(productData, "%[^\t]", itemName);
        fscanf(productData, "%f", &itemPrice);
        ITEM *item = ItemCreate(code, itemName, itemPrice); //	Adding item to linked list
        addItems(&(inventory[(code / 100) - 1].items), item);
    }
    //	Use Case 03
    FILE *customerData = fopen("DailyTransactions.dat", "r"); //reads "DailyTransactions.dat"
    ITEM *customerList = NULL;
    ITEM *customerItem = NULL;
    while (!feof(customerData)){ //tests the end of file for customerData
        fscanf(customerData, "%d\t", &code);
        if (code != 0)
        {
            fscanf(customerData, "%d", &ItemQuantity);

            temp = ItemSearch(inventory[(code / 100) - 1].items, code); //	Searching an item in list

            temp->numItems = temp->numItems + ItemQuantity;
            customerItem = ItemCreate(code, temp->itemName, temp->itemPrice);
            customerItem->numItems = ItemQuantity;
            customerItem->totalSales = customerItem->itemPrice * customerItem->numItems;

            //	Updates the category details
            inventory[(code / 100) - 1].totItemSold = inventory[(code / 100) - 1].totItemSold + ItemQuantity;
            inventory[(code / 100) - 1].totSales = inventory[(code / 100) - 1].totSales + (customerItem->itemPrice * customerItem->numItems);

            addItems(&customerList, customerItem); //	Adding item into linked list of items
        }
        else
        {
            //	Use Case 04
            printf("\nCustomer Reciept # %d\n", customerNo++);
            printf("%-10s%-20s%-10s%-10s%s\n", "Code", "Item Name", "Price", "Num Item", "Total Sales");
            customerItem = customerList;
            while (customerItem)
            {
                printf("%-10d%-20s%-10.2f%-10d%0.2f\n", customerItem->itemCode, customerItem->itemName, customerItem->itemPrice, customerItem->numItems, customerItem->totalSales);
                customerItem = customerItem->nextItem;
            }
            deallocateMemory(customerList);
            customerList = NULL;
        }
    }

    //	Use Case 05
    char catCode[] = "000";
    char catFile[] = "Inventory000.dat"; //category file
    for (i = 0; i < 8; i++)
    {
        printf("\n");
        printf("Category Name: %s\n", inventory[i].CategoryName);
        catCode[0] = i + '1';
        printf("Category Code: %s\n", catCode);
        printf("%-10s%-20s%-10s%s\n", "Code", "Item Name", "Num Item", "Total Sales");

        catFile[9] = i + '1';
        FILE *customerFile = fopen(catFile, "w"); //customer file

        //	Prints to stdout
        PrintReceipt(stdout, inventory[i].items);

        printf("%-20s%d\n", "Total Items Sold: ", inventory[i].totItemSold);
        printf("%-20s%0.2f\n", "Total Sales: ", inventory[i].totSales);

        PrintReceipt(customerFile, inventory[i].items); //	Printing to Inventory file

        fclose(customerFile);
    }

    fclose(catData);
    fclose(productData);
    fclose(customerData);

    //	Use Case 06
    printf("\n");
    printf("Daily Summary Report\n");
    printf("%-10s%-20s%-20s%s\n", "Code", "Category Name", "#Items Sold", "Total Sales");
    for (i = 0; i < 8; i++)
    {

        totItemsSold = totItemsSold + inventory[i].totItemSold;
        totSales = totSales + inventory[i].totSales;

        catCode[0] = i + '1';
        printf("%-10s", catCode);
        printf("%-20s", inventory[i].CategoryName);
        printf("%-20d", inventory[i].totItemSold);
        printf("%0.2f", inventory[i].totSales);
        printf("\n");
    }
    printf("%-20s%10d\n", "Total Customers", customerNo - 1);
    printf("%-20s%10d\n", "Total Items Sold", totItemsSold);
    printf("%-20s%10.2f\n", "Total Sales", totSales);

    //	Freeing all allocated memory
    for (i = 0; i < 8; i++)
    {
        deallocateMemory(inventory[i].items);
        inventory[i].items = NULL;
    }
    return 0;
}
