import os
import subprocess
import sys

CC="gcc"
TESTS_DIR="./tests/"

class Test:
    expected_stdout = None
    expected_stderr = None
    expected_returncode = -1

    def __init__(self, name):
        self.name = name

        def read_or_create_expected_file(name: str) -> str:
            f = f"{self.name}.{name}.expected"
            if not os.path.exists(f):
                with open(f, "w") as file:
                    print(f"[INFO] Created empty {self.name}.{name}.expected")
                return ""
            else:
                with open(f, "r") as file:
                     return file.read()

        self.expected_stdout = read_or_create_expected_file("out")
        self.expected_stderr = read_or_create_expected_file("err")
        self.expected_returncode = read_or_create_expected_file("code")

        # if self.expected_stdout: print(f"{self.name}.out.expected: {self.expected_stdout}")
        # if self.expected_stderr: print(f"{self.name}.err.expected: {self.expected_stderr}")

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
    program = sys.argv.pop(0)

    if len(sys.argv) <= 0:
        print("[ERROR] Please provide a subcommand!", file=sys.stderr)
        usage(program)
        hhelp()
        exit(1)

    subcmd = sys.argv.pop(0)

    # Change to tests dir
    os.chdir(TESTS_DIR)

    tests = {}

    for e in os.listdir(os.getcwd()):
        if not e.endswith(".c"): continue
        base_name = e.removesuffix(".c")
        if not tests.get(base_name):
            tests[base_name] = Test(base_name)

    # for v in tests:
    #     print(f"{v}: {tests[v]}")

    if subcmd == "help":
        hhelp()
        exit(0)
    elif subcmd == "build":
        for test_name in tests:
            print(f'+ Building {test_name}...')
            test = tests[test_name]

            res = subprocess.run([CC, '-o', test_name, f"{test_name}.c"],
                                 capture_output = True,
                                 text = True)
            if res.returncode != 0:
                print("[FAILED] ", end='')
                if res.stderr:
                    print(f"{res.stderr}")
                else:
                    print('')
                continue
            else:
                print("[SUCCESS] ", end='')
                if res.stdout:
                    print(f"{res.stdout}")
                else:
                    print('')

    elif subcmd == "run":
        for test_name in tests:
            print(f'+ Running {test_name}...')
            test = tests[test_name]

            res = subprocess.run([f"./{test_name}"],
                                 capture_output = True,
                                 text = True)

            if len(test.expected_returncode) <= 0:
                print(f"[WARNING] Test doesn't have any expected returncode!")
                print(f"[WARNING] Please record the expected behaviour of the test using the 'record' subcommand!")

            if res.stdout != test.expected_stdout:
                print('[FAILED]', file=sys.stderr)
                print(f"Expected: >>>{test.expected_stdout}>>>")
                print(f"But Got: >>>{res.stdout}>>>")
                continue
            print('[SUCCESS]')
    else:
        print(f"[ERROR] Invalid subcommand '{subcmd}'", file=sys.stderr)
        exit(1)

if __name__ == "__main__":
    main()
