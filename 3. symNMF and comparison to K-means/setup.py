from setuptools import Extension, setup

module = Extension("mySymNmf", sources=['symnmf.c', 'symnmfmodule.c'])
setup(name='mySymNmf',
     version='1.0',
     description='Python wrapper for custom C extension',
     ext_modules=[module])