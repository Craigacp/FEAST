import subprocess
import sys
from setuptools import setup
from setuptools.command.install import install


class FEASTInstall(install):
    def run(self):
        if sys.platform.startswith('win'):
            subprocess.run(['make', 'x64_win', 'install'])
        else:
            subprocess.run(['make', 'install'])
        install.run(self)


setup(
    name='feast',
    version='2.1.0',
    packages=['feast'],
    url='https://github.com/Craigacp/FEAST',
    license='BSD',
    author='Adam Pocock',
    author_email='adam@adampocock.com',
    description='An information theoretic feature selection toolbox.',
    install_requires=['numpy', 'pandas', 'scikit-learn'],
    cmd_class={'install': FEASTInstall},
)
