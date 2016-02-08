#!/usr/bin/env python
# This small python program allows ot forward keyboard input to
# a mqtt game controller

import os.path

#import basic pygame modules
import pygame
from pygame.locals import *

finished = False

#see if we can load more than standard BMP
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
    pygame.display.set_caption('MQTT Game Controller Desktop Client')
    pygame.mouse.set_visible(0)

    #create the background, tile the bgd image
#    bgdtile = load_image('background.gif')
#    background = pygame.Surface(SCREENRECT.size)
#    for x in range(0, SCREENRECT.width, bgdtile.get_width()):
#        background.blit(bgdtile, (x, 0))
#    screen.blit(background, (0,0))
#    pygame.display.flip()


    global finished
    while not finished:

        #get input
        for event in pygame.event.get():
            if event.type == QUIT:
                return
            elif event.type == KEYDOWN:
                
            elif event.type == KEYUP:
        keystate = pygame.key.get_pressed()
        print keystate

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
        pygame.display.update(dirty)

        #cap the framerate
        clock.tick(40)

    pygame.time.wait(1000)
    pygame.quit()



#call the "main" function if running this script
if __name__ == '__main__': main()

