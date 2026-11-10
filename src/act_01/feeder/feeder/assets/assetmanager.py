from importlib import resources
from os import path

from feeder.assets.asset import Asset
from feeder.assets.assettype import AssetType

class AssetManager:

    __instance = None

    @staticmethod
    def instance():
        if AssetManager.__instance is None:
            AssetManager()
        return AssetManager.__instance

    def __init__(self):
        if AssetManager.__instance is None:
            AssetManager.__instance = self

            self.__assets = {}

        else:
            raise Exception("Only one instance of AssetManager is Allowed!!!!")

    def load(self, category, asset_type, asset_name, asset_filename, font_size = 0, rows = 0, cols = 0):
        with resources.path(asset_filename[0], asset_filename[1]) as asset_path:
            if path.isfile(asset_path) and asset_name not in self.__assets:
                asset = Asset(asset_type, category)
                asset.load(asset_path, font_size, rows, cols)
                self.__assets[asset_name] = asset

    def set(self, category, asset_type, asset_name, payload):
        if asset_name not in self.__assets:
            asset = Asset(asset_type, category)
            asset.payload = payload
            self.__assets[asset_name] = asset

    def get(self, asset_name, row = 0, sequence = 0):
        if asset_name in self.__assets:
            if self.__assets[asset_name].asset_type == AssetType.Image:
                return self.__assets[asset_name].payload, self.__assets[asset_name].payload.get_rect()
            elif self.__assets[asset_name].asset_type == AssetType.SpriteSheet:
                return self.__assets[asset_name].payload.get_image(row, sequence)
            else:
                return self.__assets[asset_name].payload
        else:
            return None

    def clean(self, category = None):
        if category:
            for k in list(self.__assets.keys()):
                if self.__assets[k].category == category:
                    del self.__assets[k]
        else:
            self.__assets = {}
