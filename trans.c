#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACCOUNTS 100
#define DAILY_LIMIT 5000

struct client_data
{
    unsigned int acct_num;
    char first_name[20];
    char last_name[20];
    double balance;
    char password[20];
    double daily_withdrawn;
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

// -------- READ ACCOUNT --------
int read_account(FILE *fp, unsigned int acc, struct client_data *client)
{
    fseek(fp, (acc - 1) * sizeof(struct client_data), SEEK_SET);
    return fread(client, sizeof(struct client_data), 1, fp);
}

// -------- WRITE ACCOUNT --------
void write_account(FILE *fp, unsigned int acc, struct client_data *client)
{
    fseek(fp, (acc - 1) * sizeof(struct client_data), SEEK_SET);
    fwrite(client, sizeof(struct client_data), 1, fp);
}

// -------- AUTO TEXT FILE --------
void text_file(FILE *fp)
{
    FILE *out = fopen("accounts.txt", "w");
    struct client_data client;

    rewind(fp);

    fprintf(out,"%-6s%-15s%-15s%-10s\n",
            "AccNo","FirstName","LastName","Balance");

    while (fread(&client,sizeof(client),1,fp))
    {
        if (client.acct_num != 0)
        {
            fprintf(out,"%-6d%-15s%-15s%-10.2f\n",
                    client.acct_num,
                    client.first_name,
                    client.last_name,
                    client.balance);
        }
    }

    fclose(out);
}

// -------- PASSWORD CHECK --------
int check_password(struct client_data client)
{
    char input[20];

    printf("Enter password: ");
    scanf("%s", input);

    if (strcmp(input, client.password) != 0)
    {
        printf("Wrong password!\n");
        return 0;
    }

    return 1;
}

// -------- MENU --------
int menu()
{
    int choice;

    printf("\n===== BANK SYSTEM MENU =====\n");
    printf("1. Update account\n");
    printf("2. Add new account\n");
    printf("3. List accounts\n");
    printf("4. Delete account\n");
    printf("5. Search account\n");
    printf("6. Count accounts\n");
    printf("7. Reset database\n");
    printf("8. Sort accounts\n");
    printf("9. Transfer money\n");
    printf("10. Change password\n");
    printf("11. Show account\n");
    printf("12. Exit\n");

    printf("Enter choice: ");
    scanf("%d", &choice);

    return choice;
}

// -------- RESET --------
void reset_database(FILE *fp)
{
    struct client_data blank = {0};

    rewind(fp);

    for (int i = 0; i < MAX_ACCOUNTS; i++)
        fwrite(&blank,sizeof(blank),1,fp);

    text_file(fp);

    printf("Database reset successful!\n");
}

// -------- ADD ACCOUNT --------
void new_account(FILE *fp)
{
    struct client_data client = {0};
    unsigned int acc;

    printf("Enter account number: ");
    scanf("%u",&acc);

    read_account(fp,acc,&client);

    if (client.acct_num != 0)
    {
        printf("Account already exists!\n");
        return;
    }

    printf("Enter FirstName: ");
    scanf("%s",client.first_name);
    printf("Enter LastName:");
    scanf("%s",client.last_name);
    printf("Enter Balance: ");
    scanf("%lf",&client.balance);

    printf("Set password: ");
    scanf("%s", client.password);

    client.acct_num = acc;
    client.daily_withdrawn = 0;

    write_account(fp,acc,&client);

    text_file(fp);

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
    scanf("%u",&acc);

    read_account(fp,acc,&client);

    if (client.acct_num == 0)
    {
        printf("Account not found!\n");
        return;
    }

    if (!check_password(client))
        return;

    printf("1. Deposit\n2. Withdraw\n");
    scanf("%d",&choice);

    printf("Enter amount: ");
    scanf("%lf",&amount);

    if (choice == 2)
    {
        if (client.balance < amount)
        {
            printf("Insufficient balance!\n");
            return;
        }

        if (client.daily_withdrawn + amount > DAILY_LIMIT)
        {
            printf("Daily limit exceeded!\n");
            return;
        }

        client.balance -= amount;
        client.daily_withdrawn += amount;
    }
    else
    {
        client.balance += amount;
    }

    write_account(fp,acc,&client);

    text_file(fp);

    printf("Transaction successful!\n");
}

// -------- DELETE --------
void delete_account(FILE *fp)
{
    struct client_data blank = {0};
    unsigned int acc;

    printf("Enter account number: ");
    scanf("%u",&acc);

    write_account(fp,acc,&blank);

    text_file(fp);

    printf("Account deleted!\n");
}

// -------- LIST --------
void list_accounts(FILE *fp)
{
    struct client_data client;

    rewind(fp);

    printf("\n===== ACCOUNT LIST =====\n");

    while (fread(&client,sizeof(client),1,fp))
    {
        if (client.acct_num != 0)
        {
            printf("%d %s %s %.2f\n",
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
    scanf("%u",&acc);

    read_account(fp,acc,&client);

    if (client.acct_num == 0)
        printf("Account not found!\n");
    else
        printf("%d %s %s %.2f\n",
               client.acct_num,
               client.first_name,
               client.last_name,
               client.balance);
}

// -------- COUNT --------
void count_accounts(FILE *fp)
{
    struct client_data client;
    int count = 0;

    rewind(fp);

    while (fread(&client,sizeof(client),1,fp))
        if (client.acct_num != 0)
            count++;

    printf("Total accounts: %d\n",count);
}

// -------- TRANSFER --------
void transfer_money(FILE *fp)
{
    unsigned int from,to;
    double amount;
    struct client_data s,r;

    printf("From account: ");
    scanf("%u",&from);

    printf("To account: ");
    scanf("%u",&to);

    read_account(fp,from,&s);
    read_account(fp,to,&r);

    if (!check_password(s))
        return;

    printf("Amount: ");
    scanf("%lf",&amount);

    if (s.balance < amount)
    {
        printf("Insufficient balance\n");
        return;
    }

    s.balance -= amount;
    r.balance += amount;

    write_account(fp,from,&s);
    write_account(fp,to,&r);

    text_file(fp);

    printf("Transfer successful\n");
}

// -------- CHANGE PASSWORD --------
void change_password(FILE *fp)
{
    struct client_data client;
    unsigned int acc;

    printf("Enter account number: ");
    scanf("%u",&acc);

    read_account(fp,acc,&client);

    if (!check_password(client))
        return;

    printf("Enter new password: ");
    scanf("%s",client.password);

    write_account(fp,acc,&client);

    printf("Password updated!\n");
}

// -------- SHOW ACCOUNT --------
void show_account(FILE *fp)
{
    struct client_data client;
    unsigned int acc;

    printf("Enter account number: ");
    scanf("%u",&acc);

    read_account(fp,acc,&client);

    if (!check_password(client))
        return;

    printf("\nAccount Details\n");
    printf("Account No : %d\n",client.acct_num);
    printf("Name       : %s %s\n",
           client.first_name,
           client.last_name);
    printf("Balance    : %.2f\n",client.balance);
}
// -------- SORT ACCOUNTS --------
void sort_accounts(FILE *fp)
{
    struct client_data arr[MAX_ACCOUNTS];
    struct client_data client;
    int count = 0, choice;

    rewind(fp);

    // Load accounts
    while (fread(&client, sizeof(client), 1, fp))
    {
        if (client.acct_num != 0)
        {
            arr[count++] = client;
        }
    }

    if (count == 0)
    {
        printf("No accounts found!\n");
        return;
    }

    printf("\nSort By\n");
    printf("1. Account Number\n");
    printf("2. Balance\n");
    printf("3. Name\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    // Sorting
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            int swap = 0;

            switch (choice)
            {
                case 1:
                    if (arr[j].acct_num > arr[j + 1].acct_num)
                        swap = 1;
                    break;

                case 2:
                    if (arr[j].balance < arr[j + 1].balance)
                        swap = 1;
                    break;

                case 3:
                    if (strcmp(arr[j].first_name, arr[j + 1].first_name) > 0)
                        swap = 1;
                    break;

                default:
                    printf("Invalid choice!\n");
                    return;
            }

            if (swap)
            {
                struct client_data temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }

    printf("\n===== SORTED ACCOUNTS =====\n");
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

    FILE *fp = fopen("credit.dat","rb+");
    int choice;

    while ((choice = menu()) != 12)
    {
        switch (choice)
        {
            case 1: update_account(fp); break;
            case 2: new_account(fp); break;
            case 3: list_accounts(fp); break;
            case 4: delete_account(fp); break;
            case 5: search_account(fp); break;
            case 6: count_accounts(fp); break;
            case 7: reset_database(fp); break;
            case 8: sort_accounts(fp); break;
            case 9: transfer_money(fp); break;
            case 10: change_password(fp); break;
            case 11: show_account(fp); break;
        }
    }

    fclose(fp);
    return 0;
}