import sys

from bird_watch_py.app import App

def main(args = None):
    if args is None:
        args = sys.argv[:1]
    app = App()
    app.run()

if __name__ == '__main__':
    sys.exit(main())