import Archivo
import Graficos
import pyfiglet
import time
import datetime
import pathlib

def cargar ():
    directorio = input("Introduzca el directorio del archivo: ")
    Archivo.cargar(directorio)

while (True):
    print("----------------------------------------------------------------")
    print(pyfiglet.figlet_format("Eco YPF 2019"))
    print("----------------------------------------------------------------")
    print("1. Cargar")
    print("2. Graficar")
    print("3. Salir")
    print("----------------------------------------------------------------")
    entrada = input("Opción: ")

    if(entrada == "1"):
        cargar()

    if(entrada == "2"):
        print("----------------------------------------------------------------")
        print("1. Todo")
        print("2. Tiempo")
        print("3. Tensión")
        print("4. Corriente")
        print("5. Potencia")
        print("6. Energía")
        print("7. Velocidad")
        print("----------------------------------------------------------------")
        variables = input("Variables a graficar (X-Y): ")
        archivo = input("Introduzca el nombre del archivo: ")
        directorio = (pathlib.Path(__file__).parent / f'./datos/{archivo}')
        Graficos.graficar(variables, directorio)
    
    if(entrada == "3"):
        print("Cerrando...")
        time.sleep(1)
        quit()