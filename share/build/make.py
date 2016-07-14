#!/usr/bin/python3

import build
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
                           '#endif\n'.format(version=version)
                vf.write(contents)
    else:
        print("Failed running git command!");

    return result.returncode

################################################################################

def generate_build_system_deps():
    for t in buildfile.generated_files:
        t.generate()

################################################################################

def run_memcheck():
    invalid_read_str = 'Invalid read'
    no_leak_str = 'All heap blocks were freed -- no leaks are possible'
    command = ['valgrind', '--leak-check=full', '--show-leak-kinds=all']

    for f in ['./checks/check-globals',
              './checks/check-image',
              './checks/check-pool',
              './checks/check-store',
              './checks/check-chain',
              './checks/check-list',
              './checks/check-branch',
              './checks/check-frame']:
        cmd = list(command)
        cmd.extend([f, '-q'])
        result = build.Run.command(cmd)

        is_failure = (result.returncode != 0) \
                  or (no_leak_str not in result.stderr) \
                  or (invalid_read_str in result.stderr)

        print(result.stdout)
        if is_failure:
            print('Valgrind reported error!\n')

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
    def print_command():
        print(' >>> {command}'.format(command=command))

    if command == 'version':
        result = generate_version_file()

    elif command == 'memcheck':
        print_command()
        result = run_memcheck()

    elif command == 'cppcheck':
        print_command()
        result = run_cppcheck()

    elif command == 'generate':
        print_command()
        import buildfile
        generate_build_system_deps()

    elif command == 'ninja':
        print_command()
        import buildfile
        result = build.NinjaWriter().write(buildfile.p)

    elif command == 'make':
        print_command()
        import buildfile
        result = build.MakeWriter().write(buildfile.p)

    elif command == 'install':
        print_command()
        import buildfile
        ins = build.Installer(buildfile.c)
        ins(buildfile.metanoia, build.InstallConfigExec())

    else:
        print('Unknown command "{command}"'.format(command=command))
        result = 1

    exit(result)

################################################################################

