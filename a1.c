#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>

#define const 100000

int indeplinire(char*);

///versiune ii 0 daca e fara size si 1 daca e cu size
///hasperm ii false daca nu se specifica si true cand se specifica
///fct adaptata din lab
void cmdlistsize(char *path, int size, int versiune, bool hasperm)
{
    DIR *dir = NULL;
    dir = opendir(path);
    struct dirent * entry;

    struct stat statbuf;
    char fullPath[512] = {};

    if (dir == NULL)
    {
        printf("ERROR\n invalid directory path");
        return;
    }
    else
    {
        printf("SUCCESS\n");

        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                if (versiune == 1)
                {
                    snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
                    if (lstat(fullPath, &statbuf) == 0 && S_ISREG(statbuf.st_mode) && statbuf.st_size < size)
                        printf("%s/%s\n", path, entry->d_name);
                }
                else if (hasperm == true)
                {
                    snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
                    if (lstat(fullPath, &statbuf) == 0 && (statbuf.st_mode &S_IWUSR) == S_IWUSR)
                        printf("%s/%s\n", path, entry->d_name);
                }
                else
                    printf("%s/%s\n", path, entry->d_name);
            }
        }
        free(path);
        closedir(dir);
        return;
    }
}

///fct adaptata din lab de Directoare la exemplu
///versiune e 0 daca e fara size si 1 daca e cu
///hasperm ii false daca nu se specifica si true cand se specifica
void cmdrecsize(bool semafor, char *path, int size, int versiune, bool hasperm)
{
    DIR *dir = NULL;
    dir = opendir(path);
    struct dirent *entry = NULL;

    char fullPath[2 * 512] = {};

    struct stat statbuf;

    if (dir == NULL)
    {
        printf("ERROR\n invalid directory path");
        return;
    }
    else
    {
        if (semafor == false)
        {
            semafor = true;
            printf("SUCCESS\n");
        }

        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                snprintf(fullPath, 2 * 512, "%s/%s", path, entry->d_name);
                if (lstat(fullPath, &statbuf) == 0)
                {
                    if (versiune == 1)
                    {
                        if (S_ISREG(statbuf.st_mode) && statbuf.st_size < size)
                            printf("%s\n", fullPath);

                        if (S_ISDIR(statbuf.st_mode))
                            cmdrecsize(semafor, fullPath, size, 1, hasperm);
                    }
                    else if (hasperm == true)
                    {
                        if ((statbuf.st_mode &S_IWUSR) == S_IWUSR && S_ISDIR(statbuf.st_mode))
                        {
                            cmdrecsize(semafor, fullPath, 0, 0, hasperm);
                            printf("%s\n", fullPath);
                        }
                    }
                    else
                    {
                        printf("%s\n", fullPath);
                        if (S_ISDIR(statbuf.st_mode))
                            cmdrecsize(semafor, fullPath, 0, 0, hasperm);
                    }
                }
            }
        }
    }
    closedir(dir);
}

///doar citim si afisam daca sunt respectate cerintele
///pentru erori ca sa stim motivul,cum scrie in cerinta,verificam dupa ce citim
void cmdparsare(char *path)
{
    int fd = open(path, O_RDONLY);

    if (fd == -1)
        perror("ERROR\n");

    char magic[3] = {};

    int header_size = 0;
    int version = 0;
    int no_of_sections = 0;

    read(fd, &magic, 2);
    magic[2] = '\0';
    if (strcmp(magic, "pe") != 0)
    {
        printf("ERROR\n wrong magic");
        return;
    }

    read(fd, &header_size, 2);

    read(fd, &version, 2);
    if (version < 92 || version > 179)
    {
        printf("ERROR\n wrong version");
        return;
    }

    read(fd, &no_of_sections, 1);
    if (no_of_sections < 2 || no_of_sections > 17)
    {
        printf("ERROR\n wrong sect_nr");
        return;
    }

    char name[21] = {};

    int type = 0;
    int offset = 0;
    int size = 0;

    for (int i = 0; i <= no_of_sections - 1; i++)
    {
        read(fd, &name, 20);
        name[20] = '\0';
        read(fd, &type, 4);
        read(fd, &offset, 4);
        read(fd, &size, 4);

        if (!(type == 87 || type == 56 || type == 99 || type == 21 || type == 73 || type == 95 || type == 86))
        {
            printf("ERROR\n wrong sect_types");
            return;
        }
    }

    printf("SUCCESS\n");

    printf("version=%d\n", version);

    printf("nr_sections=%d\n", no_of_sections);

    lseek(fd, 0, SEEK_SET);
    lseek(fd, 7, SEEK_CUR);

    for (int i = 0; i <= no_of_sections - 1; i++)
    {
        read(fd, &name, 20);
        name[20] = '\0';
        read(fd, &type, 4);
        read(fd, &offset, 4);
        read(fd, &size, 4);

        printf("section%d: %s %d %d\n", i + 1, name, type, size);
    }
}

///adaptare cmdrecsize versiune==1 si in rest stergem codu din restul programului
void cmdfinal(bool semafor, char *path)
{
    DIR *dir = NULL;
    dir = opendir(path);
    struct dirent *entry = NULL;

    char fullPath[2 * 512] = {};

    struct stat statbuf;

    if (dir == NULL)
    {
        perror("ERROR\n invalid directory path");
        return;
    }
    else
    {
        if (semafor == false)
        {
            printf("SUCCESS\n");
            semafor = true;
        }

        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                snprintf(fullPath, 2 * 512, "%s/%s", path, entry->d_name);
                if (lstat(fullPath, &statbuf) == 0)
                {
                    if (S_ISREG(statbuf.st_mode) && indeplinire(fullPath) == 1)
                        printf("%s\n", fullPath);

                    if (S_ISDIR(statbuf.st_mode))
                        cmdfinal(semafor, fullPath);
                }
            }
        }
    }
    closedir(dir);
}

void cmdextract(char *path, int section, int linie)
{
    ///validam datele/corectitudinea
    int fd = open(path, O_RDONLY);

    if (fd == -1)
        perror("ERROR\n");

    char magic[3] = {};

    int header_size = 0;
    int version = 0;
    int no_of_sections = 0;

    read(fd, &magic, 2);
    magic[2] = '\0';
    if (strcmp(magic, "pe") != 0)
    {
        printf("ERROR\n wrong magic");
        return;
    }

    read(fd, &header_size, 2);

    read(fd, &version, 2);
    if (version < 92 || version > 179)
    {
        printf("ERROR\n wrong version");
        return;
    }

    read(fd, &no_of_sections, 1);
    if (no_of_sections < 2 || no_of_sections > 17)
    {
        printf("ERROR\n wrong sect_nr");
        return;
    }

    char name[21] = {};

    int type = 0;
    int offset = 0;
    int size = 0;

    for (int i = 0; i <= no_of_sections - 1; i++)
    {
        read(fd, &name, 20);
        name[20] = '\0';
        read(fd, &type, 4);
        read(fd, &offset, 4);
        read(fd, &size, 4);

        if (!(type == 87 || type == 56 || type == 99 || type == 21 || type == 73 || type == 95 || type == 86))
        {
            printf("ERROR\n wrong sect_types");
            return;
        }

        if (i + 1 == section)
        {
            char *linia = (char*) malloc(size);

            char *bufferlinie = NULL;
            bufferlinie = (char*) malloc(size);

            char *bufferliniesectiune = NULL;
            bufferliniesectiune = (char*) malloc(size);

            ///calc linie
            if (bufferlinie == NULL || lseek(fd, offset, SEEK_SET) == -1 || read(fd, bufferlinie, size) != size)
            {
                if (bufferlinie == NULL)
                    perror("ERROR\n");
                else
                {
                    perror("ERROR\n");
                    free(bufferlinie);
                }
                return;
            }

            int contorlinii = 1;
            for (int i = 0; i < size; i++)
                if (bufferlinie[i] == '\n')
                    contorlinii++;

            ///calc sectiune
            if (bufferliniesectiune == NULL || lseek(fd, offset, SEEK_SET) == -1 || read(fd, bufferliniesectiune, size) != size)
            {
                if (bufferliniesectiune == NULL)
                    perror("ERROR\n");
                else
                {
                    perror("ERROR\n");
                    free(bufferliniesectiune);
                }
                return;
            }

            bool semafor = true;
            int cursor = 0;
            int contorliniiv2 = 0;

            for (int i = 0; i < size; i++)
            {
                if (contorliniiv2 == contorlinii - linie)
                {
                    if (bufferliniesectiune[i] == '\n')
                    {
                        linia[cursor] = '\0';
                        semafor = false;
                        break;
                    }

                    linia[cursor] = bufferliniesectiune[i];
                    cursor++;
                }
                else if (bufferliniesectiune[i] == '\n')
                    contorliniiv2++;
            }

            if (semafor == true)
                printf("ERROR\n invalid line");
            else
            {
                printf("SUCCESS\n");
                for (int j = 0; linia[j] != '\0'; j++)
                    printf("%c", linia[j]);
            }

            free(linia);
            free(bufferlinie);
            free(bufferliniesectiune);
            return;
        }
    }

    printf("ERROR\n invalid section");

}

int main(int argc, char **argv)
{
    int ok = 0;
    if (argc > 1)
    {
        ok = 1;
        if (strcmp(argv[1], "variant") == 0)
        {
            ok=1;
            printf("37849\n");
        }

        if (strcmp(argv[1], "list") == 0 && strstr(argv[2], "path="))
        {
            ok=1;
            char *path = (char*) malloc(const);

            cmdlistsize(strcpy(path, argv[2] + 5), 0, 0, false);
        }

        if (strcmp(argv[1], "list") == 0 && strcmp(argv[2], "recursive") == 0 && strstr(argv[3], "path="))
        {
            ok=1;
            char *path = (char*) malloc(const);

            cmdrecsize(false, strcpy(path, argv[3] + 5), 0, 0, false);

            free(path);
        }

        if (strcmp(argv[1], "list") == 0 && strcmp(argv[2], "has_perm_write") == 0 && strstr(argv[3], "path="))
        {
            ok=1;
            char *path = (char*) malloc(const);

            cmdlistsize(strcpy(path, argv[3] + 5), 0, 0, true);
        }

        if (strcmp(argv[1], "list") == 0 && strstr(argv[2], "size_smaller=") && strstr(argv[3], "path="))
        {
            ok=1;
            char *path = (char*) malloc(const);
            char *sizee = (char*) malloc(const);

            cmdlistsize(strcpy(path, argv[3] + 5), atoi(strcpy(sizee, argv[2] + 13)), 1, false);

            free(sizee);
        }

        if (strcmp(argv[1], "list") == 0 && strcmp(argv[2], "has_perm_write") == 0 && strcmp(argv[3], "recursive") == 0 && strstr(argv[4], "path="))
        {
            ok=1;
            char *path = (char*) malloc(const);

            cmdrecsize(false, strcpy(path, argv[4] + 5), 0, 0, true);

            free(path);
        }

        if (strcmp(argv[1], "list") == 0 && strcmp(argv[2], "recursive") == 0 && strstr(argv[3], "size_smaller=") && strstr(argv[4], "path="))
        {
            ok=1;
            char *path = (char*) malloc(const);
            char *sizee = (char*) malloc(const);

            cmdrecsize(false, strcpy(path, argv[4] + 5), atoi(strcpy(sizee, argv[3] + 13)), 1, false);

            free(path);
            free(sizee);
        }

        if (strcmp(argv[1], "list") == 0 && strstr(argv[2], "size_smaller=") && strcmp(argv[3], "recursive") == 0 && strstr(argv[4], "path="))
        {
            ok=1;
            char *path = (char*) malloc(const);
            char *sizee = (char*) malloc(const);

            cmdrecsize(false, strcpy(path, argv[4] + 5), atoi(strcpy(sizee, argv[2] + 13)), 1, false);

            free(path);
            free(sizee);
        }

        if (strcmp(argv[1], "parse") == 0 && strstr(argv[2], "path="))
        {
            ok=1;
            char *path = (char*) malloc(const);

            cmdparsare(strcpy(path, argv[2] + 5));

            free(path);
        }

        if (strcmp(argv[1], "findall") == 0 && strstr(argv[2], "path="))
        {
            ok=1;
            char *path = (char*) malloc(const);

            cmdfinal(false, strcpy(path, argv[2] + 5));

            free(path);
        }

        if (strcmp(argv[1], "extract") == 0 && strstr(argv[2], "path=") && strstr(argv[3], "section=") && strstr(argv[4], "line="))
        {
            ok=1;
            char *path = (char*) malloc(const);
            char *linee = (char*) malloc(const);
            char *sectionn = (char*) malloc(const);

            cmdextract(strcpy(path, argv[2] + 5), atoi(strcpy(sectionn, argv[3] + 8)), atoi(strcpy(linee, argv[4] + 5)));

            free(path);
            free(sectionn);
            free(linee);
        }

        if (ok == 0)
        {
            printf("ERROR\n");
            printf("comanda_invalida");
            return -10;
        }
    }

    return 0;
}

///functii ajutatoare pentru conditii

///transformare/adaptare cmdparse,identic 90%,aceleasi verificari doar ca fara afisare
int indeplinire(char *path)
{
    int fd = open(path, O_RDONLY);

    if (fd == -1)
        perror("ERROR\n");

    bool semafor_valid = true;

    char magic[3] = {};

    int header_size = 0;
    int version = 0;
    int no_of_sections = 0;

    read(fd, &magic, 2);
    magic[2] = '\0';

    if (strcmp(magic, "pe") != 0 && semafor_valid == true)
        semafor_valid = false;

    read(fd, &header_size, 2);

    read(fd, &version, 2);
    if ((version < 92 || version > 179) && semafor_valid == true)
        semafor_valid = false;

    read(fd, &no_of_sections, 1);
    if ((no_of_sections < 2 || no_of_sections > 17) && semafor_valid == true)
        semafor_valid = false;

    char name[21] = {};

    int type = 0;
    int offset = 0;
    int size = 0;

    for (int i = 0; i < no_of_sections && semafor_valid == true; i++)
    {
        read(fd, &name, 20);
        name[20] = '\0';
        read(fd, &type, 4);
        read(fd, &offset, 4);
        read(fd, &size, 4);

        if ((!(type == 87 || type == 56 || type == 99 || type == 21 || type == 73 || type == 95 || type == 86)) || size >= 1295)
            semafor_valid = false;

    }

    if (semafor_valid == true)
        return 1;
    else
        return 0;
}

