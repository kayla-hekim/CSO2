#include "lab.h"
#include <string.h>
#include <stdio.h>

/* forward function for "attack 0" case.
   change this code to implement your attack
 */
void forward_attack_0(struct message *message) {
    if (message->from == 'A' && message->to == 'B') {
        char* msg_data = (char*)message->data;

        if (strcmp(msg_data, "PAY $1000 TO M") == 0) {
            strcpy(msg_data, "PAY $10001438 TO M");
            message->data_size = strlen("PAY $10001438 TO M\n");
            send_message(message);
            // printf("sent message with 10001438\n");
            return;
        }
        else if (strcmp(msg_data, "PAY $0438 TO M") == 0) {
            // printf("didn't have $1438 -> 10001438\n");
            return;   
        }
    }
    // printf("reached after big if block");
    send_message(message);
}

/* forward function for "attack 1" case.
   change this code to implement your attack
 */
void forward_attack_1(struct message *message) {
    static bool sent = false;

    if (message->from == 'A' && message->to == 'B')  {
        if (!sent) {
            struct message *msg_to_send = new_message('A', 'B', "PAY $10001438 TO M", true, false);
            send_message(msg_to_send);
            sent = true;
        // printf("sent message with 10001438\n");
        }
        return;
    }

    // printf("reached after big if block");
    send_message(message);
}

/* forward function for "attack 2" case.
   change this code to implement your attack
 */
void forward_attack_2(struct message *message) {
    send_message(message);
}

/* forward function for "attack 3" case.
   change this code to implement your attack
 */
void forward_attack_3(struct message *message) {
    send_message(message);
}

/* forward function for "attack 4" case.
   change this code to implement your attack */
void forward_attack_4(struct message *message) {
    send_message(message);
}

/* forward function for "attack 5" case.
   I did not intend this one to be possible. */
void forward_attack_5(struct message *message) {
    send_message(message);
}
