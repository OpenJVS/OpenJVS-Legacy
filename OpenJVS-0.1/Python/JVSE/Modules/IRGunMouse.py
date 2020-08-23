import pygame
from pygame.locals import *
from JVSE import JVSE
import threading
import time

SCREEN_WIDTH = 1280
SCREEN_HEIGHT = 800

class App:
    def __init__(self):
        self._running = True
        self._display_surf = None
        self.size = self.weight, self.height = SCREEN_WIDTH, SCREEN_HEIGHT
        self.emulator = JVSE()

    def on_init(self):
        pygame.init()
        self._display_surf = pygame.display.set_mode(self.size, pygame.HWSURFACE | pygame.DOUBLEBUF)
        self._running = True
        self.emulator.start()

    def on_event(self, event):
        if event.type == pygame.QUIT:
            self._running = False

        if event.type == pygame.MOUSEBUTTONDOWN:
            pos = pygame.mouse.get_pos()
            print "SENT MOUSE", pos[0], ":", pos[1]

            self.emulator.set_pos(int((float(pos[0]) / float(SCREEN_WIDTH)) * 255), int((float(pos[1]) / float(SCREEN_HEIGHT)) * 255))
            self.emulator.set_key_player1(7, 0)
            self.emulator.set_key_player1(6, 1)

        if event.type == pygame.MOUSEBUTTONUP:
            self.emulator.set_pos(0, 0)
            self.emulator.set_key_player1(6, 0)
            self.emulator.set_key_player1(7, 1)

        if event.type == pygame.KEYDOWN or event.type == pygame.KEYUP:
            if event.key == pygame.K_m and event.type == pygame.KEYDOWN:
                self.emulator.increment_coin()

            set_type = event.type == pygame.KEYDOWN

            # Special Keys
            if event.key == pygame.K_a:
                self.emulator.set_pos(255, 255)
                self.emulator.set_key_player1(6, 1)
            if event.key == pygame.K_s:
                self.emulator.set_pos(0, 0)
                self.emulator.set_key_player1(6, 1)
            if event.key == pygame.K_d:
                self.emulator.set_key_player1(6, 0)

            # Player Keys
            if event.key == pygame.K_t:
                self.emulator.set_key_player1(8, set_type)
            if event.key == pygame.K_q:
                self.emulator.set_key_player1(0, set_type)
            if event.key == pygame.K_e:
                self.emulator.set_key_player1(6, set_type)
            if event.key == pygame.K_r:
                self.emulator.set_key_player1(7, set_type)
            if event.key == pygame.K_w:
                self.emulator.set_key_player1(1, set_type)
            if event.key == pygame.K_y:
                self.emulator.set_key_player1(9, set_type)
            if event.key == pygame.K_u:
                self.emulator.set_key_player1(10, set_type)
            if event.key == pygame.K_i:
                self.emulator.set_key_player1(11, set_type)
            if event.key == pygame.K_o:
                self.emulator.set_key_player1(12, set_type)
            if event.key == pygame.K_p:
                self.emulator.set_key_player1(13, set_type)
            if event.key == pygame.K_l:
                self.emulator.set_key_player1(14, set_type)
            if event.key == pygame.K_k:
                self.emulator.set_key_player1(15, set_type)

            # Arrow Keys
            if event.key == pygame.K_UP:
                self.emulator.set_key_player1(2, set_type)
            if event.key == pygame.K_DOWN:
                self.emulator.set_key_player1(3, set_type)
            if event.key == pygame.K_LEFT:
                self.emulator.set_key_player1(4, set_type)
            if event.key == pygame.K_RIGHT:
                self.emulator.set_key_player1(5, set_type)

            # System Keys
            if event.key == pygame.K_1:
                self.emulator.set_key_system(0, set_type)
            if event.key == pygame.K_2:
                self.emulator.set_key_system(1, set_type)
            if event.key == pygame.K_3:
                self.emulator.set_key_system(2, set_type)
            if event.key == pygame.K_4:
                self.emulator.set_key_system(3, set_type)
            if event.key == pygame.K_5:
                self.emulator.set_key_system(4, set_type)
            if event.key == pygame.K_6:
                self.emulator.set_key_system(5, set_type)
            if event.key == pygame.K_7:
                self.emulator.set_key_system(6, set_type)
            if event.key == pygame.K_8:
                self.emulator.set_key_system(7, set_type)


    def on_loop(self):
        pass
    def on_render(self):
        pass
    def on_cleanup(self):
        self.emulator.stop()
        pygame.quit()


    def on_execute(self):
        if self.on_init() == False:
            self._running = False

        while( self._running ):
            time.sleep(0.01)
            for event in pygame.event.get():
                self.on_event(event)
            self.on_loop()
            self.on_render()
        self.on_cleanup()

if __name__ == "__main__" :
    theApp = App()
    theApp.on_execute()
