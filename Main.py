import Archivo
import Graficos
import pyfiglet
import time
import datetime
import pathlib
import os
from colorama import init, Style, Fore, Back

error = Fore.RED + Style.DIM
carga = Fore.YELLOW + Back.RESET
reset = Style.RESET_ALL
opcion = Fore.GREEN + Style.BRIGHT + Back.MAGENTA
init()

def cargar ():
    titulo()
    directorio = input("Introduzca el directorio del archivo: ")
    Archivo.cargar(directorio)

def titulo ():
    os.system('cls')
    print(opcion + Back.RESET + "------------------------------------------------------------------")
    print(pyfiglet.figlet_format("Eco - YPF 2019"))
    print("------------------------------------------------------------------" + reset)
    
while(True):
    titulo()
    print(opcion + "1." + Back.RESET +" Cargar")
    print(opcion + "2." + Back.RESET + " Graficar")
    print(opcion + "3." + Back.RESET + " Salir")
    print("------------------------------------------------------------------" + reset)
    entrada = input("Opción: ")

    if(entrada == "1"):
        cargar()

    if(entrada == "2"):
        Graficos.datos("")
            
    if(entrada == "3"):
        print("Cerrando...")
        time.sleep(0.5)
        quit()