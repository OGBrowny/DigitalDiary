#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <time.h>

void getHiddenPassword(char *password);
void setPassword();
int verifyPassword();
int isValidDate(char *date);
int isFutureDate(char *date);
void createEntry();
void viewEntry();
void editEntry();

int main() {
    int choice;
    FILE *fp;

    // Check if password exists
    fp = fopen("password.txt","r");
    if(fp == NULL){
        printf("First time user detected.\n");
        setPassword();
    } else {
        fclose(fp);
        if(!verifyPassword()){
            printf("Access denied.\n");
            return 0;
        }
    }

    printf("\nLogin successful!\n");
    printf("Welcome to your Digital Diary\n");

    while(1){
        printf("\n===== DIGITAL DIARY MENU =====\n");
        printf("1. Create Entry\n");
        printf("2. View Entry\n");
        printf("3. Edit Entry\n");
        printf("4. Exit\n");
        printf("Choose option: ");
        scanf("%d",&choice);

        switch(choice){
            case 1: createEntry(); break;
            case 2: viewEntry(); break;
            case 3: editEntry(); break;
            case 4: 
                printf("\nThank you for using Digital Diary.\nGoodbye!\n");
                exit(0);
            default: printf("Invalid choice.\n");
        }
    }
}

/* ---------------- PASSWORD ---------------- */

void getHiddenPassword(char *password){
    char ch;
    int i = 0;
    while(1){
        ch = getch();
        if(ch == 13){ // Enter
            password[i] = '\0';
            break;
        } else if(ch == 8 && i>0){ // Backspace
            i--;
            printf("\b \b");
        } else {
            password[i++] = ch;
            printf("*");
        }
    }
    printf("\n");
}

void setPassword(){
    FILE *fp;
    char pass[50];
    printf("Set new password: ");
    getHiddenPassword(pass);
    fp = fopen("password.txt","w");
    fprintf(fp,"%s",pass);
    fclose(fp);
    printf("Password set successfully.\n");
}

int verifyPassword(){
    FILE *fp;
    char saved[50], entered[50];
    fp = fopen("password.txt","r");
    fgets(saved,sizeof(saved),fp);
    fclose(fp);
    printf("Enter password: ");
    getHiddenPassword(entered);
    return strcmp(saved,entered) == 0;
}

/* ---------------- DATE VALIDATION ---------------- */

int isValidDate(char *date){
    int y,m,d;
    int daysInMonth[]={31,28,31,30,31,30,31,31,30,31,30,31};
    if(strlen(date)!=10) return 0;
    if(date[4]!='-' || date[7]!='-') return 0;
    for(int i=0;i<10;i++){
        if(i==4 || i==7) continue;
        if(!isdigit(date[i])) return 0;
    }
    sscanf(date,"%d-%d-%d",&y,&m,&d);
    if(m<1||m>12) return 0;
    if(d<1||d>daysInMonth[m-1]) return 0;
    return 1;
}

int isFutureDate(char *date){
    int y,m,d;
    sscanf(date,"%d-%d-%d",&y,&m,&d);
    time_t t = time(NULL);
    struct tm *now = localtime(&t);
    int cy = now->tm_year+1900;
    int cm = now->tm_mon+1;
    int cd = now->tm_mday;
    if(y>cy) return 1;
    if(y==cy && m>cm) return 1;
    if(y==cy && m==cm && d>cd) return 1;
    return 0;
}

/* ---------------- CREATE ENTRY ---------------- */

void createEntry(){
    FILE *fp;
    char date[20], line[1000], filename[30];
    do{
        printf("Enter date (YYYY-MM-DD): ");
        scanf("%s",date);
        if(!isValidDate(date)) printf("Invalid date format.\n");
        else if(isFutureDate(date)) printf("Future dates are not allowed.\n");
    } while(!isValidDate(date) || isFutureDate(date));

    sprintf(filename,"%s.txt",date);
    fp = fopen(filename,"w");
    printf("\nWrite diary.\nTo SAVE: type ~ alone on a new line and press Enter.\n\n");
    getchar(); // clear input buffer

    while(1){
        fgets(line,sizeof(line),stdin);
        if(strcmp(line,"~\n") == 0) break;
        fprintf(fp,"%s",line);
    }
    fclose(fp);
    printf("Diary saved successfully.\n");
}

/* ---------------- VIEW ENTRY ---------------- */

void viewEntry(){
    FILE *fp;
    char date[20], line[1000], filename[30];
    printf("Enter date to view: ");
    scanf("%s",date);
    sprintf(filename,"%s.txt",date);
    fp = fopen(filename,"r");
    if(fp == NULL){ printf("No entry found.\n"); return; }
    printf("\nDiary Entry:\n");
    while(fgets(line,sizeof(line),fp)) printf("%s",line);
    fclose(fp);
}

/* ---------------- EDIT ENTRY ---------------- */

void editEntry(){
    FILE *fp;
    char date[20], filename[30];
    char lines[1000][1000]; // store diary lines
    int count = 0, choice, lineNo;
    char input[1000];

    printf("Enter date to edit: ");
    scanf("%s",date);
    sprintf(filename,"%s.txt",date);

    // Read existing diary into memory
    fp = fopen(filename,"r");
    if(fp == NULL){ printf("No diary found.\n"); return; }
    while(fgets(lines[count], sizeof(lines[count]), fp)) count++;
    fclose(fp);

    printf("\nCurrent Diary:\n");
    for(int i = 0; i < count; i++)
        printf("%d: %s", i+1, lines[i]);

    printf("\nEdit Options:\n");
    printf("1. Edit existing line(s)\n");
    printf("2. Append new lines\n");
    printf("Choose option: ");
    scanf("%d",&choice);
    getchar(); // clear buffer

    if(choice == 1){
        while(1){
            printf("Enter line number to edit (0 to stop editing): ");
            scanf("%d",&lineNo);
            getchar();
            if(lineNo == 0) break;
            if(lineNo < 1 || lineNo > count){
                printf("Invalid line number.\n");
                continue;
            }
            printf("Enter new text for line %d (use ~ alone on new line to finish):\n", lineNo);
            int i = 0;
            while(1){
                fgets(input,sizeof(input),stdin);
                if(strcmp(input,"~\n") == 0) break;
                strcpy(lines[lineNo - 1 + i], input);
                i++;
            }
            if(i > 1) count = lineNo - 1 + i; // update total lines if multiple lines entered
        }
    } else if(choice == 2){
        printf("Append new lines (use ~ alone on new line to finish):\n");
        while(1){
            fgets(input,sizeof(input),stdin);
            if(strcmp(input,"~\n") == 0) break;
            strcpy(lines[count], input);
            count++;
        }
    } else {
        printf("Invalid choice.\n");
        return;
    }

    // Save all lines back to file
    fp = fopen(filename,"w");
    for(int i = 0; i < count; i++)
        fputs(lines[i],fp);
    fclose(fp);

    printf("Diary updated successfully.\n");
}
