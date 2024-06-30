#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>

int main(void) {
    int joystick_fd = open("/dev/input/js0", O_RDONLY);
    if (joystick_fd == -1) {
        perror("Error opening joystick device");
        return EXIT_FAILURE;
    }

    struct js_event event;
    while (1) {
        ssize_t bytes = read(joystick_fd, &event, sizeof(event));
        if (bytes == -1) {
            perror("Error reading joystick event");
            close(joystick_fd);
            return EXIT_FAILURE;
        } else if (bytes != sizeof(event)) {
            fprintf(stderr, "Unexpected bytes read: %ld\n", bytes);
            close(joystick_fd);
            return EXIT_FAILURE;
        }

        //Displays only active inputs
        if(event.value != 0 && event.type & ~ JS_EVENT_INIT){
            switch (event.type) {
                case JS_EVENT_AXIS:
                    printf("event.type: JS_EVENT_AXIS, event.number: %d, event.value: %d\n", event.number, event.value);
                    break;
                case JS_EVENT_BUTTON:
                    printf("event.type: JS_EVENT_BUTTON, event.number: %d, event.value: %d\n", event.number, event.value);
                    break;
            }
        }
    }

    if (close(joystick_fd) == -1) {
        perror("Error closing joystick device");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
