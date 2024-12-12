#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>

const int BUFFER_SIZE = 1024;

volatile int current_bit = 0;
volatile int is_waiting_for_info = 1;
volatile int is_waiting_for_response = 1;

void signal_reader_handler(int signo) {
    is_waiting_for_response = 0;
}

void signal_writer_handler(int signo) {
    if (signo == SIGUSR1) {
        current_bit = 0;
    } else if (signo == SIGUSR2) {
        current_bit = 1;
    }
    is_waiting_for_info = 0;
}

void reader_process(pid_t writer_pid) {
    sigset_t signal_mask, old_signal_mask;
    sigemptyset(&signal_mask);
    sigaddset(&signal_mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &signal_mask, &old_signal_mask);

    struct sigaction action;
    action.sa_flags = 0;
    action.sa_handler = signal_reader_handler;
    action.sa_mask = signal_mask;
    sigaction(SIGUSR1, &action, NULL);

    char buffer[BUFFER_SIZE];
    int bytes_read;

    while ((bytes_read = read(0, buffer, BUFFER_SIZE)) > 0) {
        for (int i = 0; i < bytes_read; ++i) {
            for (int j = 0; j < 8; ++j) {
                current_bit = (1 << j) & buffer[i];
                int signal_to_send = (current_bit == 0) ? SIGUSR1 : SIGUSR2;
                kill(writer_pid, signal_to_send);
                
                is_waiting_for_response = 1;
                while (is_waiting_for_response) {
                    sigsuspend(&old_signal_mask);
                }
            }
        }
    }
}

void writer_process(pid_t reader_pid) {
    sigset_t signal_mask, old_signal_mask;
    sigemptyset(&signal_mask);
    sigaddset(&signal_mask, SIGUSR1);
    sigaddset(&signal_mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &signal_mask, &old_signal_mask);

    struct sigaction action;
    action.sa_flags = 0;
    action.sa_handler = signal_writer_handler;
    action.sa_mask = signal_mask;
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGUSR2, &action, NULL);

    char byte_to_write = 1;
    while (byte_to_write) {
        byte_to_write = 0;
        for (int i = 0; i < 8; i++) {
            is_waiting_for_info = 1;
            while (is_waiting_for_info) {
                sigsuspend(&old_signal_mask);
            }
            byte_to_write |= (current_bit << i);
            kill(reader_pid, SIGUSR1);
        }
        write(1, &byte_to_write, 1);
    }
}

int main() {
    pid_t parent_pid = getpid();
    pid_t child_pid = fork();
    
    if (child_pid == 0) {
        reader_process(parent_pid);
        return 0;
    }
    
    writer_process(child_pid);
    wait(NULL);
    
    return 0;
}