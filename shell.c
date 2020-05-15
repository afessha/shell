#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX 512

void welcome();

void init();

void terminal(char *initialPATH, char *initialDIR);

void load_commandHistory(char *initialDIR, char *History[]);

void load_Aliases(char *initialDIR, char *aliasArray[11][2]);

void tokenizer(char *aliasArray[11][2], char input[], char *History[]);

void saveHistory(char input[], char *History[]);

void commandHub(char *aliasArray[11][2], char *systemInput[], char *History[]);

void cd(char *command[]);

void getpath(char *command[]);

void setpath(char *command[]);

void history(char *History[]);

void alias(char *aliasArray[11][2], char *command[]);

void unalias(char *aliasArray[11][2], char *command[]);

void lastCommand(char *aliasArray[11][2], char *History[]);

void relativeCommand(char *aliasArray[11][2], char *command[], char *History[]);

void specificCommand(char *aliasArray[11][2], char *command[], char *History[]);

void externalCommand(char *command[]);

void save_command(char *History[], char *initialDIR);

void save_alias(char *aliasArray[11][2], char *initialDIR);

void exitShell(char *aliasArray[11][2], char *initialPATH, char *initialDIR, char *History[]);

int main(void)
{
    welcome();
    init();
}

void welcome()
{
    printf("------------------\n");
    printf("   Simple Shell   \n");
    printf("------------------\n\n");
}

void init()
{
    char d[1024];
    char *initialDIR = getcwd(d, sizeof(d));

    printf("Current Directory: %s\n\n", initialDIR);

    char *initialPATH = getenv("PATH");

    chdir(getenv("HOME"));
    char c[1024];
    printf("Home Directory: %s\n", getcwd(c, sizeof(c)));
    printf("\n");
    printf("Path: %s\n\n", initialPATH);

    terminal(initialPATH, initialDIR);
}

void terminal(char *initialPATH, char *initialDIR)
{
    char *History[21];
    char *aliasArray[11][2];

    char *emptyCheck = malloc(1);

    strcat(emptyCheck, "\0");

    for (int i = 0; i < 21; i++)
    {
        History[i] = malloc(MAX);
        strcpy(History[i], emptyCheck);
    }

    for (int i = 0; i < 11; i++)
    {
        aliasArray[i][0] = malloc(MAX);
        aliasArray[i][1] = malloc(MAX);

        strcpy(aliasArray[i][0], emptyCheck);
        strcpy(aliasArray[i][1], emptyCheck);
    }

    load_commandHistory(initialDIR, History);
    load_Aliases(initialDIR, aliasArray);

    while (1)
    {
        char input[513] = {'\t'};
        printf("> ");
        input[MAX] = '\n';
        if (fgets(input, 514, stdin) == NULL)
        {

            printf("%s\n", initialPATH);
            exitShell(aliasArray, initialPATH, initialDIR, History);
            exit(0);
        }
        while (input[MAX] != '\n')
        {
            printf("Error: too many characters.\n ");
            input[MAX] = '\n';
            char c;
            while ((c = getchar()) != '\n' && c != EOF)
            {
            };

            printf(">");
            if (fgets(input, 514, stdin) == NULL)
            {
                exitShell(aliasArray, initialPATH, initialDIR, History);
                exit(0);
            }
        }

        if (strcmp(input, "exit\n") == 0)
        {

            exitShell(aliasArray, initialPATH, initialDIR, History);
            exit(0);
        }

        if (input[0] != '\n' && input[0] != ' ')
        {
            tokenizer(aliasArray, input, History);
        }
    }
}

void load_commandHistory(char *initialDIR, char *History[])
{

    FILE *fp;
    chdir(initialDIR);
    fp = fopen(".hist_list", "r");
    if (fp == NULL)
    {
        printf("...History file not found...\n");
    }
    else
    {

        char buffer[MAX];
        int count = 0;

        while (fgets(buffer, MAX, fp) != NULL)
        {
            int length = strlen(buffer);

            buffer[length] = '\0';
            char val[MAX];

            strtok(buffer, "\n");
            strcpy(val, buffer);
            strcpy(History[count], val);
            count++;
        }
        fclose(fp);
    }
    chdir(getenv("HOME"));
}

void load_Aliases(char *initialDIR, char *aliasArray[11][2])
{

    FILE *fp;
    chdir(initialDIR);
    fp = fopen(".aliases", "r");

    if (fp == NULL)
    {
        printf("...Alias file not found...\n");
    }
    else
    {

        char buffer[MAX];
        int count = 0;

        while (fgets(buffer, MAX, fp) != NULL)
        {
            int length = strlen(buffer);

            buffer[length] = '\0';
            char name[length];
            char command[length];

            char *input;

            input = strtok(buffer, " ");
            strcpy(name, input);
            strcpy(aliasArray[count][0], name);

            input = strtok(NULL, "\n");
            strcpy(command, input);
            strcpy(aliasArray[count][1], command);
            count++;
        }
        fclose(fp);
    }

    chdir(getenv("HOME"));
}

void tokenizer(char *aliasArray[11][2], char input[], char *History[])
{
    char *systemInput[50];
    char toSave[MAX];

    strcpy(toSave, input);

    char *inputToken = strtok(input, " '\t' \n | < > & ;");

    int index = 0;
    if (inputToken != NULL)
    {
        while (inputToken != NULL)
        {
            systemInput[index] = inputToken;
            index++;
            inputToken = strtok(NULL, " '\t' \n | < > & ;");
        }
    }

    systemInput[index] = NULL;

    if (strcmp(systemInput[0], "!!") == 0)
    {
        commandHub(aliasArray, systemInput, History);
    }

    else if (strncmp(systemInput[0], "!-", 2) == 0 || strncmp(systemInput[0], "!", 1) == 0)
    {
        commandHub(aliasArray, systemInput, History);
    }

    else
    {
        saveHistory(toSave, History);
        commandHub(aliasArray, systemInput, History);
    }
}

void saveHistory(char input[], char *History[])
{
    if (strcmp(History[19], "\0"))
    { //if the history is full
        for (int i = 0; i < 20; i++)
        {
            strcpy(History[i], History[i + 1]);
        }
        free(History[20]);
        int i = 0;
        while (input[i] != '\n')
        {
            i++;
        }
        input[i] = '\0';
        char *value = malloc(MAX);
        value = strcat(value, input);

        strcpy(History[19], value);
    }

    else if (!strcmp(History[19], "\0"))
    { // if the history is not full
        int i = 0;
        while (input[i] != '\n')
        {
            i++;
        }
        input[i] = '\0';

        char *value = malloc(MAX);
        value = strcat(value, input);
        int j = 0;
        while (strcmp(History[j], "\0"))
        {
            j++;
        }
        strcpy(History[j], value);
    }
}

void commandHub(char *aliasArray[11][2], char *systemInput[], char *History[])
{

    for (int i = 0; i < 10; i++)
    {
        if (!strcmp(systemInput[0], aliasArray[i][0]))
        {
            char holder[MAX];
            strcpy(holder, aliasArray[i][1]);
            tokenizer(aliasArray, holder, History);
            return;
        }
    }

    if (!strcmp(systemInput[0], "cd"))
    {
        cd(systemInput);
    }

    else if (!strcmp(systemInput[0], "getpath"))
    {
        getpath(systemInput);
    }

    else if (!strcmp(systemInput[0], "setpath"))
    {
        setpath(systemInput);
    }

    else if (!strcmp(systemInput[0], "history"))
    {
        history(History);
    }

    else if (!strcmp(systemInput[0], "alias"))
    {
        alias(aliasArray, systemInput);
    }

    else if (!strcmp(systemInput[0], "unalias"))
    {
        unalias(aliasArray, systemInput);
    }

    else if (!strcmp(systemInput[0], "!!"))
    {
        lastCommand(aliasArray, History);
    }

    else if (!strncmp(systemInput[0], "!-", 2))
    {
        relativeCommand(aliasArray, systemInput, History);
    }

    else if (!strncmp(systemInput[0], "!", 1))
    {
        specificCommand(aliasArray, systemInput, History);
    }

    else
    {
        externalCommand(systemInput);
    }
}

void cd(char *command[])
{
    if (command[1] != NULL && *command[1] != '~' && command[2] == NULL)
    {
        int i = chdir(command[1]);
        if (i < 0)
        {
            printf("Could not change directory to: %s. Not directory\n",
                   command[1]);
        }
        else
        {
            char cwd[1024];
            printf("%s\n", getcwd(cwd, sizeof(cwd)));
        }
    }
    else if (command[1] != NULL && *command[1] != '~' && command[2] != NULL)
    {
        printf("Directory could not be changed. Too many parameters\n");
    }
    else
    {
        int i = chdir(getenv("HOME"));
        if (i < 0)
        {
            printf("Directory could not be changes\n");
        }
        else
        {
            char cwd[1024];
            printf("%s\n", getcwd(cwd, sizeof(cwd)));
        }
    }
}

void getpath(char *command[])
{

    if (strcmp(command[0], "getpath") == 0 && command[1] == NULL)
    {
        printf("$ Current Path: %s\n", getenv("PATH"));
    }
    else if (strcmp(command[0], "getpath") == 0 && command[1] != NULL)
    {
        printf("Error: No such path\n");
    }
}

void setpath(char *command[])
{
    if (strcmp(command[0], "setpath") == 0 && command[1] != NULL && command[2] != NULL)
    {
        printf("Error: Too many parameters\n");
    }

    else if (strcmp(command[0], "setpath") == 0 && command[1] != NULL)
    {
        setenv("PATH", command[1], 1);
        printf("$ Path Set To: %s\n", getenv("PATH"));
    }
    else if (strcmp(command[0], "setpath") == 0 && command[1] == NULL)
    {
        printf("Error: No path provided\n");
    }
}

void history(char *History[])
{
    if (!strcmp(History[0], "\0"))
    {
        printf("No commands entered yet\n");
    }
    else
    {
        for (int i = 0; i < 20; i++)
        {
            if (strcmp(History[i], "\0"))
            {
                printf("%d : %s \n", (i + 1), History[i]);
            }
        }
    }
}

void alias(char *aliasArray[11][2], char *command[])
{
    if (command[1] == NULL)
    {
        int j = 0;
        for (int i = 0; i < 10; i++)
        {
            if (strcmp(aliasArray[i][0], "\0") && strcmp(aliasArray[i][1], "\0"))
            {
                printf("Alias: \"%s\" Command: \"%s\"\n", aliasArray[i][0], aliasArray[i][1]);
                j++;
            }
        }
        if (j == 0)
        {
            printf("No aliases set\n");
        }
        return;
    }

    else if (command[2] == NULL)
    {
        printf("No command set\n");
        return;
    }

    if (!strcmp(command[1], "alias"))
    {
        printf("Error: cannot alias 'alias'");
        printf("\n");
        return;
    }

    char *temp = malloc(MAX);
    strcat(temp, command[2]);
    for (int i = 0; i < 10; i++)
    {
        if (!strcmp(temp, aliasArray[i][0]) && !strcmp(command[1], aliasArray[i][1]))
        {
            printf("Cannot create alias, it creates an infinite loop\n");
            return;
        }
    }
    int i = 3;
    while (command[i] != NULL)
    {
        strcat(temp, " ");
        strcat(temp, command[i]);
        i++;
    }

    char *aliasName = malloc(MAX);
    char *aliasCommand = malloc(MAX);
    aliasName = strcat(aliasName, command[1]);
    aliasCommand = strcat(aliasCommand, temp);

    for (int j = 0; j < 10; j++)
    {
        if (!strcmp(aliasArray[j][0], "\0"))
        {
            strcpy(aliasArray[j][0], aliasName);
            strcpy(aliasArray[j][1], aliasCommand);
            printf("Created alias \"%s\" for the command: \"%s\"\n", command[1], aliasCommand);
            return;
        }
        else if (!strcmp(aliasArray[j][0], aliasName))
        {
            printf("Changed alias \"%s\" from the command: \"%s\"\n", command[1], aliasArray[j][1]);
            strcpy(aliasArray[j][1], aliasCommand);
            printf("---to the command: %s\n", aliasCommand);

            return;
        }
    }
    printf("Alias list is full\n");
}

void unalias(char *aliasArray[11][2], char *command[])
{
    int j = 0;
    for (int i = 0; i < 10; i++)
    {
        if (!strcmp(command[1], aliasArray[i][0]))
        {
            printf("Unliased command: \"%s\" from alias \"%s\"\n", aliasArray[i][1], command[1]);
            strcpy(aliasArray[i][0], "\0");
            strcpy(aliasArray[i][1], "\0");
            j = 1;
        }
    }
    if (j == 0)
    {
        printf("Alias %s does not exist.\n", command[1]);
    }
}

void lastCommand(char *aliasArray[11][2], char *History[])
{

    int x = 0;
    if (!strcmp(History[0], "\0"))
    {
        printf("Error, no commands in history to execute \n");
    }
    else
    {
        char input[MAX];
        int j = 0;
        while (strcmp(History[j], "\0"))
        {
            j++;
        }

        strcpy(input, History[j - 1]);
        char *systemInput[50];
        char *inputToken = strtok(input, " '\t' \n | < > & ;");

        int index = 0;
        if (inputToken != NULL)
        {
            while (inputToken != NULL)
            {
                systemInput[index] = inputToken;
                index++;
                inputToken = strtok(NULL, " '\t' \n | < > & ;");
            }
        }
        systemInput[index] = NULL;
        commandHub(aliasArray, systemInput, History);
    }
}

void specificCommand(char *aliasArray[11][2], char *command[], char *History[])
{

    char *value = strtok(command[0], "!");
    if (value != NULL)
    {
        int x = atoi(value);
        if (x < 1 || x > 20)
        {
            printf("Error : index entered out of range \n");
        }
        else
        {
            if (!strcmp(History[x - 1], "\0"))
            {
                printf("Error, no commands in history to execute \n");
            }

            else
            {
                char input[MAX];
                strcpy(input, History[x - 1]);
                char *systemInput[50];
                char *inputToken = strtok(input, " '\t' \n | < > & ;");

                int index = 0;
                if (inputToken != NULL)
                {
                    while (inputToken != NULL)
                    {
                        systemInput[index] = inputToken;
                        index++;
                        inputToken = strtok(NULL, " '\t' \n | < > & ;");
                    }
                }
                systemInput[index] = NULL;
                commandHub(aliasArray, systemInput, History);
            }
        }
    }
}

void relativeCommand(char *aliasArray[11][2], char *command[], char *History[])
{

    char *value = strtok(command[0], "!-");
    if (value != NULL)
    {
        int x = atoi(value);
        if (x < 1 || x > 20)
        {
            printf("Error : index entered out of range \n");
        }
        else
        {
            int j = 1;
            while (strcmp(History[j], "\0"))
            {
                j++;
            }
            if (!strcmp(History[j - x], "\0") || j - x < 0)
            {
                printf("Error, no commands in history to execute \n");
            }
            else
            {
                char input[MAX];
                strcpy(input, History[j - x]);
                char *systemInput[50];
                char *inputToken = strtok(input, " '\t' \n | < > & ;");

                int index = 0;
                if (inputToken != NULL)
                {
                    while (inputToken != NULL)
                    {
                        systemInput[index] = inputToken;
                        index++;
                        inputToken = strtok(NULL, " '\t' \n | < > & ;");
                    }
                }
                systemInput[index] = NULL;
                commandHub(aliasArray, systemInput, History);
            }
        }
    }
}

void externalCommand(char *command[])
{
    pid_t c_pid, pid;
    int status;

    c_pid = fork();

    if (c_pid == -1)
    {
        perror("Error: fork failed");
        _exit(1);
    }

    if (c_pid == 0)
    {
        execvp(command[0], command);
        perror(command[0]);
        _exit(1);
    }
    else if (c_pid > 0)
    {
        if ((pid = wait(&status)) < 0)
        {
            perror("Error: wait failed");
            _exit(1);
        }
    }
}

void save_command(char *History[], char *initialDIR)
{
    FILE *fp;
    chdir(initialDIR);
    fp = fopen(".hist_list", "w+");
    if (fp == NULL)
    {
        printf("Save file not found");
    }
    else
    {
        for (int i = 0; i < 20; i++)
        {
            if (strcmp(History[i], "\0") != 0)
            {
                fprintf(fp, "%s", History[i]);
                if (strcmp(History[i + 1], "\0") != 0)
                {
                    fputs("\n", fp);
                }
            }
        }
        fclose(fp);
    }
}

void save_alias(char *aliasArray[11][2], char *initialDIR)
{
    FILE *fp;
    chdir(initialDIR);
    fp = fopen(".aliases", "w+");
    if (fp == NULL)
    {
        printf("Save file not found");
    }
    else
    {
        for (int i = 0; i < 11; i++)
        {
            if (strcmp(aliasArray[i][0], "\0") != 0)
            {
                fprintf(fp, "%s %s", aliasArray[i][0], aliasArray[i][1]);
                if (strcmp(aliasArray[i + 1][0], "\0") != 0)
                {
                    fputs("\n", fp);
                }
            }
        }
        fclose(fp);
    }
}

void exitShell(char *aliasArray[11][2], char *initialPATH, char *initialDIR, char *History[])
{
    printf("\nCurrent path: %s\n", getenv("PATH"));

    setenv("PATH", initialPATH, 1);

    printf("\n");
    printf("Restored path to: %s\n\n", getenv("PATH"));
    printf("Saving History..\n");

    save_command(History, initialDIR);
    save_alias(aliasArray, initialDIR);

    for (int i = 0; i < 21; i++)
    {
        free(History[i]);
    }

    for (int i = 0; i < 11; i++)
    {
        free(aliasArray[i][0]);
        free(aliasArray[i][1]);
    }

    printf("Exiting...\n");
    printf("\n");
    exit(0);
}
