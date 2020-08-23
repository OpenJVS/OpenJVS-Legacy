import pygame
from pygame.locals import *
from JVSE import JVSE
import threading
import time

class App:
    def __init__(self):
        self._running = True
        self._display_surf = None
        self.size = self.weight, self.height = 1280, 800
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

            self.emulator.set_pos(int((float(pos[0]) / float(1280)) * 255), int((float(pos[1]) / float(800)) * 255))
            self.emulator.set_key_player1(6, 1)

        if event.type == pygame.MOUSEBUTTONUP:
            self.emulator.set_pos(255, 255)
            self.emulator.set_key_player1(6, 0)


        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_SPACE:
                self.emulator.set_pos(255, 255)
                self.emulator.set_key_player1(6, 1)
                time.sleep(0.1)
                self.emulator.set_key_player1(6, 0)

            if event.key == pygame.K_x:
                self.emulator.set_key_player1(0, 1)
            if event.key == pygame.K_z:
                self.emulator.set_key_player1(6, 1)
            if event.key == pygame.K_w:
                self.emulator.set_key_player1(7, 1)
            if event.key == pygame.K_q:
                self.emulator.set_key_player1(8, 1)
            if event.key == pygame.K_c:
                self.emulator.set_key_player1(1, 1)
            if event.key == pygame.K_RIGHT:
                self.emulator.set_key_player1(5, 1)
            if event.key == pygame.K_UP:
                self.emulator.set_key_player1(2, 1)
            if event.key == pygame.K_DOWN:
                self.emulator.set_key_player1(3, 1)
            if event.key == pygame.K_LEFT:
                self.emulator.set_key_player1(4, 1)


            if event.key == pygame.K_v:
                self.emulator.increment_coin()

        if event.type == pygame.KEYUP:
            if event.key == pygame.K_q:
                self.emulator.set_key_player1(8, 0)
            if event.key == pygame.K_x:
                self.emulator.set_key_player1(0, 0)
            if event.key == pygame.K_z:
                self.emulator.set_key_player1(6, 0)
            if event.key == pygame.K_w:
                self.emulator.set_key_player1(7, 0)
            if event.key == pygame.K_c:
                self.emulator.set_key_player1(1, 0)
            if event.key == pygame.K_RIGHT:
                self.emulator.set_key_player1(5, 0)
            if event.key == pygame.K_UP:
                self.emulator.set_key_player1(2, 0)
            if event.key == pygame.K_DOWN:
                self.emulator.set_key_player1(3, 0)
            if event.key == pygame.K_LEFT:
                self.emulator.set_key_player1(4, 0)



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
