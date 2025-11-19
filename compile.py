from os.path import abspath, dirname, join
from subprocess import run
from time import sleep

RUN_AFTER_COMPILE = False

COMPILER = r"gcc"
SRC_FILEPATH = r"src"
OUTPUTNAME = r"main.exe"

INCLUDE = r"include"

WARNINGS = "-Wall -Werror"

DirectoryPath = dirname(abspath(__file__))

SOURCE = join(SRC_FILEPATH, '*.c')

CMD = f'{COMPILER} {WARNINGS} {SOURCE} -I {INCLUDE} -o "{OUTPUTNAME}"'

print(CMD)

result = run(CMD, shell=True, capture_output=True, text=True)

if(result.returncode != 0):
    print("Error:")
    print(result.stderr)
else:
    print("Code Sucesfullly Compiled!!")
    if(RUN_AFTER_COMPILE):
        print("Running program\n\n")
        sleep(2)

        CMD = f"{OUTPUTNAME}"
        run(CMD, shell=True)

# gcc -Wall -Werror src\*.c -I include -o "main.exe"