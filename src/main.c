#include "ctk/io.h"

// Settings
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

// Commands
const CStr CMD_GET_VOLUME = cstr("pactl get-sink-volume @DEFAULT_SINK@");
const CStr CMD_GET_MUTE_INPUT = cstr("pactl get-sink-mute @DEFAULT_SINK@");
const CStr CMD_GET_MUTE_OUTPUT = cstr("pactl get-source-mute @DEFAULT_SOURCE@");
const CStr CMD_SET_MUTE_INPUT_OFF = cstr("pactl set-sink-mute @DEFAULT_SINK@ no");
const CStr CMD_SET_MUTE_INPUT_ON = cstr("pactl set-sink-mute @DEFAULT_SINK@ yes");
const CStr CMD_SET_MUTE_OUTPUT_OFF = cstr("pactl set-source-mute @DEFAULT_SOURCE@ no");
const CStr CMD_SET_MUTE_OUTPUT_ON = cstr("pactl set-source-mute @DEFAULT_SOURCE@ yes");
const CStr CMD_NOTIFY_MUTED = cstr("notify-send -r 9999 \"Volume\" \"Muted\"");
const CStr CMD_NOTIFY_MIC_ON = cstr("notify-send -r 9999 \"Microphone\" \"Enabled\"");
const CStr CMD_NOTIFY_MIC_OFF = cstr("notify-send -r 9999 \"Microphone\" \"Disabled\"");
const Str CMD_FMT_NOTIFY_VOLUME = str("notify-send -r 9999 \"Volume\" \"%d%%\"");
const Str CMD_FMT_SET_VOLUME = str("pactl set-sink-volume @DEFAULT_SINK@ %d%%");

typedef enum { INCREMENT, DECREMENT, MUTE_OUTPUT, MUTE_INPUT } ModifyFlag;

i32 get_volume();
bool get_mute_status(ModifyFlag flag);
void modify_volume(ModifyFlag flag, bool muted);

int main(i32 argc, c8** argv) {
    if (argc < 2) {
        print(&str("%s\n"), &HELP_MSG);
        return 1;
    }

    Str flag_str = str_from_cstr(&cstr(argv[1]));

    if (str_equals_str(&flag_str, &str("--higher"))) {
        modify_volume(INCREMENT, get_mute_status(MUTE_INPUT));
    } else if (str_equals_str(&flag_str, &str("--lower"))) {
        modify_volume(DECREMENT, get_mute_status(MUTE_INPUT));
    } else if (str_equals_str(&flag_str, &str("--mute"))) {
        if (get_mute_status(MUTE_INPUT)) {
            command_run(&CMD_SET_MUTE_INPUT_OFF);
            command_runf(&CMD_FMT_NOTIFY_VOLUME, get_volume());
        } else {
            command_run(&CMD_SET_MUTE_INPUT_ON);
            command_run(&CMD_NOTIFY_MUTED);
        }
    } else if (str_equals_str(&flag_str, &str("--mute-mic"))) {
        if (get_mute_status(MUTE_OUTPUT)) {
            command_run(&CMD_SET_MUTE_OUTPUT_OFF);
            command_run(&CMD_NOTIFY_MIC_ON);
        } else {
            command_run(&CMD_SET_MUTE_OUTPUT_ON);
            command_run(&CMD_NOTIFY_MIC_OFF);
        }
    } else {
        print(&str("%s\n"), &HELP_MSG);
    }

    return 0;
}

i32 get_volume() {
    Process* process = process_start(PROCESS_READ, &CMD_GET_VOLUME);
    char character;
    char number[4] = "0\0\0\0";
    usize number_index = 0;
    bool first_slash_found = false;

    while ((character = fgetc(process)) && number_index < MAX_VOL_STR.length && character != '%') {
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

    process_end(process);
    return strtol(number, null, INT_BASE);
}

bool get_mute_status(ModifyFlag flag) {
    Process* process;
    char character;
    bool muted = false;

    if (flag == MUTE_INPUT) {
        process = process_start(PROCESS_READ, &CMD_GET_MUTE_INPUT);
    } else {
        process = process_start(PROCESS_READ, &CMD_GET_MUTE_OUTPUT);
    }

    while ((character = fgetc(process))) {
        if (character == 'y') {
            muted = true;
            break;
        }

        if (character == 'n') {
            muted = false;
            break;
        }
    }

    process_end(process);
    return muted;
}

void modify_volume(ModifyFlag flag, bool muted) {
    i32 volume = get_volume();

    if (muted) {
        command_runf(&CMD_FMT_NOTIFY_VOLUME, volume);
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

    command_runf(&CMD_FMT_SET_VOLUME, volume_new);
    command_runf(&CMD_FMT_NOTIFY_VOLUME, volume_new);
}
