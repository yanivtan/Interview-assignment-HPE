#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
///cr+lf+5 tab + output without semicolon
char* random_string(char * string, unsigned length);

int main(int argc,char* argv[])
{
    char* ch=NULL;
    char* string ="-scr";
    int mode=0; //mode1 - scr mode2 - not scr
    int max_events=1000;
    int test_length=1;
    float ramp_up=0.2;
    float tear_down=0.2;

    ///mode
    ch =argv[1];
    if (!strcmp(ch,string)) mode=1;
    else mode=2;

    if (mode==1)
    {
        max_events = atoi((argv[4]));
        test_length = atoi(argv[6]);
        ramp_up = atof(argv[8]);
        tear_down =atof(argv[10]);
    }
    else
    {
        max_events = atoi((argv[2]));
        test_length = atoi(argv[3]);
        ramp_up = atof(argv[4]);
        tear_down =atof(argv[5]);
    }

    ///input check
    if (test_length>60) exit(1);
    if (test_length<1) exit(2);
    if (ramp_up>15)  exit(3);
    if (ramp_up<0)  exit(4);
    if (tear_down>15)  exit(5);
    if (tear_down<0)  exit(6);

    FILE* fp; //file read
    FILE* f; // file write
    FILE* f2;
    int i=0;
    int j=0;
    char str[300];
    char random_line[300];
    int arr_rate[60];
    float ramp_up_rate=0;
    float tear_down_rate=0;
    float time_in_max_events=0;
    int ramp_up_flag=1;
    int tear_down_flag=0;
    int max_events_counter=0;
    int nLines=0;
    int randLine;
    int randLine2;
    int count=0;
    int output_choise;
    int provided=0;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char randNum128[128];
    char randNum100[100];

    ///initilaize rate
    arr_rate[0]=0;

    ///open log file
    if (mode==1) fp = fopen (argv[2], "r");
    else fp = fopen (argv[1], "r");

    if(fp == NULL)
    {
        printf("not provided\n");
        provided=2;
    }

    if (provided==2)
    {
        ///create log file
        f2 = fopen ("Log file", "w");
        if (f2 == NULL)
        {
            exit(1);
        }

        for (i=0;i<100000;i++)
        {
            fprintf(f2,"ISO8601");
            fprintf(f2,"%d-%d-%d %d:%d:%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
            fprintf(f2," ");
            fprintf(f2,"%d",i);
            fprintf(f2," ");
            fprintf(f2,"%s",random_string(randNum128,128));
            fprintf(f2,"%s\n",random_string(randNum100,100));
        }
        fclose(f2);
    }

    ///log calculate
    ramp_up_rate=max_events/ramp_up;
    tear_down_rate=max_events/tear_down;
    time_in_max_events=test_length-ramp_up-tear_down;
    for (i=1;i<test_length;i++)
    {
        if (ramp_up_flag==1)
        {
            arr_rate[i]=arr_rate[i-1]+ramp_up_rate;
            if(arr_rate[i]>max_events) arr_rate[i]=max_events;
        }
        if (arr_rate[i-1]>=max_events)
        {
            ramp_up_flag=0;
            arr_rate[i]=max_events;
            max_events_counter++;
        }
        if (max_events_counter>=time_in_max_events)
        {
            tear_down_flag=1;
        }
        if (tear_down_flag==1)
        {
            arr_rate[i]=arr_rate[i-1]-tear_down_rate;
        }
    }

    randLine2 = rand()%3; ///chose between output
    if (randLine2==0) output_choise=1;
    else if (randLine2==1) output_choise=2;
    else output_choise=3;
    printf("chose output=%d\n",output_choise);

    ///open output file
    if(mode==1) f = fopen (argv[output_choise+11], "w");
    else f = fopen (argv[output_choise+5], "w");

    if(f == NULL)
    {
      perror("Error opening file");
      return(-1);
    }

    ///write to output
    if (provided==0)
    {
        for (i=0;i<test_length;i++)
        {
            for(j=0;j<arr_rate[i];j++)
            {
                if (fgets(str,300, fp)!=NULL)
                {
                    //printf("%s",str);
                    randLine= rand()%3; //0,1,2
                    if (randLine==1) fprintf(f, "%s", str);
                }
            }
        }
    }
    else //not provided
    {
        f2 = fopen ("Log file", "r");
        for (i=0;i<test_length;i++)
        {
            for(j=0;j<arr_rate[i];j++)
            {
                if (fgets(str,300, f2)!=NULL)
                {
                    //printf("%s",str);
                    randLine= rand()%3; //0,1,2
                    if (randLine==1) fprintf(f, "%s", str);
                }
            }
        }
    }

    fprintf(f, "My process ID: %d\n", getpid());
    fprintf(f,"Tools Log file:\n");
    for (i=0;i<test_length;i++) fprintf(f,"min=%d events=%d\n",i,arr_rate[i]);
    fprintf(f,"Brief_log:\n");
    fprintf(f,"max_events=%d\n",max_events);
    fprintf(f,"test_length=%d\n",test_length);
    fprintf(f,"ramp_up=%f\n",ramp_up);
    fprintf(f,"tear_down=%f\n",tear_down);

    fclose(f);
    if (provided==0) fclose(fp);
    else fclose(f2);
}

char* random_string(char * string, unsigned length)
{
    srand((unsigned int) time(0) + getpid());
    /* ASCII characters 65 to 90 */
    unsigned int num_chars = length - 1;
    unsigned int i;
    for (i = 0; i < num_chars; ++i)
    {
      string[i] = rand() % (90 - 65 + 1) + 65;
    }
    string[num_chars] = '\0';
    return string;
}
