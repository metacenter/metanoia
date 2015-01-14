#!/usr/bin/python
# file: make.py
# vim: tabstop=4 expandtab colorcolumn=81 list

import os.path
import subprocess

#-------------------------------------------------------------------------------

class Generator:
    """Represends file generating script or program"""

    command_body = None
    command_args = None
    shortcut = None

    def __init__(self, command_body, command_args):
        self.command_body = command_body
        self.command_args = command_args

    def get_command(self, output_path, input_paths):
        inputs = ' '.join(input_paths)
        args = self.command_args.format(output=output_path, inputs=inputs)
        return '{0} {1}'.format(self.command_body, args)

#-------------------------------------------------------------------------------

class Target:
    """Represents target object"""

    proc = None
    output = None
    inputs = list()
    deps = list()
    pkgs = set()
    input_dir = str()
    output_dir = str()

    def __init__(self, output, inputs,
                 deps=list(), pkgs=set(),
                 input_dir=str(), output_dir=str()):
        self.output = output
        self.inputs = inputs
        self.deps = deps
        self.pkgs = pkgs
        self.input_dir = input_dir
        self.output_dir = output_dir

    def __str__(self):
        return self.output

    def add_input(self, inp):
        """Adds input to target"""

        if isinstance(inp, str):
            if inp in self.inputs:
                raise Exception('Input "{0}" already given!'.format(inp))
            self.inputs.append(inp)

        elif isinstance(inp, Target):
            self.add_input(inp.output)
            self.add_pkgs(inp.pkgs)

        else:
            raise Exception('Input must be passed as string! '
                            'Got {0}'.format(type(inp)));

    def add_pkgs(self, pkgs):
        """Adds packages to link"""

        if  (not isinstance(pkgs, set)):
            raise Exception('Packages must be passed as set of strings! '
                            'Got {0}'.format(type(pkgs)));

        self.pkgs.update(pkgs)

#-------------------------------------------------------------------------------

class CompileTarget(Target):
    pass

class LinkTarget(Target):
    pass

class GeneratedTarget(Target):
    generator = None
    def __init__(self, output, inputs, deps=list(), generator=None,
                 input_dir=str(), output_dir=str()):
        Target.__init__(self, output, inputs, deps=deps,
                        input_dir=input_dir, output_dir=output_dir)
        self.generator = generator

#-------------------------------------------------------------------------------

class Make:

    #---------------------------------------------------------------------------
    # GLOBAL VARIABLES
    #---------------------------------------------------------------------------

    cc = 'gcc'
    cflags = list()
    cflags = list()
    lflags = list()

    srcdir = 'src'
    builddir = 'build'
    resdir = 'res'
    gendir = 'gen'

    all = list()
    targets = list()
    generators = list()

    #---------------------------------------------------------------------------
    # COMMANDS
    #---------------------------------------------------------------------------

    def run_makefile(self, args):
        with open('Makefile', 'w') as mf:
            def get_command_output(args):
                return subprocess.check_output(args).decode("utf-8")[:-1]

            def wr(string='', end='\n'):
                print(string, file=mf, end=end)

            def wr_compile_command(t, output_path, input_paths):
                wr('\n\t@mkdir -p {0}'.format(self.builddir))
                wr('\t@echo "  CC   {0}"'.format(t.output))
                wr('\t@{0} {1} -o {2} -I{3} -I{4} \\'
                    .format(self.cc, ' '.join(self.cflags), output_path,
                            self.srcdir, self.gendir))
                wr('\t       -c {0}'.format(' '.join(input_paths)), end='')
                if len(t.pkgs):
                    command = ['pkg-config', '--cflags']
                    command.extend(sorted(t.pkgs))
                    output = get_command_output(command)
                    if output:
                        wr(' \\\n\t       {0}'.format(output), end='')

            def wr_link_command(t, output_path, input_paths):
                wr('\n\t@mkdir -p {0}'.format(self.builddir))
                wr('\t@echo "  LD   {0}"'.format(t.output))
                wr('\t@{0} {1} -o {2} \\' \
                    .format(self.cc, ' '.join(self.lflags), output_path))
                wr('\t       {0}'.format(' '.join(input_paths)), end='')
                if len(t.pkgs):
                    command = ['pkg-config', '--libs']
                    command.extend(sorted(t.pkgs))
                    output = get_command_output(command)
                    if output:
                        wr(' \\\n\t       {0}'.format(output), end='')

            def wr_gen_command(t, output_path, input_paths):
                wr('\n\t@mkdir -p {0}'.format(self.gendir))
                wr('\t@echo "  GEN  {0}"'.format(t.output))
                command = t.generator.get_command(output_path, input_paths)
                wr('\t@{0}'.format(command), end='')

            def wr_gen_target(t, output_path, dep_paths):
                padding = (len(output_path) + 2) * ' '
                dependences = list()
                dependences.extend(input_paths)
                dependences.extend(dep_paths)
                wr('{0}: Makefile'.format(output_path), end='')
                for d in dependences:
                    wr(' \\\n{0}{1}'.format(padding, d), end='')

            def wr_compile_target(t, output_path, input_paths, dep_paths):
                command = ['gcc', '-MM', '-MT', output_path, '-Igen']
                command.extend(input_paths)
                dep_str = get_command_output(command)
                dep_list = [s for s in dep_str.split(' ')[2:] if len(s) > 8]
                dep_paths.extend(dep_list)
                wr_gen_target(t, output_path, dep_paths)


            def make_path_list(objects, default_dir):
                result = list()
                for o in objects:
                    directory = default_dir

                    if isinstance(o, Target):
                        directory = o.output_dir

                    result.append(os.path.join(directory, str(o)))

                return result


            print('  >> Makefile')

            if len(self.all):
                wr('all: {0}\n'.format(' '.join(self.all)))
            wr('clean:\n\trm -rf {0} {1}\n'.format(self.builddir, self.gendir))

            for t in self.targets:
                output_path = os.path.join(t.output_dir, t.output)
                input_paths = make_path_list(t.inputs, t.input_dir)
                dep_paths   = make_path_list(t.deps, t.input_dir)

                if isinstance(t, CompileTarget):
                    wr_compile_target(t, output_path, input_paths, dep_paths)
                    wr_compile_command(t, output_path, input_paths)
                elif isinstance(t, LinkTarget):
                    wr_gen_target(t, output_path, dep_paths)
                    wr_link_command(t, output_path, input_paths)
                elif isinstance(t, GeneratedTarget):
                    wr_gen_target(t, output_path, dep_paths)
                    wr_gen_command(t, output_path, input_paths)
                else:
                    print("Unknown process type!")
                wr('\n')

    #---------------------------------------------------------------------------
    # METHODS
    #---------------------------------------------------------------------------

    def validate_arguments(self, output, inputs, deps,
                           pkgs, input_dir, output_dir):
        if output is None:
            raise Exception('No output path specified!')

        if not isinstance(output, str):
            raise Exception('Output path must be passed as string! '
                            'Got {0}'.format(type(output)));

        if  (not isinstance(inputs, list)) \
        and (not isinstance(inputs, tuple)):
            raise Exception('Input must be passed as list of strings!, '
                            'Got {0}'.format(type(inputs)));

        if  (not isinstance(deps, list)) \
        and (not isinstance(deps, tuple)):
            raise Exception('Dependences must be passed as list of strings! '
                            'Got {0}'.format(type(deps)));

        if not isinstance(pkgs, set):
            raise Exception('Packages must be passed as set of strings! '
                            'Got {0}'.format(type(pkgs)))

        if not isinstance(input_dir, str):
            raise Exception('Input directory must be passed as string! '
                            'Got {0}'.format(type(input_dir)))

        if not isinstance(output_dir, str):
            raise Exception('Output directory must be passed as string! '
                            'Got {0}'.format(type(input_dir)))

    #---------------------------------------------------------------------------

    def add_compile_target(self,
                           output=None,
                           inputs=list(),
                           deps=list(),
                           pkgs=set(),
                           input_dir='',
                           output_dir=''):
        try:
            self.validate_arguments(output, inputs, deps,
                                    pkgs, input_dir, output_dir)
        except Exception as e:
            print(e.message)
            return

        input_dir  = input_dir  if input_dir  != '' else self.srcdir
        output_dir = output_dir if output_dir != '' else self.builddir

        target = CompileTarget(output, inputs, deps=deps, pkgs=pkgs,
                               input_dir=input_dir, output_dir=output_dir)
        self.targets.append(target)
        return target

    #---------------------------------------------------------------------------

    def add_link_target(self,
                        output=None,
                        inputs=list(),
                        pkgs=set(),
                        input_dir='',
                        output_dir='',
                        include_in_all=False):
        try:
            self.validate_arguments(output, inputs, list(),
                                    pkgs, input_dir, output_dir)
        except Exception as e:
            print(e.message)
            return

        input_dir  = input_dir  if input_dir  != '' else self.builddir
        output_dir = output_dir if output_dir != '' else self.builddir

        target = LinkTarget(output, inputs, pkgs=pkgs,
                            input_dir=input_dir, output_dir=output_dir)
        self.targets.append(target)

        if include_in_all:
            self.all.append(os.path.join(self.builddir, output))

        return target

    #---------------------------------------------------------------------------

    def add_generated_target(self,
                             generator=None,
                             output=None,
                             inputs=list(),
                             deps=list(),
                             input_dir='',
                             output_dir=''):
        try:
            self.validate_arguments(output, inputs, deps,
                                    set(), input_dir, output_dir)
        except Exception as e:
            print(e.message)
            return

        input_dir  = input_dir  if input_dir  != '' else self.resdir
        output_dir = output_dir if output_dir != '' else self.gendir

        target = GeneratedTarget(output, inputs,
                                 deps=deps, generator=generator,
                                 input_dir=input_dir, output_dir=output_dir)
        self.targets.append(target)
        return target

    #---------------------------------------------------------------------------

    def add_generator(self, generator):
        if isinstance(generator, Generator):
            self.generators.append(generator)
        else:
            print('Passed parameter is not Generator!')

    #---------------------------------------------------------------------------

    def run(self, args):
        if not len(args):
            print('No command specified!')
            return

        # command
        c = args[0]
        # arguments
        a = args[1:] if len(args) > 1 else list()

        if c == 'Makefile':
            self.run_makefile(a)

        else:
            print('Unknown command: "{0}"'.format(command))

    #---------------------------------------------------------------------------
    # SETTERS
    #---------------------------------------------------------------------------

    def set_c_compiler(self, name):
        if isinstance(name, str):
            self.cc = name
        else:
            print('Compiler anme must be passed as string!')

    def set_cflags(self, flags):
        if isinstance(flags, list) or isinstance(flags, tuple):
            self.cflags = flags
        else:
            print('WFLAGS must be passed as list of strings!')

    def set_lflags(self, flags):
        if isinstance(flags, list) or isinstance(flags, tuple):
            self.lflags = flags
        else:
            print('LFLAGS must be passed as list of strings!')

    def set_source_directory(self, path):
        if isinstance(path, str):
            self.srcdir = path
        else:
            print('Source directory must be passed as string!')

    def set_build_directory(self, path):
        if isinstance(path, str):
            self.builddir = path
        else:
            print('Build directory must be passed as string!')

    def set_resource_directory(self, path):
        if isinstance(path, str):
            self.resdir = path
        else:
            print('Resource directory must be passed as string!')

    def set_gen_directory(self, path):
        if isinstance(path, str):
            self.gendir = path
        else:
            print('Gen directory must be passed as string!')

    #---------------------------------------------------------------------------
    # GETTERS
    #---------------------------------------------------------------------------

    def get_source_directory(self):
        return self.srcdir

    def get_build_directory(self):
        return self.builddir

    def get_resource_directory(self):
        return self.resdir

    def get_gen_directory(self):
        return self.gendir

#-------------------------------------------------------------------------------
