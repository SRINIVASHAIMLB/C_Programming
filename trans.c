#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACCOUNTS 100
#define DAILY_LIMIT 5000

struct client_data
{
    unsigned int acct_num;
    char first_name[10];
    char last_name[15];
    double balance;
    char password[10];       // 🔐 Password
    double daily_withdrawn;  // 💳 Daily withdrawal tracker
};

// -------- FILE INITIALIZATION --------
void initialize_file()
{
    FILE *fp = fopen("credit.dat", "rb");

    if (fp == NULL)
    {
        fp = fopen("credit.dat", "wb");
        struct client_data blank = {0};

        for (int i = 0; i < MAX_ACCOUNTS; i++)
            fwrite(&blank, sizeof(blank), 1, fp);

        printf("File initialized successfully!\n");
    }

    fclose(fp);
}

// -------- HELPER FUNCTIONS --------
int read_account(FILE *fp, unsigned int acc, struct client_data *client)
{
    fseek(fp, (acc - 1) * sizeof(struct client_data), SEEK_SET);
    return fread(client, sizeof(struct client_data), 1, fp);
}

void write_account(FILE *fp, unsigned int acc, struct client_data *client)
{
    fseek(fp, (acc - 1) * sizeof(struct client_data), SEEK_SET);
    fwrite(client, sizeof(struct client_data), 1, fp);
}

int valid_account(unsigned int acc)
{
    return (acc >= 1 && acc <= MAX_ACCOUNTS);
}

// -------- PASSWORD CHECK --------
int check_password(struct client_data client)
{
    char input[10];

    printf("Enter password: ");
    scanf("%9s", input);

    if (strcmp(input, client.password) != 0)
    {
        printf("Wrong password!\n");
        return 0;
    }
    return 1;
}

// -------- MENU --------
unsigned int menu()
{
    unsigned int choice;

    printf("\n===== BANK SYSTEM MENU =====\n");
    printf("1. Generate accounts.txt\n");
    printf("2. Update account\n");
    printf("3. Add new account\n");
    printf("4. List all accounts\n");
    printf("5. Delete accounts\n");
    printf("6. Search account\n");
    printf("7. Count accounts\n");
    printf("8. Reset database\n");
    printf("9. Sort accounts by balance\n");
    printf("10. Exit\n");
    printf("Enter choice: ");

    scanf("%u", &choice);
    return choice;
}

// -------- RESET DATABASE --------
void reset_database()
{
    FILE *fp = fopen("credit.dat", "wb");
    struct client_data blank = {0};

    for (int i = 0; i < MAX_ACCOUNTS; i++)
        fwrite(&blank, sizeof(blank), 1, fp);

    fclose(fp);
    printf("Database reset successful!\n");
}

// -------- GENERATE TEXT FILE --------
void text_file(FILE *fp)
{
    FILE *out = fopen("accounts.txt", "w");
    struct client_data client;

    rewind(fp);

    fprintf(out, "%-6s%-15s%-15s%-10s\n",
            "AccNo", "FirstName", "LastName", "Balance");

    while (fread(&client, sizeof(client), 1, fp))
    {
        if (client.acct_num != 0)
        {
            fprintf(out, "%-6d%-15s%-15s%-10.2f\n",
                    client.acct_num,
                    client.first_name,
                    client.last_name,
                    client.balance);
        }
    }

    fclose(out);
    printf("accounts.txt generated successfully!\n");
}

// -------- ADD ACCOUNT --------
void new_account(FILE *fp)
{
    struct client_data client = {0};
    unsigned int acc;

    printf("Enter account number (1-100): ");
    scanf("%u", &acc);

    if (!valid_account(acc))
    {
        printf("Invalid account number!\n");
        return;
    }

    read_account(fp, acc, &client);

    if (client.acct_num != 0)
    {
        printf("Account already exists!\n");
        return;
    }

    printf("Enter FirstName LastName Balance: ");
    scanf("%9s %14s %lf", client.first_name, client.last_name, &client.balance);

    printf("Set password: ");
    scanf("%9s", client.password);

    client.acct_num = acc;
    client.daily_withdrawn = 0;

    write_account(fp, acc, &client);

    printf("Account created successfully!\n");
}

// -------- UPDATE ACCOUNT --------
void update_account(FILE *fp)
{
    struct client_data client;
    unsigned int acc;
    double amount;
    int choice;

    printf("Enter account number: ");
    scanf("%u", &acc);

    if (!valid_account(acc))
    {
        printf("Invalid account number!\n");
        return;
    }

    read_account(fp, acc, &client);

    if (client.acct_num == 0)
    {
        printf("Account not found!\n");
        return;
    }

    if (!check_password(client))
        return;

    printf("1. Deposit\n2. Withdraw\nEnter choice: ");
    scanf("%d", &choice);

    printf("Enter amount: ");
    scanf("%lf", &amount);

    if (choice == 2)
    {
        if (client.balance < amount)
        {
            printf("Insufficient balance!\n");
            return;
        }

        if (client.daily_withdrawn + amount > DAILY_LIMIT)
        {
            printf("Daily withdrawal limit exceeded!\n");
            return;
        }

        client.balance -= amount;
        client.daily_withdrawn += amount;
    }
    else
    {
        client.balance += amount;
    }

    write_account(fp, acc, &client);

    printf("Transaction successful!\n");
}

// -------- DELETE --------
void delete(FILE *fp)
{
    int n;
    unsigned int acc;
    struct client_data client, blank = {0};

    printf("How many accounts to delete: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++)
    {
        printf("Enter account number: ");
        scanf("%u", &acc);

        if (!valid_account(acc))
        {
            printf("Invalid account!\n");
            continue;
        }

        read_account(fp, acc, &client);

        if (client.acct_num == 0)
        {
            printf("Account No:%d not found!\n", acc);
        }
        else
        {
            write_account(fp, acc, &blank);
            printf("Deleted account %d\n", acc);
        }
    }
}

// -------- LIST --------
void list_accounts(FILE *fp)
{
    struct client_data client;

    rewind(fp);

    printf("\n===== ACCOUNT LIST =====\n");
    printf("%-6s%-15s%-15s%-10s\n",
           "AccNo", "FirstName", "LastName", "Balance");

    while (fread(&client, sizeof(client), 1, fp))
    {
        if (client.acct_num != 0)
        {
            printf("%-6d%-15s%-15s%-10.2f\n",
                   client.acct_num,
                   client.first_name,
                   client.last_name,
                   client.balance);
        }
    }
}

// -------- SEARCH --------
void search_account(FILE *fp)
{
    struct client_data client;
    unsigned int acc;

    printf("Enter account number: ");
    scanf("%u", &acc);

    if (!valid_account(acc))
    {
        printf("Invalid account number!\n");
        return;
    }

    read_account(fp, acc, &client);

    if (client.acct_num == 0)
    {
        printf("Account not found!\n");
    }
    else
    {
        printf("Found: %d %s %s %.2f\n",
               client.acct_num,
               client.first_name,
               client.last_name,
               client.balance);
    }
}

// -------- COUNT --------
void count_accounts(FILE *fp)
{
    struct client_data client;
    int count = 0;

    rewind(fp);

    while (fread(&client, sizeof(client), 1, fp))
    {
        if (client.acct_num != 0)
            count++;
    }

    printf("Total active accounts: %d\n", count);
}

// -------- SORT --------
void sort_accounts(FILE *fp)
{
    struct client_data arr[MAX_ACCOUNTS];
    int count = 0;

    rewind(fp);

    while (fread(&arr[count], sizeof(struct client_data), 1, fp))
    {
        if (arr[count].acct_num != 0)
            count++;
    }

    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            if (arr[j].balance < arr[j + 1].balance)
            {
                struct client_data temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }

    printf("\n=== SORTED ACCOUNTS (By Balance) ===\n");
    printf("%-6s%-15s%-15s%-10s\n",
           "AccNo", "FirstName", "LastName", "Balance");

    for (int i = 0; i < count; i++)
    {
        printf("%-6d%-15s%-15s%-10.2f\n",
               arr[i].acct_num,
               arr[i].first_name,
               arr[i].last_name,
               arr[i].balance);
    }
}

// -------- MAIN --------
int main()
{
    initialize_file();

    FILE *fp = fopen("credit.dat", "rb+");
    unsigned int choice;

    while ((choice = menu()) != 10)
    {
        switch (choice)
        {
        case 1: text_file(fp); break;
        case 2: update_account(fp); break;
        case 3: new_account(fp); break;
        case 4: list_accounts(fp); break;
        case 5: delete(fp); break;
        case 6: search_account(fp); break;
        case 7: count_accounts(fp); break;
        case 8: reset_database(); break;
        case 9: sort_accounts(fp); break;
        default: printf("Invalid choice!\n");
        }
    }

    fclose(fp);
    return 0;
}