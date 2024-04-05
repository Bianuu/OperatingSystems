#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define citeste "REQ_PIPE_37849"
#define scrie "RESP_PIPE_37849"
#define start "START#"

int main()
{
    unlink(scrie);

    int fdscrie=-1;
    int fdciteste=-1;

    int varianttt=37849;

    char buffer;
    char cuvant[250];
    int index=0;

    char valoare[]="VALUE";
    char hastag='#';

    if(mkfifo(scrie,0600)!=0)
    {
        printf("ERROR\ncannot create the response pipe\n");
        return 1;
    }

    fdciteste=open(citeste,O_RDONLY);

    if(fdciteste==-1)
        printf("ERROR\ncannot open the request pipe\n");

    fdscrie=open(scrie,O_WRONLY);

    if(fdscrie==-1)
        printf("ERROR\ncannot open the response pipe\n");

    if(write(fdscrie,&start,6)==6)
        printf("SUCCESS\n");

    while(read(fdciteste,&buffer,1)!=0)
    {
        if(buffer!='#')
        {
            cuvant[index]=buffer;
            index++;
        }
        else
        {
            cuvant[index]='\0';

            if(strcmp(cuvant,"EXIT")==0)
                break;

            if(strcmp(cuvant,"VARIANT")==0)
            {
                for(int i=0; i<strlen(cuvant); i++)
                    write(fdscrie,&cuvant[i],1);
                write(fdscrie,&hastag,1);

                strcpy(cuvant,"");
                index=0;

                write(fdscrie,&varianttt,sizeof(varianttt));

                for(int i=0; i<strlen(valoare); i++)
                    write(fdscrie,&valoare[i],1);
                write(fdscrie,&hastag,1);
            }
        }
    }

    unlink(citeste);

    close(fdciteste);
    close(fdscrie);
    return 0;
}
