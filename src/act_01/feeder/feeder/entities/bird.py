import math
import random

import pygame

from feeder.entities.gameobject import GameObject
from feeder.assets.assetmanager import AssetManager
from feeder.config import cfg_item
from feeder.events import Events

class Bird(GameObject):

    def __init__(self, name, spline):
        super().__init__()
        self.__name = name

        speed_range = cfg_item("entities", "speed")
        self.__speed = random.uniform(*speed_range)

        self.__init_spline(spline)

        self.__row = cfg_item("entities", "birds", self.__name, "row")

        _, rect = AssetManager.instance().get(cfg_item("entities", "name"), self.__row, 0)
        self.rect = rect.copy()
        self._render_rect = rect.copy()
        self.__sequence = 0
        self.__max_sequence = 2
        self.__current_sequence_time = 0
        self.__time_per_sequence = cfg_item("entities", "time_per_sequence")

        self._center(self.__position)

    def handle_input(self, key, is_pressed):
        pass

    def process_events(self, event):
        pass

    def update(self, delta_time):
        self.__current_sequence_time += delta_time
        if self.__current_sequence_time >= self.__time_per_sequence:
            self.__current_sequence_time -= self.__time_per_sequence
            self.__sequence = (self.__sequence + 1) % self.__max_sequence
        self.__move_spline(delta_time)

    def render(self, surface_dst):
        image, _ = AssetManager.instance().get(self.__name + str(self.__sequence))
        image = pygame.transform.rotate(image, -self.__orientation - 90)
        self._render_rect = image.get_rect(center = self.__position.xy)
        self.rect = self._render_rect.copy()

        surface_dst.blit(image, self._render_rect)

    def __init_spline(self, spline):
        self.__spline = spline
        time = self.__spline.est_dist / self.__speed #time to cover spline at entity regular speed
        self.__spline_msec_ratio = 1 / time #incremental pos in spline per msec
        self.__current_spline_point = 0.0 #normalized [0,1] pos in spline

        self.__position = (self.__spline.get_point(self.__current_spline_point))

    def __move_spline(self, delta_time):
        self.__current_spline_point += self.__spline_msec_ratio * delta_time

        if self.__current_spline_point >= 1.0:
            end_event = pygame.event.Event(pygame.USEREVENT, event = Events.BIRD_END_POINT, bird = self)
            pygame.event.post(end_event)
        else:
            old_pos = self.__position
            self.__position = self.__spline.get_point(self.__current_spline_point)
            self.__velocity = self.__position - old_pos
            self.__set_orientation()

            self._center(self.__position)

    def __set_orientation(self):
        if self.__velocity.length_squared() > 0:
            self.__orientation = math.degrees(math.atan2(self.__velocity.y, self.__velocity.x))