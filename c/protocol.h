/** Send message to message broker */
void send_message(char * address, char * payload, int len);

/** Receive message from message broker */
void recv_message(char * address, char * payload, int len);