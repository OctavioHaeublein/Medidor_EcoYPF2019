import time
import matplotlib.pyplot as plt
import csv
import numpy as np
import pathlib
import pyfiglet
import os
from colorama import init, Style, Fore, Back

tiempo = []
tension = []
corriente = []
potencia = []
energia = []
velocidad = []
variables_graficos = []

#variables = {"1":Todo,"2":Tiempo,"3":Tension,"4":Corriente,"5":Potencia,"6":Energia, "7": Velocidad}

error = Fore.RED + Style.DIM
carga = Fore.YELLOW + Back.RESET
reset = Style.RESET_ALL
opcion = Fore.GREEN + Style.BRIGHT + Back.MAGENTA
init()

def datos():
    
    os.system('cls')
    print(opcion + Back.RESET + "------------------------------------------------------------------")
    print(pyfiglet.figlet_format("Eco - YPF 2019"))
    print("------------------------------------------------------------------" + reset)
    
    directorio = (pathlib.Path(__file__).parent / f'./datos/')
    x = 1
    directorios = []
    for i in directorio.iterdir():
        print(opcion + f"{x}." + carga + f" {i}")
        directorios.append(i)
        x += 1
    print(opcion + f"{x}." + carga +" Volver" + reset)
    
    x = int(input("Introduzca el número de archivo: "))
    
    if((x > len(directorios)) or (x <= 0) or (x == (len(directorios)+1))):
        print(error + "Introduzca un valor válido para el rango de archivos" + reset)
        time.sleep(1)
        return
    else:
        x = (x-1)
    
    os.system('cls')
    print(opcion + Back.RESET + "------------------------------------------------------------------")
    print(pyfiglet.figlet_format("Eco - YPF 2019"))
    print("------------------------------------------------------------------" + reset)

    print(opcion + "1." + Back.RESET + " Todo")
    print(opcion + "2." + Back.RESET + " Tensión")
    print(opcion + "3." + Back.RESET + " Corriente")
    print(opcion + "4." + Back.RESET + " Potencia")
    print(opcion + "5." + Back.RESET + " Energía")
    print(opcion + "6." + Back.RESET + " Velocidad")
    print("------------------------------------------------------------------" + reset)
    variables = input("Variables a graficar (X-Y): ")
 
    graficar(directorios[x], variables)
    
def graficar(directorio, variables):
        
    i = 0
    try:
        with open(directorio,'r') as csv_file:
            reader = csv.DictReader(csv_file)
            for row in reader:
                tiempo.append(float(row['tiempo']))
                tension.append(float(row['tension']))
                corriente.append(float(row['corriente']))
                potencia.append(float(row['potencia']))
                energia.append(float(row['energia']))
                velocidad.append(float(row['velocidad']))
                print(carga + f"-[{i}]-" + reset)
                i += 1
                #time.sleep(0.01)
            csv_file.close()

    except Exception as e:
        print(error + "Error cargando datos desde archivo 'cache.csv'.")
        print("Puede que se encuentre vacio.")
        print(e)
        print(reset)
        time.sleep(2)
        return 0
        
    if(variables == "1"):
        variables = "2 3 4 5 6"
        cantidad_graficos = 5
        print("Reasignando..")
        
    variables_graficos = variables.split(' ')
    cantidad_graficos = len(variables_graficos)
    
    if(cantidad_graficos > 1):
        fig, axs = plt.subplots(cantidad_graficos,1)
    
    y_grafico = []
    x = 0
    for i in range(len(variables_graficos)):
        
        if(variables_graficos[i]=="2"):
            y_grafico = tension
        if(variables_graficos[i]=="3"):
            y_grafico = corriente
        if(variables_graficos[i]=="4"):
            y_grafico = potencia
        if(variables_graficos[i]=="5"):
            y_grafico = energia
        if(variables_graficos[i]=="6"):
            y_grafico = velocidad
                
        if(cantidad_graficos == 1):
            plt.plot(tiempo,y_grafico)
            plt.grid()
        else:
            axs[x].plot(tiempo,y_grafico)
            axs[x].grid(True)
            x += 1
            
    plt.show()
    