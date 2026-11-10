from abc import ABC, abstractmethod

import pygame

from feeder.config import Config, cfg_item

class GameObject(pygame.sprite.Sprite, ABC):

    def __init__(self):
        super().__init__()
        self._position = pygame.math.Vector2(0.0, 0.0)
        self.rect = pygame.Rect(0,0,0,0)
        self._render_rect = pygame.Rect(0,0,0,0)

    @abstractmethod
    def handle_input(self, key, is_pressed):
        pass

    @abstractmethod
    def process_events(self, event):
        pass

    @abstractmethod
    def update(self, delta_time):
        pass

    @abstractmethod
    def render(self, surface_dst):
        pass

    def _render_debug(self, surface_dst, position = None):
        if Config.instance().debug:
            if not position:
                position = self._position
            pygame.draw.rect(surface_dst, cfg_item("debug", "collider_color"), self.rect, 1)
            pygame.draw.rect(surface_dst, cfg_item("debug", "render_color"), self._render_rect, 1)
            pygame.draw.circle(surface_dst, cfg_item("debug", "position_color"), position, 2)

    def _center(self, position):
        self.rect.center = position.xy
        self._render_rect.center = position.xy

    def _in_bounds(self, distance):
        new_position = self._position + distance

        return new_position.x >= 0 and new_position.x <= cfg_item("game", "screen_size")[0] and new_position.y >= 0 and new_position.y <= cfg_item("game", "screen_size")[1]

    #TODO Refactor in only one function
    def _in_bounds_hor(self, distance):
        new_position = pygame.math.Vector2(self._position.x + distance.x, self._position.y)

        return new_position.x >= 0 and new_position.x <= cfg_item("game", "screen_size")[0]

    def _in_bounds_ver(self, distance):
        new_position = pygame.math.Vector2(self._position.x, self._position.y + distance.y)

        return new_position.y >= 0 and new_position.y <= cfg_item("game", "screen_size")[1]


    @property
    def position(self):
        return self._position

    @position.setter
    def position(self, value):
        self._position = value