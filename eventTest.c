#if 1
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>

// JS_EVENT_BUTTON     button pressed/released 
// JS_EVENT_AXIS       joystick moved          
// JS_EVENT_INIT       initial state of device 

//    D-UP = event.type 2, event.number 7, event.value -32767
//  D-DOWN = event.type 2, event.number 7, event.value  32767
//  D-LEFT = event.type 2, event.number 6, event.value -32767
// D-RIGHT = event.type 2, event.number 6, event.value  32767


int main(void) {
    int joystick_fd = open("/dev/input/js0", O_RDONLY);
    if (joystick_fd == -1) {
        perror("Error opening joystick device");
        return EXIT_FAILURE;
    }

    struct js_event event;
    while (read(joystick_fd, &event, sizeof(event)) > 0) {
		if (event.type == JS_EVENT_AXIS){
            printf("Type: JS_EVENT_AXIS, Number: %d, Value: %d\n", event.number, event.value);
		}
		if (event.type == JS_EVENT_BUTTON) {
            printf("Type: JS_EVENT_BUTTON, Number: %d, Value: %d\n", event.number, event.value);
		}
    }
    close(joystick_fd);
}
#endif

#if 0

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>

// JS_EVENT_BUTTON     button pressed/released 
// JS_EVENT_AXIS       joystick moved          
// JS_EVENT_INIT       initial state of device 

//    D-UP = event.type 2, event.number 7, event.value -32767
//  D-DOWN = event.type 2, event.number 7, event.value  32767
//  D-LEFT = event.type 2, event.number 6, event.value -32767
// D-RIGHT = event.type 2, event.number 6, event.value  32767

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

        switch (event.type) {
            case JS_EVENT_AXIS:
                printf("Type: JS_EVENT_AXIS, Number: %d, Value: %d\n", event.number, event.value);
                break;
            case JS_EVENT_BUTTON:
                printf("Type: JS_EVENT_BUTTON, Number: %d, Value: %d\n", event.number, event.value);
                break;
            default:
                // Unknown event type
         //       fprintf(stderr, "Unknown event type: %d\n", event.type);
                break;
        }
    }

    if (close(joystick_fd) == -1) {
        perror("Error closing joystick device");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
#endif
