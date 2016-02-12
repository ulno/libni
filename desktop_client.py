#!/usr/bin/env python
# This small python program allows to forward keyboard input to
# a udp socket

# constants
#DESTINATION_HOST = "192.168.15.167"; # TODO: move to parameters
#DESTINATION_HOST = "192.168.43.1"; # TODO: move to parameters
DESTINATION_HOST = "ulno-work"; # TODO: move to parameters
# DESTINATION_HOST = "localhost";
# DESTINATION_HOST = "192.168.15.194";
DESTINATION_PORT = 19877;

NUMBER_OF_BUTTONS = 256;
allocated_buttons = 0;
NUMBER_OF_ANALOG = 16;
allocated_analogs = 0;

NO_EVENT = 0
BUTTON_UP_EVENT = 1
BUTTON_DOWN_EVENT = 2
ANALOG_EVENT = 3

import os.path
import socket
sock = socket.socket(socket.AF_INET, # Internet
                      socket.SOCK_DGRAM) # UDP

# import basic pygame modules
import pygame
from pygame.locals import *

# more constants
SCREENRECT = Rect(0, 0, 320, 320)
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
    K_RETURN: 13,
    K_ESCAPE: 27,
    K_SPACE: ' ',
    K_UP: 'w',
    K_w: 'w',
    K_RIGHT: 'd',
    K_d: 'd',
    K_DOWN: 's',
    K_s: 's',
    K_LEFT: 'a',
    K_a: 'a'
}

# create reverse keymap
reverse_keymap={}
for sym in KEY_MAPPING:
    mapping = KEY_MAPPING[sym]
    if mapping in reverse_keymap:
        reverse_keymap[mapping].append(sym)
    else:
        reverse_keymap[mapping] = [sym]

# global variables
finished = False
frame = 0

# allocate send-buffer
MAX_BUFFER_SIZE = 128
message = bytearray(MAX_BUFFER_SIZE)
message_filled = 0

# see if we can load more than standard BMP
if not pygame.image.get_extended():
    raise SystemExit("Sorry, extended image module required")

main_dir = os.path.split(os.path.abspath(__file__))[0]

def load_image(file):
    "loads an image, prepares it for play"
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


def send_message():
    global message
#    print "Sending", message_filled, "bytes:",repr(message)
    sock.sendto(message[0:message_filled], (DESTINATION_HOST, DESTINATION_PORT))

def add_key_to_message(nr, pressed):
    global message, message_filled
    if isinstance(nr, str):
        nr = ord(nr)
    if message_filled > MAX_BUFFER_SIZE-3:
        raise "Not enough space in buffer."
    if pressed:
        message[message_filled] = BUTTON_DOWN_EVENT
    else:
        message[message_filled] = BUTTON_UP_EVENT
    message_filled += 1
    message[message_filled] = nr
    message_filled += 1

def read_send_keys():
    global message, reverse_keymap, message_filled
    message_filled = 0
    keystate = pygame.key.get_pressed()
    for key in reverse_keymap:
        # check if any of tehm is pressed
        pressed = False
        for sym in reverse_keymap[key]:
            pressed |= keystate[sym] != 0
        add_key_to_message(key, pressed)
    send_message()


def main(winstyle = 0):
    # Initialize pygame
    pygame.init()
    if pygame.mixer and not pygame.mixer.get_init():
        print ('Warning, no sound')
        pygame.mixer = None

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
                read_send_keys();
            elif event.type == KEYUP:
                read_send_keys();

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



#call the "main" function if running this script
if __name__ == '__main__': main()

