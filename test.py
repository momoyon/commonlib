#!/bin/env python3
import os
import subprocess
import sys

CC="gcc"
TESTS_DIR="./tests/"

class Test:
    expected_stdout = ''
    expected_stderr = ''
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
        if self.expected_returncode == '':
            self.expected_returncode = -1
        else:
            self.expected_returncode = int(self.expected_returncode)

        # if self.expected_stdout: print(f"{self.name}.out.expected: {self.expected_stdout}")
        # if self.expected_stderr: print(f"{self.name}.err.expected: {self.expected_stderr}")
    def save_expected(self):
        def write_expected(name: str, content: str):
            f = f"{self.name}.{name}.expected"
            with open(f, "w") as file:
                file.write(content)

        write_expected("out", self.expected_stdout)
        write_expected("err", self.expected_stderr)
        write_expected("code", str(self.expected_returncode))


def usage(program: str):
    print(f"Usage: {program} <subcmd> [flags]")

# NOTE: We named this hhelp because help is a builtin python function
def hhelp():
    print('''
    Subcommands:
        help    - Prints this help message.
        build   - Builds all the tests.
        run     - Runs all the tests.
        record  - Records the expected behaviour of all the tests.

    Flags:
        -h      - Same as the help subcommand.
          ''')

def main():
    program = sys.argv.pop(0)

    if len(sys.argv) <= 0:
        print("[ERROR] Please provide at least one subcommand!", file=sys.stderr)
        usage(program)
        hhelp()
        exit(1)

    flags = []

    subcmds = []

    while len(sys.argv) > 0:
        arg = sys.argv.pop(0)

        if arg.startswith('-') or arg.startswith('/'):
            flags.append(arg)
        else:
            subcmds.append(arg)

    # Parse flags
    for flag_with_prefix in flags:
        flag = flag_with_prefix[1:]
        if flag == 'h':
            hhelp()
            exit(0)
        else:
            print(f"[ERROR] Invalid flag '{flag}'", file=sys.stderr)
            exit(1)

    if len(subcmds) <= 0:
        print("[ERROR] Please provide at least one subcommand!", file=sys.stderr)
        usage(program)
        hhelp()
        exit(1)


    # Change to tests dir
    os.chdir(TESTS_DIR)

    tests = {}

    for e in os.listdir(os.getcwd()):
        if not e.endswith(".c"): continue
        base_name = e.removesuffix(".c")
        if not tests.get(base_name):
            tests[base_name] = Test(base_name)

    for subcmd in subcmds:
        total_tests_count = len(tests)
        current_test_id = -1

        if subcmd == "help":
            hhelp()
            exit(0)
        elif subcmd == "build":
            print(f'----- [BUILD] -----')
            for test_name in tests:
                current_test_id += 1
                print(f'+ Building {test_name} [{current_test_id+1}/{total_tests_count}]...')
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
                    print("[PASS] ", end='')
                    if res.stdout:
                        print(f"{res.stdout}")
                    else:
                        print('')
            if current_test_id == total_tests_count-1:
                print("ALL TESTS BUILD")
        elif subcmd == "run":
            print(f'----- [RUN] -----')
            for test_name in tests:
                current_test_id += 1
                print(f'+ Running {test_name} [{current_test_id+1}/{total_tests_count}]...')
                test = tests[test_name]

                res = None
                try:
                    res = subprocess.run([f"./{test_name}"], capture_output = True, text = True)
                except Exception as e:
                    print(f"[ERROR] Failed to run ./{test_name}: {e}")
                    continue

                if test.expected_returncode == -1:
                    print(f"[WARNING] Test doesn't have any expected returncode!")
                    print(f"[WARNING] Please record the expected behaviour of the test using the 'record' subcommand!")

                if res.stdout != test.expected_stdout:
                    print('[FAILED]', file=sys.stderr)
                    print(f"Expected: >>>{test.expected_stdout}>>>")
                    print(f"But Got: >>>{res.stdout}>>>")
                    continue
                print('[PASS]')
            if current_test_id == total_tests_count-1:
                print("ALL TESTS PASS")
        elif subcmd == "record":
            print(f'----- [RECORD] -----')
            for test_name in tests:
                print(f"+ Recording expected behaviour for '{test_name}'...")
                test = tests[test_name]

                prompt_msg = "Record current behaviour as the expected one? [y/N]"
                ans = input(prompt_msg)

                if ans.lower() == "y":
                    res = subprocess.run([f"./{test_name}"],
                                         capture_output = True,
                                         text = True)
                    tests[test_name].expected_stdout = res.stdout
                    tests[test_name].expected_stderr = res.stderr
                    tests[test_name].expected_returncode = res.returncode
                    tests[test_name].save_expected()
                    print('[SUCCESS] Recorded expected behaviour')
                else:
                    print('[SKIP]')

        else:
            print(f"[ERROR] Invalid subcommand '{subcmd}'", file=sys.stderr)
            exit(1)

if __name__ == "__main__":
    main()
