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

    def __init__(self, name, output, inputs, deps=list(), pkgs=set()):
        self.name = name
        self.output = output
        self.inputs = inputs
        self.deps = deps
        self.pkgs = pkgs

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
    def __init__(self, name, output, inputs,
                 deps=list(), generator=None):
        Target.__init__(self, name, output, inputs, deps=deps)
        self.generator = generator

class TestTarget(Target):
    pass

#-------------------------------------------------------------------------------

class Make:

    #---------------------------------------------------------------------------
    # GLOBAL VARIABLES
    #---------------------------------------------------------------------------

    cc = 'gcc'
    cflags = list()
    lflags = list()
    oflags = list()

    srcdir = 'src'
    builddir = 'build'
    resdir = 'res'
    gendir = 'gen'
    testdir = 'tests'
    checkdir = 'checks'

    all = list()
    checks = list()
    targets = list()
    generators = list()

    #---------------------------------------------------------------------------
    # COMMANDS
    #---------------------------------------------------------------------------

    def run_makefile(self, args):
        with open('Makefile', 'w') as mf:
            def get_command_output(args):
                try:
                    return subprocess.check_output(args).decode("utf-8")[:-1]
                except:
                    return ''

            def wr(string='', end='\n'):
                print(string, file=mf, end=end)

            def wr_compile_command(t):
                inputs = list()
                inputs.extend(t.inputs)
                inputs.extend(t.deps)
                wr('\n\t@mkdir -p {0}'.format(self.builddir))
                wr('\t@echo "  CC   {0}"'.format(t.name))
                wr('\t@{0} {1} {2} -o {3} -I{4} -I{5} \\'
                    .format(self.cc, ' '.join(self.cflags),
                     ' '.join(self.oflags), t.output, self.srcdir, self.gendir))
                wr('\t       -c {0}'.format(' '.join(inputs)), end='')
                if len(t.pkgs):
                    command = ['pkg-config', '--cflags']
                    command.extend(sorted(t.pkgs))
                    output = get_command_output(command)
                    if output:
                        wr(' \\\n\t       {0}'.format(output), end='')

            def wr_link_command(t):
                wr('\n\t@mkdir -p {0}'.format(self.builddir))
                wr('\t@echo "  LD   {0}"'.format(t.name))
                wr('\t@{0} {1} {2} -o {3} \\' \
                    .format(self.cc, ' '.join(self.lflags),
                            ' '.join(self.oflags), t.output))
                wr('\t       {0}'.format(' '.join(t.inputs)), end='')
                if len(t.pkgs):
                    command = ['pkg-config', '--libs']
                    command.extend(sorted(t.pkgs))
                    output = get_command_output(command)
                    if output:
                        wr(' \\\n\t       {0}'.format(output), end='')

            def wr_gen_command(t):
                wr('\n\t@mkdir -p {0}'.format(self.gendir))
                wr('\t@echo "  GEN  {0}"'.format(t.name))
                command = t.generator.get_command(t.output, t.inputs)
                wr('\t@{0}'.format(command), end='')

            def wr_gen_target(t, additional_deps=list()):
                padding = (len(t.output) + 2) * ' '
                dependences = list()
                dependences.extend(t.inputs)
                dependences.extend(t.deps)
                dependences.extend(additional_deps)
                wr('{0}: Makefile'.format(t.output), end='')
                for d in dependences:
                    wr(' \\\n{0}{1}'.format(padding, d), end='')

            def wr_compile_target(t):
                bare_command = ['gcc', '-MM', '-MT', t.output, '-Igen', '-Isrc']
                dep_result = list()
                inputs = list()
                inputs.extend(t.inputs)
                inputs.extend(t.deps)
                for inp in inputs:
                    command = list(bare_command)
                    command.append(inp)
                    dep_str = get_command_output(command)
                    dep_list = [s for s in dep_str.split(' ')[2:] if len(s) > 4]
                    dep_result.extend(dep_list)
                wr_gen_target(t, dep_result)

            def wr_test_command(t):
                inputs = list()
                inputs.extend(t.inputs)
                inputs.extend(t.deps)
                wr('\n\t@mkdir -p {0}'.format(self.checkdir))
                wr('\t@echo "  CC   {0}"'.format(t.name))
                wr('\t@{0} {1} {2} -o {3} -I{4} -I{5} \\'
                    .format(self.cc, ' '.join(self.cflags),
                     ' '.join(self.oflags), t.output, self.srcdir, self.gendir))
                wr('\t      {0}'.format(' '.join(inputs)), end='')

            print('  >> Makefile')

            if len(self.all):
                wr('all: {0}\n'.format(' '.join(self.all)))
            wr('res/force:\n\nclean:\n\trm -rf doc {0} {1} {2} callgrind*\n'
                             .format(self.builddir, self.gendir, self.checkdir))
            if len(self.checks):
                wr('checks: {0}\ncheck: checks\n'
                   '\t@time (for c in checks/check*; do $$c; done)\n\n'
                   .format(' '.join(self.checks)))

            for t in self.targets:
                if isinstance(t, CompileTarget):
                    wr_compile_target(t)
                    wr_compile_command(t)
                elif isinstance(t, LinkTarget):
                    wr_gen_target(t)
                    wr_link_command(t)
                elif isinstance(t, GeneratedTarget):
                    wr_gen_target(t)
                    wr_gen_command(t)
                elif isinstance(t, TestTarget):
                    wr_compile_target(t)
                    wr_test_command(t)
                else:
                    print("Unknown process type!")
                wr('\n')

    #---------------------------------------------------------------------------
    # METHODS
    #---------------------------------------------------------------------------

    def absolutize_path(self, objects, directory):
        if isinstance(objects, str):
            return os.path.join(directory, objects)
        elif isinstance(objects, list):
            return [os.path.join(directory, str(o)) for o in objects]
        else:
            print("Wrong object format! ({0})".format(type(objects)))

    def validate_arguments(self, name, output, inputs, deps, pkgs):
        if output is None:
            raise Exception('No output path specified!')

        if not isinstance(name, str) and name is not None:
            raise Exception('Name must be passed as string! '
                            'Got {0}'.format(type(name)));

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

    #---------------------------------------------------------------------------

    def add_compile_target(self,
                           name=None,
                           output=None,
                           inputs=list(),
                           deps=list(),
                           pkgs=set()):
        try:
            self.validate_arguments(name, output, inputs, deps, pkgs)
        except Exception as e:
            print(e)
            return

        name   = name or output
        output = self.absolutize_path(output, self.builddir)
        inputs = self.absolutize_path(inputs, self.srcdir)
        deps   = self.absolutize_path(deps,   self.gendir)

        target = CompileTarget(name, output, inputs, deps=deps, pkgs=pkgs)
        self.targets.append(target)
        return target

    #---------------------------------------------------------------------------

    def add_link_target(self,
                        name=None,
                        output=None,
                        inputs=list(),
                        pkgs=set(),
                        include_in_all=False):
        try:
            self.validate_arguments(name, output, inputs, list(), pkgs)
        except Exception as e:
            print(e)
            return

        name   = name or output
        output = self.absolutize_path(output, self.builddir)
        inputs = self.absolutize_path(inputs, self.builddir)

        target = LinkTarget(name, output, inputs, pkgs=pkgs)
        self.targets.append(target)

        if include_in_all:
            self.all.append(output)

        return target

    #---------------------------------------------------------------------------

    def add_generated_target(self,
                             name=None,
                             generator=None,
                             output=None,
                             inputs=list(),
                             deps=list()):
        try:
            self.validate_arguments(name, output, inputs, deps, set())
        except Exception as e:
            print(e)
            return

        name   = name or output
        output = self.absolutize_path(output, self.gendir)
        inputs = self.absolutize_path(inputs, self.resdir)
        deps   = self.absolutize_path(deps,   self.resdir)

        target = GeneratedTarget(name, output, inputs,
                                 deps=deps, generator=generator)
        self.targets.append(target)
        return target

    #---------------------------------------------------------------------------

    def add_test(self,
                 name=None,
                 output=None,
                 inputs=list(),
                 deps=list()):
        try:
            self.validate_arguments(name, output, inputs, deps, set())
        except Exception as e:
            print(e)
            return

        name   = name or output
        output = self.absolutize_path(output, self.checkdir)
        inputs = self.absolutize_path(inputs, self.testdir)
        deps   = self.absolutize_path(deps,   self.srcdir)

        target = TestTarget(name, output, inputs, deps=deps)
        self.targets.append(target)
        self.checks.append(output)
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

    def set_oflags(self, flags):
        if isinstance(flags, list) or isinstance(flags, tuple):
            self.oflags = flags
        else:
            print('OFLAGS must be passed as list of strings!')

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
