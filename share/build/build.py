#!/usr/bin/python3

# TODO comments

import os
import os.path
import shutil
import fnmatch
import subprocess

################################################################################
# Tools

def _warn(string):
    print(string)

class BuildException(Exception):
    pass

################################################################################
# Config

class Config:
    """ """

    def __init__(self):
        # development directories
        self.source_dir   = 'src'
        self.resource_dir = 'res'
        self.test_dir     = 'tests'

        # output directories
        self.gen_dir          = 'gen'
        self.intermediate_dir = 'inter'
        self.build_dir        = 'build'
        self.check_dir        = 'checks'
        self.install_dir      = 'install'

    def set_source_directory(self, v):
        self.source_dir = str(v)

    def set_resource_directory(self, v):
        self.resource_dir = str(v)

    def set_test_directory(self, v):
        self.test_dir = str(v)

    def set_gen_directory(self, v):
        self.gen_dir = str(v)

    def set_intermediate_directory(self, v):
        self.intermediate_dir = str(v)

    def set_build_directory(self, v):
        self.build_dir = str(v)

    def set_check_directory(self, v):
        self.check_dir = str(v)

    def set_install_directory(self, v):
        self.install_dir = str(v)

    def set_c_compiler(self, v):
        self.c_compiler = str(v)

    def set_oflags(self, v):
        self.oflags = v

    def set_cflags(self, v):
        self.cflags = v

    def set_lflags(self, v):
        self.lflags = v

    def add_oflags(self, v):
        self.oflags.extend(v)

    def add_cflags(self, v):
        self.cflags.extend(v)

    def add_lflags(self, v):
        self.lflags.extend(v)

    def get_output_directories(self):
        return sorted({self.gen_dir,
                       self.intermediate_dir,
                       self.build_dir,
                       self.check_dir,
                       self.install_dir})

#===============================================================================

class ConfigClangDebug(Config):
    def __init__(self):
        Config.__init__(self)
        self.c_compiler = 'clang'
        self.oflags     = ['-DDEBUG', '-g', '-O0']
        self.cflags     = ['-Wall', '-W', '-Wextra', '-Werror']
        self.lflags     = []

#===============================================================================

class ConfigClangRelease(Config):
    def __init__(self):
        Config.__init__(self)
        self.c_compiler = 'clang'
        self.oflags     = ['-DNDEBUG', '-O3']
        self.cflags     = ['-Wall', '-W', '-Wextra', '-Werror']
        self.lflags     = []

#===============================================================================

class ConfigGccDebug(Config):
    def __init__(self):
        Config.__init__(self)
        self.c_compiler = 'gcc'
        self.oflags     = ['-DDEBUG', '-g', '-O0']
        self.cflags     = ['-Wall', '-W', '-Wextra', '-Wpedantic', '-Werror']
        self.lflags     = []

#===============================================================================

class ConfigGccRelease(Config):
    def __init__(self):
        Config.__init__(self)
        self.c_compiler = 'gcc'
        self.oflags     = ['-DNDEBUG', '-O3']
        self.cflags     = ['-Wall', '-W', '-Wextra', '-Wpedantic', '-Werror']
        self.lflags     = []

################################################################################
# Install config

class InstallConfig:
    def __init__(self, directory='/opt', mod=0o555):
        self.directory = directory
        self.mod =  mod

#===============================================================================

class InstallConfigExec(InstallConfig):
    def __init__(self):
        InstallConfig.__init__(self, directory='usr/bin', mod=0o755)

################################################################################
# Run command

class RunResult:
    """ """

    def __init__(self, returncode, stdout, stderr):
        self.returncode = int(returncode)
        self.stdout = str(stdout)
        self.stderr = str(stderr)

#===============================================================================

class Run:
    """ """

    @staticmethod
    def command(command, pipe=True):
        if pipe:
            completed = subprocess.run(command, stdout=subprocess.PIPE,
                                                stderr=subprocess.PIPE)
        else:
            completed = subprocess.run(command)

        return RunResult(completed.returncode,
                         completed.stdout.decode("utf-8")[:-1] \
                         if completed.stdout is not None else '',
                         completed.stderr.decode("utf-8")[:-1] \
                         if completed.stderr is not None else '')

    @staticmethod
    def get_included_headers(command):
        headers = set()

        cmd = list(command)
        cmd.append('-MM')
        result = Run.command(cmd)

        if result.returncode == 0:
            for h in result.stdout.split(' ')[2:]:
                if  len(h) > 4 \
                and not fnmatch.fnmatch(h, '/*'):
                    headers.add(h)
        else:
            _warn('Compiler error for command {command}:'
                  .format(command=cmd))
            _warn(result.stderr)

        return headers

    @staticmethod
    def get_pflags(pkgs, option):
        flags = set()

        command = ['pkg-config', option]
        command.extend(pkgs)

        result = Run.command(command)
        if result.returncode == 0:
            if len(result.stdout) > 3:
                flags = {s for s in result.stdout.split(' ')}
        else:
            _warn('pkg-config error:')
            _warn(result.stderr)

        return flags

################################################################################
# Generator

class Generator:
    """Represents file generating script or program"""

    def __init__(self, short, command):
        self._short = short
        self._command = tuple(command)

    def get_command(self, output_path=None, input_paths=None):
        output = output_path if output_path is not None else ''
        inputs = ' '.join(input_paths) if input_paths is not None else ''
        return [item.format(output=output, inputs=inputs)
                for item in self._command]

    def get_command_string(self, output_path=None, input_paths=tuple()):
        return ' '.join(self.get_command(output_path, input_paths))

    def get_short(self):
        return self._short

################################################################################
# Project

def _assert_target_uniqueness(method):
    def inner(him, target):
        if him._accept(target):
            method(him, target)
        him._targets[target.get_output()] = target
    return inner

#-------------------------------------------------------------------------------

class Project:
    """ """

    def __init__(self, config):
        self._config = config
        self._gen_targets = list()
        self._compiled_targets = list()
        self._test_targets = list()
        self._executable_targets = list()
        self._check_targets = list()

        self._default = list()
        self._targets = dict()
        self._phonies = dict()

        self._clean_commands = list()
        self.add_clean_commands(config.get_output_directories())

    def add(self, targets, include_in_default=False):
        if not isinstance(targets, list) and not isinstance(targets, tuple):
            targets = [targets]

        for t in targets:
            t.set_config(self._config)
            t._add_to_project(self)
            if include_in_default:
                self._default.append(t)

    def _accept(self, target):
        if target.get_output() in self._targets.keys():
            if target != self._targets[target.get_output()]:
                _warn("Redefinition of target '{target}'".format(target=target))
            return False

        return True;

#-------------------------------------------------------------------------------

    def get_config(self):
        return self._config

    def get_phony_targets(self):
        return [self._phonies[k] for k in sorted(self._phonies.keys())]

    def get_default_targets(self):
        return self._default

    def get_generated_targets(self):
        return self._gen_targets

    def get_compiled_targets(self):
        return self._compiled_targets

    def get_test_targets(self):
        return self._test_targets

    def get_executable_targets(self):
        return self._executable_targets

    def get_check_targets(self):
        return self._check_targets

    def get_all_targets(self):
        all = list()
        all.extend(self._executable_targets)
        all.extend(self._check_targets)
        return all

    def add_clean_commands(self, commands):
        self._clean_commands.extend(commands)

    def get_clean_commands(self):
        return self._clean_commands

#-------------------------------------------------------------------------------

    @_assert_target_uniqueness
    def _add_generated(self, target):
        self._gen_targets.append(target)

    @_assert_target_uniqueness
    def _add_compiled(self, target):
        self._compiled_targets.append(target)

    @_assert_target_uniqueness
    def _add_test(self, target):
        self._test_targets.append(target)

    @_assert_target_uniqueness
    def _add_executable(self, target):
        self._executable_targets.append(target)

    @_assert_target_uniqueness
    def _add_check(self, target):
        self._check_targets.append(target)
        for item in target.get_inputs():
            item.set_config(self._config)
            item._add_to_project(self)

    @_assert_target_uniqueness
    def _add_static(self, target):
        pass

    @_assert_target_uniqueness
    def _add_shared(self, target):
        pass

    def _add_phony(self, target):
        if target.get_generator_short() not in self._phonies:
            self._phonies[target.get_generator_short()] = target
        else:
            _warn('Phony "{phony}" already defined!'
                  .format(target.get_generator_short))

################################################################################
# Target

class Target:
    """If `inputs` id None - force"""

    def __init__(self, inputs, output, deps, pkgs, libs, alias):
        self._input_dir = None
        self._output_dir = None

        if output is None:
            if inputs is None:
                pass
            elif len(inputs) == 1:
                output_split = str(inputs[0]).split('.')[:-1]
                output_split.append('o')
                self._output = '.'.join(output_split)
            else:
                raise BuildException('No output specified!')
        else:
            self._output = str(output)

        self._inputs = None if inputs is None else list(inputs)
        self._deps = None if deps is None else list(deps)
        self._pkgs = set() if pkgs is None else set(pkgs)
        self._libs = set() if libs is None else set(libs)
        self._alias = alias

    def __str__(self):
        return str(self._output)

    def get_output_name(self):
        return self._output

    def get_output(self):
        if self._alias is not None:
            result = self._alias
        elif self._output_dir is None:
            _warn('Target "{output}" has not output specified! '
                  'Did you add it to project?'.format(output=self._output))
            result = self._output
        else:
            result = os.path.join(self._output_dir, str(self._output))

        return result

    def get_output_dir(self):
        return self._output_dir

    def get_input_paths(self):
        if self._inputs is not None:
            return list(item.get_output() if isinstance(item, Target)
                        else os.path.join(self._input_dir, str(item))
                        for item in self._inputs)
        else:
            return None

    def get_inputs(self):
        return self._inputs if self._inputs is not None else list()

    def get_deps(self):
        if self._deps is not None:
            return list(item.get_output() if isinstance(item, Target)
                        else os.path.join(self._deps_dir, str(item))
                        for item in self._deps)
        else:
            return None

    def get_pkgs(self):
        return self._pkgs if self._pkgs is not None else set()

    def get_libs(self):
        return self._libs if self._libs is not None else set()

    def get_command_string(self, **kwargs):
        return ' '.join(self.get_command(**kwargs))

    def _fetch_pflags(self, option):
        if  self._pflags is None \
        and self._pkgs is not None \
        and (len(self._pkgs) > 0):
            self._pflags = Run.get_pflags(self._pkgs, option)

        return self._pflags if self._pflags is not None else set()

#===============================================================================

class PhonyTarget(Target):
    """ """

    def __init__(self, generator, deps=None):
        Target.__init__(self, None, None, deps, None, None, None)

        if isinstance(generator, Generator):
            self._gen = generator
        else:
            _warn('Argument is not generator!');

    def set_config(self, config):
        self._deps_dir = ''

    def get_generator_short(self):
        return self._gen.get_short()

    def get_command(self, output_path=None, input_paths=None):
        return self._gen.get_command(output_path, input_paths)

    def _add_to_project(self, project):
        project._add_phony(self);

#===============================================================================

class GeneratedTarget(Target):
    """ """

    def __init__(self, generator, output, inputs=None, deps=None, alias=None):
        Target.__init__(self, inputs, output, deps, None, None, alias)

        if isinstance(generator, Generator):
            self._gen = generator
        else:
            _warn('Argument is not generator!');

    def set_config(self, config):
        self._input_dir = config.resource_dir
        self._output_dir = config.gen_dir
        self._deps_dir = config.resource_dir

    def get_generator_short(self):
        return self._gen.get_short()

    def get_command(self, output_path=None,
                          input_paths=None):
        output_path = output_path if output_path is not None \
                                  else self.get_output()

        input_paths = [input_paths] if input_paths is not None \
                                    else self.get_input_paths()

        return self._gen.get_command(output_path=output_path,
                                     input_paths=input_paths)

    def get_deps(self):
        deps = None
        if self._inputs is not None or self._deps is not None:
            deps = list()
            if self._inputs is not None:
                deps.extend(self.get_input_paths())
            if self._deps is not None:
                deps.extend(Target.get_deps(self))
            deps.sort()
        return deps

    def _add_to_project(self, project):
        project._add_generated(self);

#===============================================================================

class CompileTarget(Target):
    """ """

    def __init__(self, inputs=tuple(), output=None,
                 deps=None, pkgs=None, libs=None):
        Target.__init__(self, inputs, output, deps, pkgs, libs, None)

    def set_config(self, config):
        self._input_dir = config.source_dir
        self._output_dir = config.intermediate_dir
        self._dep_dir = config.intermediate_dir
        self._include_dirs = [config.source_dir, config.gen_dir]
        self._compiler = config.c_compiler;
        self._cflags = config.cflags;
        self._oflags = config.oflags;
        self._pflags = None

    def get_command(self, with_output=True, with_inputs=True):
        command = [self._compiler]
        command.extend(self._oflags)
        command.extend(self._cflags)
        command.append('-c')
        command.extend(self.get_cflags())

        if with_output:
            command.extend(['-o', self.get_output()])

        if with_inputs:
            command.extend(self.get_input_paths())

        return command

    def get_deps(self, included=True):
        deps = set()
        deps.update(self.get_input_paths())

        if self._deps is not None:
            deps.update(d.get_output() for d in self._deps)

        if included:
            cmd = self.get_command(with_output=False)
            deps.update(Run.get_included_headers(cmd))

        return sorted(deps)

    def get_cflags(self, with_local=True, with_external=True):
        flags = list()

        if with_local:
            flags.extend(['-I{d}'.format(d=d) for d in self._include_dirs])

        if with_external:
            flags.extend(sorted(self._fetch_pflags('--cflags')))

        return flags

    def _add_to_project(self, project):
        project._add_compiled(self);
        project.add([item for item in self._inputs
                     if hasattr(item, '_add_to_project')])

    @staticmethod
    def from_matching(match_str, config):
        files = sorted(list(f for f in os.listdir(config.source_dir)
                       if fnmatch.fnmatch(f, match_str)))
        return list(CompileTarget([f]) for f in files)

#===============================================================================

class TestTarget(CompileTarget):
    """ """

    def __init__(self, inputs=tuple(), output=None):
        CompileTarget.__init__(self, inputs, output)

    def set_config(self, config):
        self._input_dir = config.test_dir
        self._output_dir = config.intermediate_dir
        self._include_dirs = [config.source_dir, config.test_dir]
        self._compiler = config.c_compiler;
        self._cflags = config.cflags;
        self._oflags = config.oflags;
        self._pflags = None

    def _add_to_project(self, project):
        project._add_test(self);

#===============================================================================

class ExecutableTarget(Target):
    """ """

    def __init__(self, inputs=tuple(), output=None):
        Target.__init__(self, inputs, output, None, None, None, None)
        self._targets = list()
        self._libs = set()

    def set_config(self, config):
        self._input_dir = config.intermediate_dir
        self._output_dir = config.build_dir
        self._linker = config.c_compiler;
        self._lflags = config.lflags;
        self._oflags = config.oflags;
        self._pflags = None

    def get_command(self, with_output=True, with_inputs=True):
        command = [self._linker]
        command.extend(self._oflags)
        command.extend(self._lflags)

        if with_output:
            command.extend(self.get_lflags())
            command.extend(['-o', self.get_output()])

        if with_inputs:
            command.extend(self.get_input_paths())

        return command

    def get_lflags(self):
        lflags = set()
        lflags.update(self._fetch_pflags('--libs'))
        lflags.update('-l{lib}'.format(lib=lib) for lib in self._libs)
        return sorted(lflags)

    def add(self, targets):
        self._targets.extend(targets)
        self._inputs.extend(targets)
        for t in targets:
            self._pkgs.update(t.get_pkgs())
            self._libs.update(t.get_libs())

    def _add_to_project(self, project):
        project._add_executable(self)
        project.add(self._targets)

#===============================================================================

class CheckTarget(ExecutableTarget):
    """ """

    def __init__(self, inputs=tuple(), output=None):
        ExecutableTarget.__init__(self, inputs, output)

    def set_config(self, config):
        self._input_dir = config.intermediate_dir
        self._output_dir = config.check_dir
        self._linker = config.c_compiler;
        self._lflags = config.lflags;
        self._oflags = config.oflags;
        self._pflags = None

    def _add_to_project(self, project):
        project._add_check(self);
        project.add(self._targets)

################################################################################
# Installator

class Installer:
    """ """

    def __init__(self, config):
        self.prefix = config.install_dir \
                   if config.install_dir is not None else '/'

    def __call__(self, target, install_config):
        directory = install_config.directory \
                 if install_config.directory is not None else ''

        dst_directory = os.path.join(self.prefix, directory)
        dst_path = os.path.join(dst_directory, target.get_output_name())
        src_path = target.get_output()

        print('  CP   "{src}" > "{dst}"'.format(src=src_path, dst=dst_path))

        if os.path.isfile(src_path):
            try:
                os.makedirs(dst_directory)
            except FileExistsError:
                pass
            shutil.copyfile(src_path, dst_path)
            os.chmod(dst_path, install_config.mod)

################################################################################
# Writer base

class Writer:
    """ """

    def __init__(self, build_file_path):
        self.build_file_path = build_file_path

    def wr(self, value):
        self.out.write(str(value))

    def write(self, project):
        try:
            if not isinstance(project, Project):
                raise BuildException('Writer argument should be project!')

            with open(self.build_file_path, 'w') as out:
                self.out = out
                self._write(project)
                del self.out

        except BuildException as e:
            _warn(e)

#===============================================================================
# Ninja writer

class NinjaWriter(Writer):
    """ """

    def __init__(self):
        Writer.__init__(self, 'build.ninja')
        self._printed_rules = set()

    def _write(self, project):
        c = project.get_config()
        self._wr_main_targets(c.c_compiler, c.c_compiler,
                              c.cflags, c.lflags, c.oflags,
                              project.get_all_targets(),
                              project.get_check_targets(),
                              project.get_clean_commands())

        for item in project.get_generated_targets():
            self._wr_generated_rule(item)

        for item in project.get_phony_targets():
            self._wr_phony_rule(item)
            self._wr_phony_build(item)

        for item in project.get_executable_targets():
            self._wr_executable_build(item)

        for item in project.get_check_targets():
            self._wr_executable_build(item)

        for item in project.get_generated_targets():
            self._wr_generated_build(item)

        for item in project.get_compiled_targets():
            self._wr_compiled_build(item)

        for item in project.get_test_targets():
            self._wr_compiled_build(item)

        self._wr_default_targets(project.get_default_targets())

    def _wr_rule(self, prefix, name, command=None):
        if name not in self._printed_rules:
            self.wr('rule {name}\n'.format(name=name))
            self.wr('    description = {prefix} $out\n'
                    .format(prefix=prefix))

            if command is not None:
                self.wr('    command = {command}\n'.format(command=command))

            self.wr('\n')

            self._printed_rules.add(name)

    def _wr_build(self, name, output, inputs=None, deps=tuple(),
                  cflags=tuple(), lflags=tuple()):
        if deps is None:
            deps = ' force'
        elif len(deps) == 0:
            deps = ''
        else:
            deps = ' {deps}'.format(deps=' '.join(deps))

        self.wr('build {output}: {name}{deps}\n'
                .format(output=output, name=name, deps=deps))

        if inputs is not None and len(inputs) > 0:
            self.wr('    inputs = {inputs}\n'.format(inputs=' '.join(inputs)))

        if len(cflags) > 0:
            self.wr('    cflags = {cflags}\n'.format(cflags=' '.join(cflags)))

        if len(lflags) > 0:
            self.wr('    lflags = {lflags}\n'.format(lflags=' '.join(lflags)))

        self.wr('\n')

    def _wr_main_targets(self, compiler, linker,
                         cflags, lflags, oflags,
                         all, checks, clean):
        cflagsstr = ' '.join(cflags)
        lflagsstr = ' '.join(lflags)
        oflagsstr = ' '.join(oflags)
        all_str = ' '.join(t.get_output() for t in all)
        checks_str = ' '.join(t.get_output() for t in checks)
        check_str = './' + '; ./'.join(t.get_output() for t in checks)

        self.wr('build all: phony {all}\n\n'.format(all=all_str))
        self.wr('build force: phony\n\n')
        self.wr('build checks: phony {checks}\n\n'.format(checks=checks_str))

        self.wr('rule check\n'
                '    description = run unit tests\n'
                '    command = {check}\n\n'
                'build check: check force checks\n\n'
                .format(check=check_str))
        self.wr('rule clean\n'
                '    description = clean\n'
                '    command = rm -rf {clean}\n\n'
                'build clean: clean\n\n'
                .format(clean=' '.join(clean)))

        self.wr('rule cc\n    deps = gcc\n    depfile = $out.d\n'
                '    description = CC   $out\n'
                '    command = {compiler} {oflags} {cflags} $cflags '
                '-MMD -MF $out.d -o $out -c $inputs\n\n'
                .format(compiler=compiler, oflags=oflagsstr, cflags=cflagsstr))
        self.wr('rule ld\n    description = LD   $out\n    command = '
                '{linker} {oflags} {lflags} $lflags -o $out $in\n\n'
                .format(linker=linker, oflags=oflagsstr, lflags=lflagsstr))

    def _wr_default_targets(self, defaults):
        defaults_str = ' '.join(t.get_output() for t in defaults)
        self.wr('default {defaults}\n\n'.format(defaults=defaults_str))

    def _wr_phony_rule(self, t):
        self._wr_rule('', t.get_generator_short(), t.get_command_string())

    def _wr_generated_rule(self, t):
        cmd = t.get_command_string(output_path='$out', input_paths='$inputs')
        self._wr_rule('GEN ', t.get_generator_short(), cmd)

    def _wr_phony_build(self, t):
        self._wr_build(t.get_generator_short(), t.get_generator_short(),
                       deps=t.get_deps())

    def _wr_generated_build(self, t):
        self._wr_build(t.get_generator_short(), t.get_output(),
                       inputs=t.get_input_paths(), deps=t.get_deps())

    def _wr_compiled_build(self, t):
        self._wr_build('cc', t.get_output(), inputs=t.get_input_paths(),
                       deps=t.get_deps(included=False), cflags=t.get_cflags())

    def _wr_executable_build(self, t):
        self._wr_build('ld', t.get_output(), deps=t.get_input_paths(),
                       lflags=t.get_lflags())

#===============================================================================
# Make writer

class MakeWriter(Writer):
    """ """

    def __init__(self):
        Writer.__init__(self, 'Makefile')

    def _write(self, project):
        self._wr_main_targets(project.get_default_targets(),
                              project.get_all_targets(),
                              project.get_check_targets(),
                              project.get_clean_commands())

        for item in project.get_phony_targets():
            self._wr_phony_target(item)

        for item in project.get_executable_targets():
            self._wr_executable_target(item)

        for item in project.get_check_targets():
            self._wr_executable_target(item)

        for item in project.get_generated_targets():
            self._wr_generated_target(item)

        for item in project.get_compiled_targets():
            self._wr_compiled_target(item)

        for item in project.get_test_targets():
            self._wr_compiled_target(item)

    def _wr_rule(self, prefix, output, outputdir,
                 inputs=tuple(), deps=None, command_str=None):
        self.wr(output)
        self.wr(': ')

        if deps is not None:
            padding = (len(str(output)) + 2) * ' '
            self.wr(self.build_file_path)
            for item in deps:
                self.wr(' \\\n')
                self.wr(padding)
                self.wr(item)
        else:
            self.wr('force')

        if outputdir is not None:
            self.wr('\n\t@mkdir -p {outputdir}\n\t@echo " {prefix} {output}"'
                .format(outputdir=outputdir, prefix=prefix, output=str(output)))

        if command_str is not None:
            self.wr('\n\t@')
            self.wr(command_str)
            if inputs is not None:
                for item in inputs:
                    self.wr(' \\\n\t    {item}'.format(item=item))

        self.wr('\n\n')

    def _wr_main_targets(self, defaults, all, checks, clean):
        defaults_str = ' '.join(t.get_output() for t in defaults)
        all_str = ' '.join(t.get_output() for t in all)
        checks_str = ' '.join(t.get_output() for t in checks)
        check_str = '\t@./' + '\n\t@./'.join(t.get_output() for t in checks)

        self.wr('default: {defaults}\n\n'.format(defaults=defaults_str))
        self.wr('all: {all}\n\n'.format(all=all_str))
        self.wr('checks: {checks}\n\n'.format(checks=checks_str))
        self.wr('check: checks\n{check}\n\n'.format(check=check_str))
        self.wr('clean:\n\trm -rf {clean}\n\n'
                .format(clean=' '.join(clean)))
        self.wr('force:\n\n')

    def _wr_phony_target(self, t):
        self._wr_rule('', t.get_generator_short(), None, inputs=None,
                      deps=t.get_deps(), command_str=t.get_command_string())

    def _wr_generated_target(self, t):
        self._wr_rule(' GEN ', t.get_output(), t.get_output_dir(),
                      None, t.get_deps(), t.get_command_string())

    def _wr_compiled_target(self, t):
        self._wr_rule(' CC  ', t.get_output(), t.get_output_dir(),
                      t.get_input_paths(), t.get_deps(),
                      t.get_command_string(with_inputs=False))

    def _wr_executable_target(self, t):
        self._wr_rule(' LD  ', t.get_output(), t.get_output_dir(),
                      t.get_input_paths(), t.get_input_paths(),
                      t.get_command_string(with_inputs=False))

################################################################################

