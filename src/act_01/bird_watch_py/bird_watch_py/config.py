from importlib import resources
import json

def cfg_item(*items):
    data = Config.instance().data
    for item in items:
        data = data[item]
    return data

class Config:
    __json_file =  ["bird_watch_py.assets.config", "config.json"]
    __instance = None

    @staticmethod
    def instance():
        if Config.__instance is None:
            Config()
        return Config.__instance

    def __init__(self):
        if Config.__instance is None:
            Config.__instance = self
            with resources.path(Config.__json_file[0], Config.__json_file[1]) as font_path:
                with open(font_path) as file:
                    self.data = json.load(file)
            self.__debug = False
        else:
            raise Exception("Only one instance of Config is Allowed!!!!")