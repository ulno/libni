#!/usr/bin/env python
# This small python program allows to forward keyboard input to
# a network socket

from enum import Enum

######## Buttons
class Buttons(Enum):
    UP = 16 # ctrl-p (Previous)
    RIGHT = 6 # ctrl-f (Forward)
    DOWN = 14 # ctrl-n (Next)
    LEFT = 2  # ctrl-b (Back)
    ESCAPE = 27 # SPACE
    FIRE = 32 # SPACE
    SPACE = 32 # SPACE
    RETURN = 13 # return
    ENTER = 13 #return/enter

####### constants for configuration
CLIENT_ID =  "\0\0\0\1"
import pygame # import basic pygame modules
from pygame.locals import * # for key-symbols
KEY_MAPPING = {
    K_0: '0',
    K_1: '1',
    K_2: '2',
    K_3: '3',
    K_4: '4',
    K_5: '5',
    K_6: '6',
    K_7: '7',
    K_8: '8',
    K_9: '9',
    K_RETURN: Buttons.RETURN.value,
    K_ESCAPE: Buttons.ESCAPE.value,
    K_SPACE: ' ',
    K_UP: Buttons.UP.value,
    K_w: Buttons.UP.value,
    K_RIGHT: Buttons.RIGHT.value,
    K_d: Buttons.RIGHT.value,
    K_DOWN: Buttons.DOWN.value,
    K_s: Buttons.DOWN.value,
    K_LEFT: Buttons.LEFT.value,
    K_a: Buttons.LEFT.value
}

####### basic constants for program frmo simple types
CONFIG_FILE = "config.yaml"
MAGIC = "LBNI" # magic identifier for libni (Library for Network Input)
PROTOCOL_VERSION = 1
NUMBER_OF_BUTTONS = 256
NUMBER_OF_ANALOGS = 16
NUMBER_OF_BUTTON_BYTES = (NUMBER_OF_BUTTONS + 7) / 8
NUMBER_OF_ANALOGS_BYTES = NUMBER_OF_ANALOGS * 2
MAX_BUFFER_SIZE = 128
BUFFER_HEADER_SIZE=16


######### other IMPORTS
import os.path
import socket
import yaml # for config file
import urlparse
import struct
import random

######## more constants needing imports
SCREENRECT = Rect(0, 0, 320, 320)

class Event(Enum):
    NO = 0           # no load
    BUTTON_UP = 1    # load: 1 byte with button number
    BUTTON_DOWN = 2  # load: 1 byte with button number
    ANALOG = 3       # load: 1 byte with analog number + 2 byte big endian value
    ANALOG_WORD = 4  # load: 2 byte with big endian analog number
    #                        + 4 byte big endian value or 32 big endian floating point
    ANALOG_16 = 5    # load: 4 byte id + 16 byte data

config = yaml.safe_load(open(CONFIG_FILE))

destination_config = urlparse.urlparse(config["destination"])

destination_scheme = destination_config.scheme
destination_host = destination_config.hostname
destination_port = destination_config.port
destination_path = destination_config.path

# create reverse keymap
reverse_keymap={}
for sym in KEY_MAPPING:
    mapping = KEY_MAPPING[sym]
    if mapping in reverse_keymap:
        reverse_keymap[mapping].append(sym)
    else:
        reverse_keymap[mapping] = [sym]

# global variables
main_dir = os.path.split(os.path.abspath(__file__))[0]
finished = False
frame = 0
sock = None
tcp_socket = None

# allocate send-buffer
message = bytearray(MAX_BUFFER_SIZE)
message_filled = BUFFER_HEADER_SIZE

# define buffer header
pos = 0
for c in MAGIC:
    message[pos] = c
    pos += 1
# pos = 4
version = struct.pack(">H",PROTOCOL_VERSION)
for c in version:
    message[pos] = c
    pos += 1
# pos = 8
protocol_type = struct.pack(">H",config["protocol"])
for c in protocol_type:
    message[pos] = c
    pos += 1
# pos = 10
for c in range(4):
    message[pos] = random.randrange(256)
    pos += 1
# pos = 4
if not isinstance(CLIENT_ID, str):
    CLIENT_ID = struct.pack(">L",CLIENT_ID)
else:
    CLIENT_ID = CLIENT_ID[:4] # shorten
    CLIENT_ID += ('\0'*(4-len(CLIENT_ID)))
for c in CLIENT_ID:
    message[pos] = c
    pos += 1


#### send functions for different protocols (schemes)
def send_message_udp():
    global message, message_filled, destination_host, destination_port
#    print "Sending", message_filled, "bytes:",repr(message)
    sock.sendto(message[0:message_filled], (destination_host, destination_port))


def send_message_tcp():
    global message, message_filled, tcp_socket
#    print "Sending", message_filled, "bytes:",repr(message)
    tcp_socket.send(message[0:message_filled])


def send_message_mqtt():
    global message, message_filled
    pass


#### other pygame initialization
# see if we can load more than standard BMP
if not pygame.image.get_extended():
    raise SystemExit("Sorry, extended image module required")


def load_image(file):
    """loads an image, prepares it for play"""
    file = os.path.join(main_dir, 'data', file)
    try:
        surface = pygame.image.load(file)
    except pygame.error:
        raise SystemExit('Could not load image "%s" %s'%(file, pygame.get_error()))
    return surface.convert()


def load_images(*files):
    imgs = []
    for file in files:
        imgs.append(load_image(file))
    return imgs


def add_key_bit_wise(nr, pressed):
    """Set the bit corresponding to a key number to the stae of pressed."""
    if isinstance(nr, str):
        nr = ord(nr)
    byte_nr = nr / 8 + BUFFER_HEADER_SIZE
    bit_nr = nr % 8
    if pressed:
        message[byte_nr] |= (1 << bit_nr)
    else:
        message[byte_nr] &= (255 - (1<<bit_nr))


def add_key_event_wise(nr, pressed):
    global message, message_filled
    if isinstance(nr, str):
        nr = ord(nr)
    if message_filled > MAX_BUFFER_SIZE-3:
        raise "Not enough space in buffer."
    if pressed:
        message[message_filled] = Event.BUTTON_DOWN.value
    else:
        message[message_filled] = Event.BUTTON_UP.value
    message_filled += 1
    message[message_filled] = nr
    message_filled += 1


def read_send_keys():
    global message, reverse_keymap, message_filled, message_filled_reset, keys_adder
    message_filled = message_filled_reset
    keystate = pygame.key.get_pressed()
    for key in reverse_keymap:
        # check if any of them is pressed
        pressed = False
        for sym in reverse_keymap[key]:
            pressed |= keystate[sym] != 0
        keys_adder(key, pressed)
    sender()

# select sender functions based on chosen network protocol (scheme)
if destination_config.scheme == "udp":
    sock = socket.socket(socket.AF_INET,  # Internet
                      socket.SOCK_DGRAM)  # UDP
    sender = send_message_udp
elif destination_config.scheme == "tcp":
    sock = socket.socket(socket.AF_INET,  # Internet
                      socket.SOCK_STREAM)  # TCP
    sock.connect((destination_host, destination_port))
    sender = send_message_tcp
elif destination_config.scheme == "mqtt":
    sender = send_message_mqtt
else:
    raise Exception("Destination scheme %s unknown." % destination_config.scheme)

if config["protocol"] == 0:
    keys_adder = add_key_bit_wise
    message_filled_reset = BUFFER_HEADER_SIZE + NUMBER_OF_BUTTON_BYTES + NUMBER_OF_ANALOGS_BYTES
elif config["protocol"] == 1:
    keys_adder = add_key_event_wise
    message_filled_reset = BUFFER_HEADER_SIZE
else:
    raise Exception("Protocol %s unknown." % config["protocol"])

# main function having the game loop, reading and sending keys
def main(winstyle = 0):
    # Initialize pygame
    pygame.init()
    #if pygame.mixer and not pygame.mixer.get_init():
    #    print ('Warning, no sound')
    #    pygame.mixer = None

    # Set the display mode
    winstyle = 0  # |FULLSCREEN
    bestdepth = pygame.display.mode_ok(SCREENRECT.size, winstyle, 32)
    screen = pygame.display.set_mode(SCREENRECT.size, winstyle, bestdepth)

    #decorate the game window
#    icon = pygame.transform.scale(Alien.images[0], (32, 32))
#    pygame.display.set_icon(icon)
    pygame.display.set_caption('Game Net Controller Desktop Client')
    pygame.mouse.set_visible(0)

    #create the background, tile the bgd image
#    bgdtile = load_image('background.gif')
#    background = pygame.Surface(SCREENRECT.size)
#    for x in range(0, SCREENRECT.width, bgdtile.get_width()):
#        background.blit(bgdtile, (x, 0))
#    screen.blit(background, (0,0))
#    pygame.display.flip()

    global finished, frame
    while not finished:
        frame += 1
        if frame > 20:
            frame = 0
            read_send_keys()
        #get input
        for event in pygame.event.get():
            if event.type == QUIT:
                return
            elif event.type == KEYDOWN:
                read_send_keys()
            elif event.type == KEYUP:
                read_send_keys()

        # clear/erase the last drawn sprites
#        all.clear(screen, background)

        #update all the sprites
 #       all.update()

        #handle player input
        #direction = keystate[K_RIGHT] - keystate[K_LEFT]
        
        #firing = keystate[K_SPACE]
        
        #draw the scene
#        dirty = all.draw(screen)
        dirty = True
        #pygame.display.update(dirty)

        #cap the framerate
        pygame.time.Clock().tick(120)

    sock.close()
    pygame.time.wait(1000)
    pygame.quit()


# call the "main" function if running this script
if __name__ == '__main__': main()

