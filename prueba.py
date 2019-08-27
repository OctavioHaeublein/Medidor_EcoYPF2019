import pathlib

path = pathlib.Path('F:\Proyectos\Medidor_EcoYPF2019\prueba')

for i in path.iterdir():
    print(i)