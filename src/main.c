#include "jsmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "version.h"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

static const char *JSON_STRING =
    "{\"id\": 1000, \"command\": \"scan\"}";

static int jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
    if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
            strncmp(json + tok->start, s, tok->end - tok->start) == 0)
    {
        return 0;
    }
    return -1;
}

static long int sessionId;

//static char *commandFromJSON;

static int getCommandFromJSON()
{
    enum t_enumCommandFromJSON {COMMAND_UNKNOWN,COMMAND_SCAN};
    int result;
    int i;
    int r;
    jsmn_parser p;
    jsmntok_t t[8]; /* We expect no more than 128 tokens */

    char * pEnd; /* using for strtol() */

    /* Выводим принятую строку JSON */
    printf("Input JSON: %s\n", JSON_STRING);

    jsmn_init(&p);
    r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t,
                   sizeof(t) / sizeof(t[0]));
    if (r < 0)
    {
        printf("Failed to parse JSON: %d\n", r);
        return 1;
    }

    /* Assume the top-level element is an object */
    if (r < 1 || t[0].type != JSMN_OBJECT)
    {
        printf("Object expected\n");
        return 1;
    }

    /* Loop over all keys of the root object */
    for (i = 1; i < r; i++)
    {
        if (jsoneq(JSON_STRING, &t[i], "id") == 0)
        {
            /* We may want to do strtol() here to get numeric value */
            printf("- ID: %.*lu\n", t[i + 1].end - t[i + 1].start,
                   sessionId = strtol((JSON_STRING + t[i + 1].start), &pEnd, 10));
            printf("Session ID = %lu\n", sessionId);
            i++;
        }
        else if (jsoneq(JSON_STRING, &t[i], "command") == 0)
        {
            /* We may use strndup() to fetch string value */
            if (strncmp(JSON_STRING + t[i + 1].start,"scan",4) == 0)
            {
                printf("- Command: %.*s\n", t[i + 1].end - t[i + 1].start,
                       JSON_STRING + t[i + 1].start);
                printf("Received command: SCAN\n");
                /*TODO: Вставить обработчик команды SCAN*/
                result = COMMAND_SCAN;
            }
            else
            {
                printf("- Command: %.*s\n", t[i + 1].end - t[i + 1].start,
                       JSON_STRING + t[i + 1].start);
                printf("Received unknown command\n");
                /*TODO: Вставить генерацию ошибки: Неизвестная команда*/
                result = COMMAND_UNKNOWN;
            }
            i++;
        }
        else
        {
            printf("Unexpected key: %.*s\n", t[i].end - t[i].start,
                   JSON_STRING + t[i].start);
        }
    }
    return result;
}

static enum t_enumCommandFromJSON {COMMAND_UNKNOWN,COMMAND_SCAN} enumCommandFromJSON;

int main()
{
    /* Print version OptiHost*/
    printf("OptiHost version: v%lu.%lu.%lu.%lu\n",MAJOR,MINOR,BUILD,REVISION);

    enumCommandFromJSON = getCommandFromJSON();


    /* Switch command*/
    switch (enumCommandFromJSON)
    {
    case COMMAND_UNKNOWN:
        printf("Unknown command\n");
        break;
    case COMMAND_SCAN:
        printf("Command SCAN\n");
        break;
    default:
        printf("Unknown command\n");
        break;
    }


    return EXIT_SUCCESS;
}
