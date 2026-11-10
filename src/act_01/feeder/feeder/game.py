from importlib import resources
import os
from os import path
import random

import pygame

from feeder.config import cfg_item, Config
from feeder.assets.splines.spline import Spline
from feeder.assets.assetmanager import AssetManager
from feeder.assets.assettype import AssetType
from feeder.assets.background import Background
from feeder.entities.bird import Bird
from feeder.events import Events
from feeder.entities.rendergroup import RenderGroup

class Game:

    def __init__(self):
        pygame.init()

        self.__screen = pygame.display.set_mode(cfg_item("game", "screen_size"), 0, 32)
        pygame.display.set_caption(cfg_item("game", "title"))

        self.__load_assets()
        self.__create_splines()

        self.__running = False

        self.__bg = Background(cfg_item("background", "name"))
        self.__birds = RenderGroup()

        self.__birds.add(Bird("eagle", self.__splines[0]))

    def __del__(self):
        AssetManager.instance().clean()
        pygame.quit()

    def run(self):
        self.__running = True

        last_time = pygame.time.get_ticks()
        time_since_last_frame = 0
        time_since_last_feed = 0

        time_per_frame = cfg_item("game", "time_per_frame")
        time_per_feed = cfg_item("feed", "time_per_feed")
        time_to_sleep = cfg_item("game", "time_to_sleep")

        while self.__running:
            delta_time, last_time = self.__calc_delta_time(last_time)
            time_since_last_frame += delta_time
            time_since_last_feed += delta_time

            if time_since_last_frame > time_per_frame:
                time_since_last_frame -= time_per_frame
                self.__events_loop()
                self.__update(time_per_frame)
                self.__render()
            elif time_since_last_feed > time_per_feed:
                time_since_last_feed -= time_per_feed
                self.__produce_feed()
            else:
                pygame.time.wait(time_to_sleep)

    def __events_loop(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.__running = False
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    self.__running = False
                if event.key == pygame.K_F5:
                    Config.instance().debug = not Config.instance().debug
            if event.type == pygame.USEREVENT:
                self.__process_events(event)

    def __process_events(self, event):
        if event.event == Events.BIRD_END_POINT:
            self.__birds.remove(event.bird)

    def __update(self, delta_time):
        self.__spawn_bird(delta_time)
        self.__birds.update(delta_time)

    def __render(self):
        self.__screen.fill(cfg_item("game", "bg_color"))
        self.__bg.render(self.__screen)
        self.__render_debug(self.__screen)
        self.__birds.draw(self.__screen)
        pygame.display.update()

    def __calc_delta_time(self, last_time):
        current_time = pygame.time.get_ticks()
        delta_time = current_time - last_time
        return delta_time, current_time

    def __load_assets(self):
        AssetManager.instance().load('main', AssetType.Font, cfg_item("font", "name"), cfg_item("font", "file"), font_size = cfg_item("font", "size"))
        AssetManager.instance().load('main', AssetType.Image, cfg_item("background", "name"), cfg_item("background", "image_file"))
        AssetManager.instance().load('main', AssetType.SpriteSheet, cfg_item("entities", "name"), cfg_item("entities", "image_file"), rows = cfg_item("entities", "size")[0], cols = cfg_item("entities", "size")[1])
        self.__load_asset_birds()

    def __load_asset_birds(self):
        birds = cfg_item("entities", "birds")
        for bird in birds:
            for i in range(3):
                sheet, clip = AssetManager.instance().get(cfg_item("entities", "name"), row = birds[bird]["row"], sequence = i)
                image = pygame.Surface(clip.size, flags = pygame.SRCALPHA, depth = 32)
                image.blit(sheet, pygame.Rect((0,0), clip.size), clip)
                AssetManager.instance().set('main', AssetType.Image, bird + str(i), image)

    def __create_splines(self):
        self.__splines = []
        self.__spline_control_points = cfg_item("splines")
        self.__spline_points = []

        for spline_control_points in self.__spline_control_points:
            control_points = []
            for control_point in spline_control_points:
                point_as_vector = pygame.math.Vector2(control_point)
                control_points.append(point_as_vector)
            sp = Spline(control_points)
            self.__splines.append(sp)

            spline_res = cfg_item("debug", "spline_res")
            spline_points = []
            ratio = 1/spline_res
            step = 0
            for _ in range(spline_res):
                spline_points.append(sp.get_point(step))
                step += ratio
            self.__spline_points.append(spline_points)

    def __render_debug(self, surface_dst):
        if Config.instance().debug:
            for sp_cp in self.__spline_control_points:
                for point in sp_cp:
                    pygame.draw.circle(surface_dst, cfg_item("debug", "spline_control_point_color"), point, 2)

            for spline_points in self.__spline_points:
                old_point = pygame.math.Vector2(spline_points[0])
                for i in range(cfg_item("debug", "spline_res")):
                    point = spline_points[i]
                    if i>0:
                        pygame.draw.line(surface_dst, cfg_item("debug", "spline_color"),(old_point.x, old_point.y),(point.x, point.y), 1)
                    old_point = point

    def __spawn_bird(self, delta_time):
        spawn_prob = cfg_item("entities", "spawn_prob")
        if random.random() < spawn_prob:
            bird_name = random.choice(list(cfg_item("entities", "birds")))
            spline = random.choice(self.__splines)
            self.__birds.add(Bird(bird_name, spline))

    def __produce_feed(self):
        ticks = str(pygame.time.get_ticks())

        tmp_pack = cfg_item("feed", "file_name_tmp")       
        with resources.path(tmp_pack[0], tmp_pack[1]) as tmp_base:
            tmp_path = str(tmp_base) + ticks + ".png"

        file_pack = cfg_item("feed", "file_name")
        with resources.path(file_pack[0], file_pack[1]) as file_base:
            final_path = str(file_base) + ticks + ".png"

        pygame.image.save(self.__screen, tmp_path)
        os.replace(tmp_path, final_path)