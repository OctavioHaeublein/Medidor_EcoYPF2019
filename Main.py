import Archivo
import Graficos
import pyfiglet
import time
import datetime
import pathlib
import os
from colorama import init, Style, Fore, Back

init()
intro = Fore.GREEN + Style.BRIGHT + Back.RESET

def cargar ():
    os.system('cls')
    print(intro +  "------------------------------------------------------------------")
    print(pyfiglet.figlet_format("Eco - YPF 2019"))
    print("------------------------------------------------------------------")
    print(Style.RESET_ALL)
    directorio = input("Introduzca el directorio del archivo: ")
    Archivo.cargar(directorio)

while(True):
    os.system('cls')
    print(intro +  "------------------------------------------------------------------")
    print(pyfiglet.figlet_format("Eco - YPF 2019"))
    print("------------------------------------------------------------------")
    print(Back.MAGENTA + "1." + intro +" Cargar")
    print(Back.MAGENTA + "2." + intro + " Graficar")
    print(Back.MAGENTA + "3." + intro + " Salir")
    print("------------------------------------------------------------------" + Style.RESET_ALL)
    entrada = input("Opción: ")

    if(entrada == "1"):
        cargar()

    if(entrada == "2"):
        Graficos.datos()
            
    if(entrada == "3"):
        print("Cerrando...")
        time.sleep(1)
        quit()