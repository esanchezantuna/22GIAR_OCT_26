import pygame

class SpriteSheet:

    def __init__(self, image_file, rows, cols):
        self.__image = pygame.image.load(image_file).convert_alpha()
        self.__sequences = []

        width = self.__image.get_width() / cols
        height = self.__image.get_height() / rows

        for row in range(rows):
            row_seq = []
            y = row * height
            for col in range(cols):
                x = col * width
                row_seq.append(pygame.Rect(x, y, width, height))
            self.__sequences.append(row_seq)

    def get_image(self, row, sequence = 0):
        if row <= len(self.__sequences):
            row_seq = self.__sequences[row]
            if sequence <= len(row_seq):
                return self.__image, row_seq[sequence]
        return pygame.Surface((0,0)), pygame.Rect(0,0,0,0)