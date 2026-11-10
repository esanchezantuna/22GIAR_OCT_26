from setuptools import setup

setup(
    name = "feeder",
    version = "1.0.0",
    packages = ["feeder"],
    install_requires = ["pygame"],
    entry_points = {
        "console_scripts" : ["feeder = feeder.__main__:main"]
    }
)