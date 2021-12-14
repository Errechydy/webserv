#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main (void)
{
    char *http_user_agent;
    printf ("Content-type: text/plain\n\n");
    http_user_agent = getenv ("REQUEST_METHOD");
    if (http_user_agent == NULL) {
        printf ("Oops! Your browser failed to set the HTTP_USER_AGENT ");
        printf ("environment variable!\n");
    } else if (!strncmp (http_user_agent, "Mosaic", 6)) {
        printf ("I guess you are sticking with the original, huh?\n");
    } else if (!strncmp (http_user_agent, "Mozilla", 7)) {
        printf ("Well, you are not alone. A majority of the people are ");
        printf ("using Netscape Navigator!\n");
    } else if (!strncmp (http_user_agent, "Lynx", 4)) {
        printf ("Lynx is great, but go get yourself a graphic browser!\n");
    } else {
        printf ("I see you are using the %s browser.\n", http_user_agent);
        printf ("I don't think it's as famous as Netscape, Mosaic or Lynx!\n");
    }
    return (0);
}