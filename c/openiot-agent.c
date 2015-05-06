#include "pb.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "sitewhere.h"
#include "sitewhere.pb.h"

#include "proton/message.h"
#include "proton/messenger.h"
#include "pncompat/misc_funcs.inc"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>


#define MODE_NULL 0
#define MODE_REGISTER 1
#define MODE_MEASUREMENT 2
#define MODE_ALERT 3
#define MODE_LOCATION 4

#define check(messenger)                                                     \
  {                                                                          \
    if(pn_messenger_errno(messenger))                                        \
    {                                                                        \
      die(__FILE__, __LINE__, pn_error_text(pn_messenger_error(messenger))); \
    }                                                                        \
  }                                                                          \

void die(const char *file, int line, const char *message)
{
  fprintf(stderr, "%s:%i: %s\n", file, line, message);
  exit(1);
}

void usage(void)
{
  printf("Usage: agent [-o addr] [message] -i [hardwareId] -s [specToken] -r|m|a|i [mode]\n");
  printf("-o     \tThe target address [amqp[s]://domain[/name]]\n");
  printf("-i     \thardwhereId to uniquely identify this device\n");
  printf("-s     \tspecificationToken assigned by device onboarding server\n");
  printf("-r     \tregistration mode, no argument needed\n");
  printf("-m     \tmeasurement mode, with key:value argument,eg: cpu.utilization:0.55\n");
  printf("-a     \talert mode, with [key:value] format argument,eg: egnine.overheat:temperature is above 1000\n");
  printf("-l     \tlocation mode, with [lat:lon:ele] format argument,eg: 23.0000:12.0000:154.0000\n");
  exit(0);
}


/** Unique hardware id for this device */
char* hardwareId = "";
/** Device specification token for hardware configuration */
char* specificationToken = "";
/** Outbound AMQP url*/
char* outboundAddress = "";

static const char * payloadkey = (char *) "payload";
static const int sendTimeout = 2000;

/** Message buffer */
uint8_t buffer[300];

/** Keeps up with whether we have registered */
bool registered = true;

/** Timestamp for last event */
struct timeval lastEvent;
struct timeval now;

/** Send message to AMQP broker */
void send_message(char * address, char * payload, int len) {

  pn_messenger_t * messenger = pn_messenger(NULL);
  pn_messenger_start(messenger);  

  pn_message_t * message = pn_message();
  pn_message_set_address(message, address);

  pn_data_t * data = pn_message_properties(message);
  pn_data_put_map(data);
  pn_data_enter(data);
  pn_data_put_string(data, pn_bytes(strlen(payloadkey), payloadkey));
  pn_data_put_string(data, pn_bytes(len, payload));
  pn_data_exit(data);

  // pn_data_t * body = pn_message_body(message);
  // pn_data_put_string(body, pn_bytes(strlen(payload), payload));
  
  pn_messenger_put(messenger, message);
  
  check(messenger);
  pn_messenger_set_timeout(messenger, sendTimeout);
  pn_messenger_send(messenger, -1);
  check(messenger);

  pn_messenger_stop(messenger);
  pn_messenger_free(messenger);
  pn_message_free(message);
  
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

int main(int argc, char** argv) {
  opterr = 0;
  int c = 0;
  int mode = MODE_NULL;

  // char * measurementString = (char *) malloc(64);
  // char * alertString = (char *) malloc(64);
  // char * locationString = (char *) malloc(64);

  char measurementString[64];
  char alertString[64];
  char locationString[64];
  
  while((c = getopt(argc, argv, "hro:i:s:m:a:l:")) != -1) {
    
    switch(c) {
      case 'h': usage(); break;
      case 'o': outboundAddress = optarg; break;
      case 'i': hardwareId = optarg; break;
      case 's': specificationToken = optarg; break;
      case 'r': 
        mode = MODE_REGISTER;
        break;  
      case 'm': 
        mode = MODE_MEASUREMENT; 
        strcpy(measurementString, optarg);
        break;  
      case 'a': 
        mode = MODE_ALERT;
        strcpy(alertString, optarg);
        break;  
      case 'l': 
        mode = MODE_LOCATION; 
        strcpy(locationString, optarg);
        break;  
      case '?':
        fprintf(stderr, "Wrong option -%c \n", optopt);
        return 1;
  
      default:
        usage();
    }
  }
  
  unsigned int len = 0;
  switch(mode)
  {
    case MODE_REGISTER:      
      if (len = sw_register(hardwareId, specificationToken, buffer, sizeof(buffer), NULL)) {
        send_message(outboundAddress, buffer, len);
        printf("R");
      } else {
        fprintf(stderr, "create register message failed.\n");
      }
      
      break;
      
    case MODE_MEASUREMENT:
      if (len = sw_measurement_multi(hardwareId, str_split(measurementString, ','), 0, buffer, sizeof(buffer), NULL)) {
        send_message(outboundAddress, buffer, len);
        printf(".");
      } else {
        fprintf(stderr, "create measurement message failed.\n");
      }

      
      break;
      
    case MODE_ALERT:      
      if (len = sw_alert(hardwareId, strtok(alertString, ":"), strtok(NULL, ":"), 0, buffer, sizeof(buffer), NULL)) {
        send_message(outboundAddress, buffer, len);
        printf("!");
      } else {
        fprintf(stderr, "create alive message failed.\n");
      }
      
      break;
      
    case MODE_LOCATION:
      if (len = sw_location(hardwareId, strtok(locationString, ":"), strtok(NULL, ":"), strtok(NULL, ":"), 0, buffer, sizeof(buffer), NULL)) {
        send_message(outboundAddress, buffer, len);
        printf("+");
      } else {
        fprintf(stderr, "create location message failed.\n");
      }
      
      break;
      
    default:
      fprintf(stderr, "No operation mode specified.\n");
      usage();
          
  }

}