#include <stdio.h>
#include "netsim.h"

char request_num; // store data[1][0] in main to then pass into callback_function if needed
int last_seq = 0;
int timeout_id = -1;
int finished = 0;


void callback_function(void *arg) {
    if (finished) return;

    char msg[5]; 

    if (last_seq == 0) {
        msg[1] = 'G';
        msg[2] = 'E';
        msg[3] = 'T';
        msg[4] = request_num;
    }
    else {
        msg[1] = 'A';
        msg[2] = 'C';
        msg[3] = 'K';
        msg[4] = last_seq;
    }       
    msg[0] = msg[1] ^ msg[2] ^ msg[3] ^ msg[4];
    send(5, msg);
    // request_num = msg[4];
    if (!finished) {
        timeout_id = setTimeout(callback_function, 1000, NULL);
    }

}


void recvd(size_t len, void* _data) {
    // FIX ME -- add proper handling of messages (BELOW)
    char *data = _data;

    char check = 0;
    for (size_t i = 1; i < len; i++) {
        check ^= data[i];
    }
    if (check != data[0]) {
        return;
    }

    clearTimeout(timeout_id);

    if (data[1] == last_seq + 1) {
        fwrite(data + 3, 1, len - 3, stdout);
        fflush(stdout);
        last_seq = data[1];
        if (data[1] == data[2]) {
            finished = 1;
        }
    }
    
    char ack[5];
    ack[1] = 'A'; ack[2] = 'C'; ack[3] = 'K'; ack[4] = data[1];
    ack[0] = ack[1] ^ ack[2] ^ ack[3] ^ ack[4];
    send(5, ack);

    if (!finished) {
        timeout_id = setTimeout(callback_function, 1000, NULL);
    }
}


int main(int argc, char *argv[]) {
    // this code should work without modification
    if (argc != 2) {
        fprintf(stderr, "USAGE: %s n\n    where n is a number between 0 and 3\n", argv[0]);
        return 1;
    }
    char data[5];
    data[1] = 'G'; data[2] = 'E'; data[3] = 'T'; data[4] = argv[1][0];
    // end of working code
    
    
    // data[0] = 102; // FIX ME -- add proper checksum computation (BELOW)
    data[0] = data[1] ^ data[2] ^ data[3] ^ data[4];
    send(5, data);
    // FIX ME -- add action if no reply
    request_num = data[4];
    timeout_id = setTimeout(callback_function, 1000, NULL);

    waitForAllTimeoutsAndMessagesThenExit();
}
