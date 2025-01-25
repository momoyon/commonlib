import os
import subprocess
import sys

TESTS_DIR="./tests/"

def usage(program: str):
    print(f"Usage: {program} <subcmd>")

# NOTE: We named this hhelp because help is a builtin python function
def hhelp():
    print('''

    Subcommands:
        help    - Prints this help message.
        build   - Builds all the tests.
        run     - Runs all the tests.
          ''')

def main():
    og_dir = os.getcwd()
    program = sys.argv.pop()

    if len(sys.arg) <= 0:

        usage(program)
        hhelp()

    # Change to tests dir
    os.chdir(TESTS_DIR)

    for e in os.listdir(os.getcwd()):
        print(f"entry: {e}")

if __name__ == "__main__":
    main()
