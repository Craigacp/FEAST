from setuptools import setup, dist, Extension
from setuptools.command.install import install
from os.path import dirname, abspath, join, exists
from os import system
import sys
import platform


base_dir = dirname(abspath(__file__))
mitoolbox_dir = mitoolbox_dir = join(base_dir, "MIToolbox")
feast_dir = base_dir
readme_path = join(base_dir, "README.md")


def CreateDependencies():

    #if not exists(mitoolbox_dir):
    #    system("git submodule init")
    #system("git submodule update --remote")

    if not exists(mitoolbox_dir):
        sys.exit(1)

    build_args = ""
    if "windows" in platform.system().lower():
        build_args = " x64_win"

    system("export LD_LIBRARY_PATH=/usr/local/lib")
    system("cd {} && make{} && cd {}".format(mitoolbox_dir, build_args, base_dir))
    system("make{}".format(build_args))


class BinaryDistribution(dist.Distribution):
    def has_ext_modules(foo):
        return True


def run_setup():

    CreateDependencies()

    with open(readme_path, "r") as readme_file:
        readme_data = readme_file.read()

    setup (
        name='fstoolbox',
        packages=['fstoolbox'],
        version='0.0.2',
        author='Adam Pocock',
        author_email='adam@adampocock.com',
        maintainer='Brent Barbachem',
        maintainer_email='barbacbd@gmail.com',
        url = 'https://github.com/Craigacp/FEAST',
        description = 'An information theoretic feature selection toolbox.',
        long_description_content_type='text/markdown',
        long_description = readme_data,
        classifiers=[
            'Intended Audience :: Developers',
            'Intended Audience :: Education',
            'Intended Audience :: Other Audience',
            'Intended Audience :: Science/Research',
            'Natural Language :: English',
            'Programming Language :: Python :: 3',
            'Programming Language :: Python :: 3.9',
            'Programming Language :: Python :: 3.10',
            'Programming Language :: Python :: 3.11',
            'Topic :: Software Development'
        ],
        keywords=['feast', 'feature', 'selection', 'fstoolbox'],
        license='MIT',
        zip_safe=False,
        python_requires='>=3.9, <4',
        install_requires=[
            'numpy',
            'pandas',
            'scikit-learn'
        ],
        distclass=BinaryDistribution,
        include_package_data=True,
        package_data={'': [
            join(mitoolbox_dir, "libMIToolbox.so"),
            join(feast_dir, "libFSToolbox.so")
        ]},
        ext_modules=[
            Extension(name="MIToolbox", sources=[]),
            Extension(name="FSToolbox", sources=[])
        ]
    )


if __name__ == '__main__':
    # Let setuptools/setup handle the setup.cfg data
    run_setup()
