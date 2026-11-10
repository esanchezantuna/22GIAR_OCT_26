import pygame

from feeder.assets.spritesheet import SpriteSheet
from feeder.assets.assettype import AssetType

class Asset:

    def __init__(self, asset_type, category):
        self.__asset_type = asset_type
        self.__category = category
        self.__payload = None

    def load(self, asset_filename_path, font_size = 0, rows = 0, cols = 0):
        if self.__asset_type == AssetType.Image:
            self.__payload = pygame.image.load(asset_filename_path).convert_alpha()
        elif self.__asset_type == AssetType.SpriteSheet:
            self.__payload = SpriteSheet(asset_filename_path, rows, cols)
        elif self.__asset_type == AssetType.Font:
            self.__payload = pygame.font.Font(asset_filename_path, font_size)

    @property
    def asset_type(self):
        return self.__asset_type

    @property
    def payload(self):
        return self.__payload

    @payload.setter
    def payload(self, value):
        self.__payload = value

    @property
    def category(self):
        return self.__category
