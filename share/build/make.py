#!/usr/bin/python
# vim: tabstop=4 expandtab colorcolumn=81 list

import build
import buildfile
import sys

################################################################################

def generate_version_file():
    git_command = ['git', 'describe', '--always']
    result = build.Run.command(git_command)
    if result.returncode == 0:
        file_path = 'gen/version.h'
        version = result.stdout
        contents = ''

        try:
            with open(file_path, 'r') as vf:
                contents = vf.read()
        except FileNotFoundError:
            pass

        if version not in contents:
            with open(file_path, 'w') as vf:
                contents = '#ifndef NOIA_VERSION\n' \
                           '#define NOIA_VERSION \"{version}\"\n' \
                           '#endif\n'''.format(version=version)
                vf.write(contents)
    else:
        prinf("Failed running git command!");

    return result.returncode

################################################################################

def run_memcheck():
    success_str = 'All heap blocks were freed -- no leaks are possible'
    command = ['valgrind', '--leak-check=full', '--show-leak-kinds=all']

    for f in ['./checks/check-globals',
              './checks/check-pool',
              './checks/check-store',
              './checks/check-chain',
              './checks/check-list',
              './checks/check-branch',
              './checks/check-frame']:
        cmd = list(command)
        cmd.extend([f, '-q'])
        result = build.Run.command(cmd)

        if result.returncode:
            print('Valgrind reported error!')

        if success_str not in result.stdout:
            print(result.stdout)

################################################################################

def run_cppcheck():
    command = ['cppcheck', '-q', '--enable=all',
               '--template=[{severity}] {file} ({line}): {id} - {message}',
               '--suppress=incorrectStringBooleanError', '.']

    result = build.Run.command(command, pipe=False)
    return result.returncode

################################################################################

if __name__ == '__main__':
    result = 0
    command = sys.argv[1] if len(sys.argv) > 1 else ''
    if command == 'version':
        result = generate_version_file()
    elif command == 'memcheck':
        result = run_memcheck()
    elif command == 'cppcheck':
        result = run_cppcheck()
    else:
        result = buildfile.run(command)

    exit(result)

################################################################################

