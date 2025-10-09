import sys
import subprocess
import os

def main():
    package_dir = os.path.dirname(__file__)
    exe_path = os.path.join(package_dir, "bin", "raven")
    sys.exit(subprocess.call([exe_path] + sys.argv[1:]))