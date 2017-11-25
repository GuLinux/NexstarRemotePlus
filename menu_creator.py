#!/usr/bin/env python3

entries = list()
entries_definitions = {}
actions = {}


def add_entry(uid, label, on_shortpress, on_longpress):
    global entries
    entries_definitions[uid] = '0x{:02X}'.format(len(entries))
    entry = {'uid': uid, 'label': label, 'on_shortpress': on_shortpress, 'on_longpress': on_longpress}
    entries.append(entry)

def find_actions():
    global entries
    for entry in entries:
        on_shortpress = entry['on_shortpress']
        on_longpress = entry['on_longpress']
        if on_shortpress not in entries_definitions:
            actions[on_shortpress] = 'ACTIONS_BASE + 0x{:02X}'.format(len(actions))
        if on_longpress not in entries_definitions:
            actions[on_longpress] = 'ACTIONS_BASE + 0x{:02X}'.format(len(actions))

def entry2menu(entry):
    return '    {{ {uid}, "{label}", {on_shortpress}, {on_longpress}, }},'.format(**entry)

add_entry('MENU_ROOT', '', 'MENU_GPS', 'MENU_GPS')
add_entry('MENU_GPS', 'GPS', 'MENU_NEXSTAR', 'MENU_GPS_WAKEUP')
add_entry('MENU_GPS_WAKEUP', 'GPS Wakeup', 'MENU_GPS_TIMEOUT', 'ACTION_GPS_WAKEUP')

add_entry('MENU_GPS_TIMEOUT', 'GPS Timeout', 'MENU_GPS_WAKEUP', 'MENU_GPS_TIMEOUT_INFINITE')
add_entry('MENU_GPS_TIMEOUT_5M', 'GPS Timeout:\\n5 minutes', 'MENU_GPS_TIMEOUT_10M', 'ACTION_SET_GPS_TIMEOUT_5M')
add_entry('MENU_GPS_TIMEOUT_10M', 'GPS Timeout:\\n10 minutes', 'MENU_GPS_TIMEOUT_20M', 'ACTION_SET_GPS_TIMEOUT_10M')
add_entry('MENU_GPS_TIMEOUT_20M', 'GPS Timeout:\\n20 minutes', 'MENU_GPS_TIMEOUT_40M', 'ACTION_SET_GPS_TIMEOUT_20M')
add_entry('MENU_GPS_TIMEOUT_40M', 'GPS Timeout:\\n40 minutes', 'MENU_GPS_TIMEOUT_INFINITE', 'ACTION_SET_GPS_TIMEOUT_40M')
add_entry('MENU_GPS_TIMEOUT_INFINITE', 'GPS Timeout:\\nNo timeout', 'MENU_GPS_TIMEOUT_5M', 'ACTION_SET_GPS_TIMEOUT_INFINITE')

add_entry('MENU_NEXSTAR', 'Nexstar', 'MENU_DATE_TIME', 'MENU_NEXSTAR_SYNC')
add_entry('MENU_NEXSTAR_SYNC', 'Nexstar Sync', 'MENU_NEXSTAR_SYNC', 'ACTION_NEXSTAR_SYNC')

add_entry('MENU_DATE_TIME', 'Date/Time', 'MENU_GPS', 'MENU_SET_TIMEZONE')

add_entry('MENU_SET_TIMEZONE', 'Set Timezone', 'MENU_SET_DAYLIGHT_SAVING', 'MENU_SET_TIMEZONE_GMT_P00')

add_entry('MENU_SET_DAYLIGHT_SAVING', 'Set Daylight Saving', 'MENU_SET_TIMEZONE', 'MENU_SET_DAYLIGHT_SAVING_OFF')
add_entry('MENU_SET_DAYLIGHT_SAVING_OFF', 'Daylight Saving:\\nOFF', 'MENU_SET_DAYLIGHT_SAVING_ON', 'ACTION_SET_DAYLIGHT_SAVING_OFF')
add_entry('MENU_SET_DAYLIGHT_SAVING_ON', 'Daylight Saving:\\nON', 'MENU_SET_DAYLIGHT_SAVING_OFF', 'ACTION_SET_DAYLIGHT_SAVING_ON')

for timezone in range(-12, +13):
    sign_def = 'P' if timezone >= 0 else 'M'
    sign = '+' if timezone >= 0 else '-'
    label = 'Set Timezone:\\nGMT{sign}{timezone:02d}'.format(sign=sign, timezone=abs(timezone)) 
    uid = 'MENU_SET_TIMEZONE_GMT_{sign}{timezone:02d}'.format(sign=sign_def, timezone=abs(timezone))
    action = 'ACTION_SET_TIMEZONE_GMT_{sign}{timezone:02d}'.format(sign=sign_def, timezone=abs(timezone))

    next_item_sign, next_item_tz = sign_def, timezone+1
    if next_item_tz == 0:
        next_item_sign = 'P'

    if next_item_tz == 13:
        next_item_sign = 'M'
        next_item_tz = -12

    next_item = 'MENU_SET_TIMEZONE_GMT_{sign}{timezone:02d}'.format(sign=next_item_sign, timezone=abs(next_item_tz))

    add_entry(uid, label, next_item, action)

find_actions()

def write_definitions(f):
    for key, value in entries_definitions.items():
        f.write('#define {} {}\n'.format(key, value))
    f.write('\n')
    for key, value in actions.items():
        f.write('#define {} {}\n'.format(key, value))
    f.write('\n')
 
def write_menu():
    global entries
    with open('menu.h', 'w') as f:
        f.write('''#pragma once

struct MenuEntry {
  uint8_t id;
  String label;
  uint8_t on_shortpress;
  uint8_t on_longpress;
};

#define ACTIONS_BASE 0xA0

''')
        write_definitions(f)    
        f.write('''static MenuEntry entries[] {
''')
        f.write('\n'.join([entry2menu(x) for x in entries]))
        f.write('\n};')

write_menu()
