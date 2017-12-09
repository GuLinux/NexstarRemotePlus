#!/usr/bin/env python3
import sys
import json

class MenuItem:
  def __init__(self, descriptor, parent=None):
    self.uid = descriptor['uid']
    self.label = descriptor['label']
    self.children = []
    self.action = descriptor.get('on_enter')
    self.next = None
    self.previous = None
    self.parent = parent
    if 'children' in descriptor:
      last_item = None
      for item in descriptor['children']:
        new_item = MenuItem(item, self)
        if last_item:
          new_item.previous = last_item.uid
          last_item.next = new_item.uid
        self.children.append(new_item)
        last_item = new_item
      self.children[0].previous = self.children[-1].uid
      self.children[-1].next = self.children[0].uid

  def cpp_initialization(self):
    format_params = { 'uid': self.uid, 'label': self.label, 'on_next': self.next, 'on_back': self.previous, 'on_enter': self.action, 'parent': '0' }
    if not self.action and self.children:
      format_params['on_enter'] = self.children[0].uid
    if self.parent:
      format_params['parent'] = self.parent.uid
    return '    {{ {uid}, "{label}", {on_next}, {on_enter}, {on_back}, {parent}}},'.format(**format_params)

  def generate_cpp(self):
    cpp_code = [self.cpp_initialization()]
    for item in self.children:
      cpp_code.extend(item.generate_cpp())
    return cpp_code

  def __str__(self):
    s = '{} ({}) <<{}  >>{}'.format(self.label, self.uid, self.previous, self.next)
    if self.action:
      return s + ' {}'.format(self.action)
    return s + '\n{}'.format('\n'.join([str(x) for x in self.children]))

  def __repr__(self):
    return self.__str__()

  def collect_uids(self):
    uids = [self.uid]
    if self.children:
      for item in self.children:
        uids.extend(item.collect_uids())
    return uids

  def collect_actions(self):
    actions = []
    if self.action:
      actions.append(self.action)
    if self.children:
      for item in self.children:
        actions.extend(item.collect_actions())
    return actions


def write_definitions(f, menu):
    for index, uid in enumerate(menu.collect_uids()):
      f.write('#define {} 0x{:02x}\n'.format(uid, index))

    f.write('\n')

    for index, action in enumerate(menu.collect_actions()):
      f.write('#define {} 0x{:02x} + ACTIONS_BASE\n'.format(action, index))

    f.write('\n')
 
def write_menu(f, menu):
    f.write('''#pragma once

struct MenuEntry {
  uint8_t id;
  const char *label;
  uint8_t on_next;
  uint8_t on_enter;
  uint8_t on_back;
  uint8_t parent;
};

#define ACTIONS_BASE 0xA0

''')
    write_definitions(f, menu)    
    f.write('''static MenuEntry entries[] {
''')
    entries = menu.generate_cpp()
    f.write('\n'.join(entries))
    f.write('\n};\n')
    f.write('static uint16_t len_entries = {};\n'.format(len(entries)))

descriptor_file = open(sys.argv[1], 'r')
descriptor = json.load(descriptor_file)
root = MenuItem(descriptor)
root.next = root.children[0].uid
root.previous= root.children[-1].uid

write_menu(sys.stdout, root)
