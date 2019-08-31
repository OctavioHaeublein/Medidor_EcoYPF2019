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
potencia_objetivo = []
variables_graficos = []
directorio = ""

#variables = {"1":Todo,"2":Tiempo,"3":Tension,"4":Corriente,"5":Potencia,"6":Energia, "7": Velocidad}

error = Fore.RED + Style.DIM
carga = Fore.YELLOW + Back.RESET
reset = Style.RESET_ALL
opcion = Fore.GREEN + Style.BRIGHT + Back.MAGENTA
init()

def titulo ():
    os.system('cls')
    print(opcion + Back.RESET + "------------------------------------------------------------------")
    print(pyfiglet.figlet_format("Eco - YPF 2019"))
    print("------------------------------------------------------------------" + reset)

def datos(directorio):
    
    titulo()
    
    if(directorio == ""):
        directorio = (pathlib.Path(__file__).parent / f'./datos/')
        x = 1
        directorios = []
        for i in directorio.iterdir():
            print(opcion + f"{x}." + carga + f" {i}" + reset)
            directorios.append(i)
            x += 1
        
        print(opcion + Back.RESET + "------------------------------------------------------------------" + reset)
        x = int(input("Introduzca el número de archivo: "))
        
        if((x > len(directorios)) or (x <= 0) or (x == (len(directorios)+1))):
            print(error + "Introduzca un valor válido para el rango de archivos" + reset)
            time.sleep(1)
            return
        else:
            directorio = directorios[x-1]
            
    titulo()

    print(opcion + "1." + Back.RESET + " Todo")
    print(opcion + "2." + Back.RESET + " Tensión")
    print(opcion + "3." + Back.RESET + " Corriente")
    print(opcion + "4." + Back.RESET + " Potencia")
    print(opcion + "5." + Back.RESET + " Energía")
    print(opcion + "6." + Back.RESET + " Velocidad")
    print(opcion + "7." + Back.RESET + " Volver")
    print("------------------------------------------------------------------" + reset)
    variables = input("Variables a graficar (X-Y): ")
    
    if(variables == '7'):
        return 0
    
    graficar(directorio, variables)
    
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
                potencia_objetivo.append(float(row['potencia_objetivo']))
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

    if(len(variables_graficos) >= 2):
        if((len(variables_graficos) == 2) ):
            cantidad_graficos = 1
        if((len(variables_graficos) < 5) and (len(variables_graficos) > 2)):
            cantidad_graficos = 2
        elif(len(variables_graficos) == 5):
            cantidad_graficos = 3
            
    y_grafico = []
    x = 1
    for i in range(len(variables_graficos)):
        
        if(variables_graficos[i]=="2"):
            y_grafico = tension
            label = 'Tension (V)'
        if(variables_graficos[i]=="3"):
            y_grafico = corriente
            label = 'Corriente (A)'
        if(variables_graficos[i]=="4"):
            y_grafico = potencia
            label = 'Potencia (W)'
        if(variables_graficos[i]=="5"):
            y_grafico = energia
            label = 'Energia (WH)'
        if(variables_graficos[i]=="6"):
            y_grafico = velocidad
            label = 'Velocidad (Km/h)'
            
        if(len(variables_graficos) == 1):
        
            fig,ax = plt.subplots()
            ax.plot(tiempo, y_grafico)
            ax.set(xlabel = 'Tiempo(h)', ylabel = label)
            ax.grid()
        
            if(variables_graficos[i] == '4'):
                ax.plot(tiempo,potencia_objetivo, dashes=[6,2],label = 'Potencia Objetivo')
                ax.legend()
        else:
            
            plt.subplot(cantidad_graficos, 2, x)
            plt.plot(tiempo, y_grafico)
            plt.ylabel(label)
            plt.xlabel('Tiempo (h)')
            if(variables_graficos[i] == '4'):
                plt.plot(tiempo,potencia_objetivo, dashes=[6,2],label = 'Potencia Objetivo')
                plt.legend()
            plt.grid()
            plt.tight_layout()
            x += 1
            
    plt.show()
    
    datos(directorio)
    