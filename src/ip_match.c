#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>

int is_valid_ip(char * url){
    if(!url){
        return 0;
    }
    regex_t regex;
    char msgbuf[100];
    int ret = 0;
    int reti;

    /* Compile regular expression */
    reti = regcomp(&regex, 
        "^([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))."
         "([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))."
         "([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))."
         "([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))$", REG_EXTENDED);
    if (reti) {
        fprintf(stderr, "Could not compile regex\n");
        regerror(reti, &regex, msgbuf, sizeof (msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        exit(3);
    }

    /* Execute regular expression */
    reti = regexec(&regex, url, 0, NULL, 0);
    if (!reti) {
        printf("Valid URL\n");
        ret = 1;
    } else if (reti == REG_NOMATCH) { //This else if always executes.
        printf("Invalid URL\n");
    } else {
        regerror(reti, &regex, msgbuf, sizeof (msgbuf));
        fprintf(stderr, "IP Regex match failed: %s\n", msgbuf);
        exit(5);
    }

    /* Free compiled regular expression if you want to use the regex_t again */
    regfree(&regex);
    return ret;
}