from collections import deque
from pathlib import Path
import time

from PIL import Image, ImageChops

from bird_watch_py.config import cfg_item

class App:

    def __init__(self):
        self.__end_time = time.time() + cfg_item("feed", "time_running") / 1000
        self.__feed_path = Path(*cfg_item("feed", "feed_path"))
        self.__output_path = Path(*cfg_item("feed", "output_path"))
        self.__diff_path = Path(*cfg_item("feed", "diff_path"))
        self.__queue = []
        self.__diff_id = 0

    def run(self):
        """ Main Loop for a limited time
        - Watch the feeder folder
        - Process an image when arrives and enqueue it
        - Every n images calls diff process
        - Deletes source image"""

        print(f"Running Bird Watch for {cfg_item("feed", "time_running") / 1000} seconds")
        while True:
            if time.time() > self.__end_time:
                print(f"Work is Done")
                break

            files = deque([f for f in self.__feed_path.iterdir() if f.is_file()])

            while files:
                file = files.popleft()
                self.__queue.append(self.__process_file(file))
                if len(self.__queue) >= cfg_item("feed", "diff_queue_length"):
                    self.__calc_diff()
                    self.__computer_vision_algorithm()

            time.sleep(cfg_item("feed", "sleep_time") / 1000)

    def __process_file(self, file):
        """Process an image, sequentially
        - Opens the file and loads the image
        - Resize in half size
        - Rotates 90CW
        - Converts Color to GrayScale
        - Saves the processed image"""

        with Image.open(file) as img:
            img.load()

        if img:
            file.unlink(missing_ok=True)
            print(f"Image loaded correctly {file.name}, {img.size[0]}x{img.size[1]},{img.mode}")
            img = self.__resize_half(img)
            img = self.__rotate_90cw(img)
            img = self.__grayscale(img)

            path_dst = self.__output_path / file.name
            img.save(path_dst)
            print(f"Processed image saved correctly {path_dst}, {img.size[0]}x{img.size[1]},{img.mode}")

            return img
        else:
            print(f"Error loading image {file.name}")
            return None

    def __resize_half(self, img):
        """Resize the image to half width and height"""

        time.sleep(100 / 1000)   #simulate latency

        w, h = img.size
        return img.resize((w // 2, h // 2), Image.Resampling.BOX)

    def __rotate_90cw(self, img):
        """Rotate the image 90° clock wise"""

        time.sleep(100 / 1000)   #simulate latency

        return img.rotate(-90, expand=True)

    def __grayscale(self, img):
        """Convert the image to grayscale (keeping 3 RGB channels)"""

        time.sleep(100 / 1000)   #simulate latency

        return img.convert("L").convert("RGB")

    def __calc_diff(self):
        """ Calculate the difference for all images in queue
        - Create a new image to accumulate the difference, black color is no diff
        - Diffs every image against the first one as base
        - Accumulates that diff adding to the diff image
        - Save the diff result
        - Clear the queue
        """

        time.sleep(100 / 1000)   #simulate latency

        base = self.__queue[0]
        diff_img = Image.new("RGB", base.size, (0,0,0))

        for img in self.__queue[1:]:
            diff_current = ImageChops.difference(base, img)
            diff_img = ImageChops.add(diff_img, diff_current, scale=1.0, offset=0)

        path_dst = self.__diff_path / (cfg_item("feed","diff_name_prefix") + str(self.__diff_id) + '.png')
        diff_img.save(path_dst)
        self.__diff_id += 1
        print(f"Diff image saved correctly {path_dst}, {diff_img.size[0]}x{diff_img.size[1]},{diff_img.mode}")

        self.__queue = []

    def __computer_vision_algorithm(self):
        """ Placeholder for the computer vision algorithm
        that should process the diff images an get the path info for birds"""

        time.sleep(100 / 1000)   #simulate latency

        return None