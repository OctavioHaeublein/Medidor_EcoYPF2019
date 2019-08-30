import Archivo
import Graficos
import pyfiglet
import time
import datetime
import pathlib

def cargar ():
    directorio = input("Introduzca el directorio del archivo: ")
    Archivo.cargar(directorio)

while(True):

    print("----------------------------------------------------------------")
    print(pyfiglet.figlet_format("Eco - YPF 2019"))
    print("----------------------------------------------------------------")
    print("1. Cargar")
    print("2. Graficar")
    print("3. Salir")
    print("----------------------------------------------------------------")
    entrada = input("Opción: ")

    if(entrada == "1"):
        cargar()

    if(entrada == "2"):
        Graficos.datos()
            
    if(entrada == "3"):
        print("Cerrando...")
        time.sleep(1)
        quit()
      