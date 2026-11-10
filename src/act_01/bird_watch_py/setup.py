from setuptools import setup

setup(
    name = "bird_watch_py",
    version = "1.0.0",
    packages = ["bird_watch_py"],
    install_requires = ["pillow"],
    entry_points = {
        "console_scripts" : ["bird_watch_py = bird_watch_py.__main__:main"]
    }
)