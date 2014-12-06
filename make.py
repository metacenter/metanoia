#!/usr/bin/python
# file: make.py
# vim: tabstop=4 expandtab colorcolumn=81 list

import os.path

#-------------------------------------------------------------------------------

class Generator:
    """Represends file generating script or program"""

    command_body = None
    command_args = None
    shortcut = None

    def __init__(self, command_body, command_args, shortcut):
        self.command_body = command_body
        self.command_args = command_args
        self.shortcut = shortcut

#-------------------------------------------------------------------------------

class Target:
    """Represents target object"""

    proc = None
    output = None
    inputs = list()
    includes = list()
    pkgs = set()
    input_dir = str()
    output_dir = str()

    def __init__(self, output, inputs,
                 includes=list(), pkgs=set(),
                 input_dir=str(), output_dir=str()):
        self.output = output
        self.inputs = inputs
        self.includes = includes
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
    def __init__(self, output, inputs, generator=None,
                 input_dir=str(), output_dir=str()):
        Target.__init__(self, output, inputs,
                        input_dir=input_dir, output_dir=output_dir)
        self.generator = generator

#-------------------------------------------------------------------------------

class Make:

    #---------------------------------------------------------------------------
    # GLOBAL VARIABLES
    #---------------------------------------------------------------------------

    cc = 'gcc'
    wflags = list()
    lflags = list()

    builddir = 'build'
    gendir = 'gen'
    srcdir = 'src'

    all = list()
    targets = list()
    generators = list()

    #---------------------------------------------------------------------------
    # COMMANDS
    #---------------------------------------------------------------------------

    def run_makefile(self, args):
        with open('Makefile', 'w') as mf:
            def wr(string='', end='\n'):
                print(string, file=mf, end=end)

            def wr_compile(t, output_path, input_paths):
                wr('\n\t@mkdir -p $(builddir)')
                wr('\t@echo "  CC  {0}"'.format(t.output))
                wr('\t@$(CC) $(WFLAGS) -o {0} -I$(srcdir) -I$(gendir) \\' \
                    .format(output_path))
                wr('\t       -c {0}'.format(' '.join(input_paths)), end='')
                if len(t.pkgs):
                    wr(' \\\n\t       ', end='')
                    wr('`pkg-config --cflags {0}`' \
                       .format(' '.join(sorted(t.pkgs))), end='')

            def wr_link(t, output_path, input_paths):
                wr('\n\t@mkdir -p $(builddir)')
                wr('\t@echo "  LD  {0}"'.format(t.output))
                wr('\t@$(CC) $(LFLAGS) -o {0} \\' \
                    .format(output_path))
                wr('\t       {0}'.format(' '.join(input_paths)), end='')
                if len(t.pkgs):
                    wr(' \\\n\t       ', end='')
                    wr('`pkg-config --libs {0}`' \
                       .format(' '.join(sorted(t.pkgs))), end='')

            def wr_gen(t, output_path, input_paths):
                wr('\n\t@mkdir -p $(gendir)')
                wr('\t@echo "  GEN {0}"'.format(t.output))
                args = t.generator.command_args.format(output=output_path,
                                                inputs=' '.join(input_paths))
                wr('\t@$({0}) {1}'.format(t.generator.shortcut, args), end='')

            def make_path_list(objects, default_dir):
                result = list()
                for o in objects:
                    directory = default_dir

                    if isinstance(o, Target):
                        directory = o.output_dir

                    result.append(os.path.join(directory, str(o)))

                return result


            print('  >> Makefile')

            wr('CC={0}'.format(self.cc))
            for gen in self.generators:
                if gen.shortcut is not None and gen.command_body is not None:
                    wr('{0}={1}'.format(gen.shortcut, gen.command_body))
            wr()
            wr('WFLAGS={0}'.format(' '.join(self.wflags)))
            wr('LFLAGS={0}'.format(' '.join(self.lflags)))
            wr()
            wr('srcdir={0}'.format(self.srcdir))
            wr('builddir={0}'.format(self.builddir))
            wr('gendir={0}'.format(self.gendir))
            wr()
            if len(self.all):
                wr('all: {0}\n'.format(' '.join(self.all)))
            wr('clean:\n\trm -r $(builddir) $(gendir)\n\n')

            for t in self.targets:
                output_path = os.path.join(t.output_dir, t.output)
                padding = (len(output_path) + 2) * ' '

                wr('{0}: Makefile'.format(output_path), end='')

                input_paths    = make_path_list(t.inputs, t.input_dir)
                includes_paths = make_path_list(t.includes, t.input_dir)

                dependences = list()
                dependences.extend(input_paths)
                dependences.extend(includes_paths)
                for d in dependences:
                    wr(' \\\n{0}{1}'.format(padding, d), end='')

                if isinstance(t, CompileTarget):
                    wr_compile(t, output_path, input_paths)
                elif isinstance(t, LinkTarget):
                    wr_link(t, output_path, input_paths)
                elif isinstance(t, GeneratedTarget):
                    wr_gen(t, output_path, input_paths)
                else:
                    print("Unknown process type!")
                wr('\n')

    #---------------------------------------------------------------------------
    # METHODS
    #---------------------------------------------------------------------------

    def validate_arguments(self, output, inputs, includes, pkgs, input_dir):
        if output is None:
            raise Exception('No output path specified!')

        if not isinstance(output, str):
            raise Exception('Output path must be passed as string! '
                            'Got {0}'.format(type(output)));

        if  (not isinstance(inputs, list)) \
        and (not isinstance(inputs, tuple)):
            raise Exception('Input must be passed as list of strings!, '
                            'Got {0}'.format(type(inputs)));

        if  (not isinstance(includes, list)) \
        and (not isinstance(includes, tuple)):
            raise Exception('Includes must be passed as list of strings! '
                            'Got {0}'.format(type(includes)));

        if not isinstance(pkgs, set):
            raise Exception('Packages must be passed as set of strings! '
                            'Got {0}'.format(type(pkgs)))

        if not isinstance(input_dir, str):
            raise Exception('Input directory must be passed as string! '
                            'Got {0}'.format(type(input_dir)))

    #---------------------------------------------------------------------------

    def add_compile_target(self,
                           output=None,
                           inputs=list(),
                           includes=list(),
                           pkgs=set(),
                           input_dir='$(srcdir)',
                           output_dir='$(builddir)'):
        try:
            self.validate_arguments(output, inputs, includes, pkgs, input_dir)
        except Exception as e:
            print(e.message)
            return

        target = CompileTarget(output, inputs, includes=includes, pkgs=pkgs,
                               input_dir=input_dir, output_dir=output_dir)
        self.targets.append(target)
        return target

    #---------------------------------------------------------------------------

    def add_link_target(self,
                        output=None,
                        inputs=list(),
                        pkgs=set(),
                        input_dir='$(builddir)',
                        output_dir='$(builddir)',
                        include_in_all=False):
        try:
            self.validate_arguments(output, inputs, list(), pkgs, input_dir)
        except Exception as e:
            print(e.message)
            return

        target = LinkTarget(output, inputs, pkgs=pkgs,
                            input_dir=input_dir, output_dir=output_dir)
        self.targets.append(target)

        if include_in_all:
            self.all.append(os.path.join('$(builddir)', output))

        return target

    #---------------------------------------------------------------------------

    def add_generated_target(self,
                             generator=None,
                             output=None,
                             inputs=list(),
                             input_dir='',
                             output_dir='$(gendir)'):
        try:
            self.validate_arguments(output, inputs, list(), set(), input_dir)
        except Exception as e:
            print(e.message)
            return

        target = GeneratedTarget(output, inputs, generator=generator,
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

    def set_wflags(self, flags):
        if isinstance(flags, list) or isinstance(flags, tuple):
            self.wflags = flags
        else:
            print('WFLAGS must be passed as list of strings!')

    def set_lflags(self, flags):
        if isinstance(flags, list) or isinstance(flags, tuple):
            self.lflags = flags
        else:
            print('LFLAGS must be passed as list of strings!')

    def set_build_directory(self, path):
        if isinstance(path, str):
            self.builddir = path
        else:
            print('Build directory must be passed as string!')

    def set_gen_directory(self, path):
        if isinstance(path, str):
            self.gendir = path
        else:
            print('Gen directory must be passed as string!')

    def set_source_directory(self, path):
        if isinstance(path, str):
            self.srcdir = path
        else:
            print('Source directory must be passed as string!')

    #---------------------------------------------------------------------------
    # GETTERS
    #---------------------------------------------------------------------------

    def get_build_directory(self):
        return '$(builddir)', self.builddir

    def get_gen_directory(self):
        return '$(gendir)', self.gendir

    def get_source_directory(self):
        return '$(srcdir)', self.srcdir

#-------------------------------------------------------------------------------
