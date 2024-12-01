#include <stdarg.h>
#include <stdio.h>
#include "ctk/io.h"
#include "ctk/types/string.h"

// SETTINGS
#define MAX_VOL 153
const i32 VOL_DELTA = 5;
const Str MAX_VOL_STR = str("153");

// Constants
const i32 INT_BASE = 10;
const Str HELP_MSG =
    str("\n\nNo argument provided!\n\n"
        "Arguments: \n"
        "\n"
        "  --mute       toggles output mute status\n"
        "  --mute-mic   toggles input mute status\n"
        "  --higher     increments the output volume by 5\n"
        "  --lower      decrements the output volume by 5\n");

typedef enum { INCREMENT, DECREMENT } ModifyFlag;

i32 get_volume();
bool get_mute_input_status();
bool get_mute_output_status();
void modify_volume(ModifyFlag flag, bool muted);

int main(i32 argc, c8** argv) {
    if (argc < 2) {
        ctk_printf("%s\n", &HELP_MSG);
        return 1;
    }

    Str flag_str = str_from_cstr(argv[1]);
    bool muted = get_mute_input_status();

    if (str_equals_str(&flag_str, &str("--higher"))) {
        modify_volume(INCREMENT, muted);
    } else if (str_equals_str(&flag_str, &str("--lower"))) {
        modify_volume(DECREMENT, muted);
    } else if (str_equals_str(&flag_str, &str("--mute"))) {
        if (muted) {
            system("pactl set-sink-mute @DEFAULT_SINK@ no");
            command_runf(&str("notify-send -r 9999 \"Volume\" \"%d%%\""), get_volume());
        } else {
            system("pactl set-sink-mute @DEFAULT_SINK@ yes");
            system("notify-send -r 9999 \"Volume\" \"Muted\"");
        }
    } else if (str_equals_str(&flag_str, &str("--mute-mic"))) {
        if (get_mute_output_status()) {
            system("pactl set-source-mute @DEFAULT_SOURCE@ no");
            system("notify-send -r 9999 \"Microphone\" \"Enabled\"");
        } else {
            system("pactl set-source-mute @DEFAULT_SOURCE@ yes");
            system("notify-send -r 9999 \"Microphone\" \"Disabled\"");
        }
    } else {
        ctk_printf("%s\n", &HELP_MSG);
    }

    return 0;
}

i32 get_volume() {
    FILE* mute_buffer = popen("pactl get-sink-volume @DEFAULT_SINK@", "r");
    char character;
    char number[4] = "0\0\0\0";
    usize number_index = 0;
    bool first_slash_found = false;

    while ((character = fgetc(mute_buffer)) && number_index < MAX_VOL_STR.length && character != '%') {
        if (character == '/') {
            first_slash_found = true;
            continue;
        }

        if (!first_slash_found) {
            continue;
        }

        if (character >= '0' && character <= '9') {
            number[number_index] = character;
            number_index++;
        }
    }

    pclose(mute_buffer);

    return strtol(number, null, INT_BASE);
}

bool get_mute_input_status() {
    FILE* mute_buffer = popen("pactl get-sink-mute @DEFAULT_SINK@", "r");
    char character;
    bool muted = false;

    while ((character = fgetc(mute_buffer))) {
        if (character == 'y') {
            muted = true;
            break;
        }

        if (character == 'n') {
            muted = false;
            break;
        }
    }

    pclose(mute_buffer);

    return muted;
}

bool get_mute_output_status() {
    FILE* mute_buffer = popen("pactl get-source-mute @DEFAULT_SOURCE@", "r");
    char character;
    bool muted = false;

    while ((character = fgetc(mute_buffer))) {
        if (character == 'y') {
            muted = true;
            break;
        }

        if (character == 'n') {
            muted = false;
            break;
        }
    }

    pclose(mute_buffer);

    return muted;
}

void modify_volume(ModifyFlag flag, bool muted) {
    i32 volume = get_volume();

    if (muted) {
        command_runf(&str("notify-send -r 9999 \"Volume\" \"%d%%\""), volume);
        return;
    }

    i32 volume_new;

    if (flag == INCREMENT) {
        if (volume + VOL_DELTA > MAX_VOL) {
            volume_new = MAX_VOL;
        } else {
            volume_new = volume + VOL_DELTA;
        }
    } else {
        if (volume - VOL_DELTA < VOL_DELTA) {
            volume_new = 0;
        } else {
            volume_new = volume - VOL_DELTA;
        }
    }

    command_runf(&str("pactl set-sink-volume @DEFAULT_SINK@ %d%%"), volume_new);
    command_runf(&str("notify-send -r 9999 \"Volume\" \"%d%%\""), volume_new);
}
