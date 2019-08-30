import time
import matplotlib.pyplot as plt
import csv
import numpy as np
import pathlib

tiempo = []
tension = []
corriente = []
potencia = []
energia = []
velocidad = []
variables_graficos = []

#variables = {"1":Todo,"2":Tiempo,"3":Tension,"4":Corriente,"5":Potencia,"6":Energia, "7": Velocidad}

def datos():
    
    directorio = (pathlib.Path(__file__).parent / f'./datos/')
    x = 1
    directorios = []
    for i in directorio.iterdir():
        print(f"{x}. {i}")
        directorios.append(i)
        x += 1
    print(f"{x}. Volver")
    
    x = int(input("Introduzca el número de archivo: "))
    
    if(x == (len(directorios)+1)):
        return 0
    else:
        x = (x-1)
    
    print("----------------------------------------------------------------")
    print("1. Todo")
    print("2. Tensión")
    print("3. Corriente")
    print("4. Potencia")
    print("5. Energía")
    print("6. Velocidad")
    print("----------------------------------------------------------------")
    variables = input("Variables a graficar (X-Y): ")
    
    if(x > len(directorios) or x <= 0 ):
        print("Introduzca un valor válido para el rango de archivos")
        time.sleep(1.5)
        datos()
        
    graficar(directorios[x], variables)
    
def graficar(directorio, variables):
    i = 0
    try:
        with open(directorio,'r') as csv_file:
            reader = csv.DictReader(csv_file)
            print("Puede que se encuentre vacio.")
            for row in reader:
                tiempo.append(float(row['tiempo']))
                tension.append(float(row['tension']))
                corriente.append(float(row['corriente']))
                potencia.append(float(row['potencia']))
                energia.append(float(row['energia']))
                velocidad.append(float(row['velocidad']))
                print(f"-[{i}]-")
                i += 1
                #time.sleep(0.01)
            csv_file.close()

    except Exception as e:
        print("Error cargando datos desde archivo 'cache.csv'.")
        print("Puede que se encuentre vacio.")
        print(e)
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
    