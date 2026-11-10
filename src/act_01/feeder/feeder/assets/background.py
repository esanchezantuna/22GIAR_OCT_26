from feeder.assets.assetmanager import AssetManager

class Background:

    def __init__(self, name):
        self.__name = name

        _, rect = AssetManager.instance().get(self.__name)
        self.__render_rect = rect

    def render(self, surface_dst):
        image, rect = AssetManager.instance().get(self.__name)
        surface_dst.blit(image, self.__render_rect, rect)
