import time
import matplotlib.pyplot as plt
import csv
import numpy as np

tiempo = []
tension = []
corriente = []
potencia = []
energia = []
velocidad = []
variables_graficos = []

#variables = {"1":Todo,"2":Tiempo,"3":Tension,"4":Corriente,"5":Potencia,"6":Energia, "7": Velocidad}

def graficar(variables,directorio):
    i = 0

    try:
        with open(directorio,'r') as csv_file:
            reader = csv.DictReader(csv_file)
            for row in reader:
                tiempo.append(int(row['tiempo']))
                tension.append(int(row['tension']))
                corriente.append(int(row['corriente']))
                potencia.append(int(row['potencia']))
                energia.append(int(row['energia']))
                velocidad.append(int(row['velocidad']))
                print(f"-[{i}]-")
                i += 1
                time.sleep(0.01)
            csv_file.close()

    except Exception as e:
        print("Error cargando datos desde archivo 'cache.csv'.")
        print("Puede que se encuentre vacio.")
        print(e)
        time.sleep(2)
        return 0
    
    if(variables == "1"):
        variables = "2 3 2 4 2 5 2 6 2 7"
        print("Reasignando..")
        
    variables_graficos = variables.split(' ')
        
    cantidad_graficos = int(len(variables_graficos)/2)
    if(cantidad_graficos > 1):
        fig, axs = plt.subplots(cantidad_graficos,1)
    
    x_grafico = []
    y_grafico = []
    x = 0
    for i in range(len(variables_graficos)):
        
        if(((i%2) == 0) or (i == 0)):
            if(variables_graficos[i]=="2"):
                x_grafico = tiempo
            if(variables_graficos[i]=="3"):
                x_grafico = tension
            if(variables_graficos[i]=="4"):
                x_grafico = corriente
            if(variables_graficos[i]=="5"):
                x_grafico = potencia
            if(variables_graficos[i]=="6"):
                x_grafico = energia
            if(variables_graficos[i]=="7"):
                x_grafico = velocidad
                
        if((i%2) != 0):
            if(variables_graficos[i]=="2"):
                y_grafico = tiempo
            if(variables_graficos[i]=="3"):
                y_grafico = tension
            if(variables_graficos[i]=="4"):
                y_grafico = corriente
            if(variables_graficos[i]=="5"):
                y_grafico = potencia
            if(variables_graficos[i]=="6"):
                y_grafico = energia
            if(variables_graficos[i]=="7"):
                y_grafico = velocidad
                
            if(cantidad_graficos == 1):
                plt.plot(x_grafico,y_grafico)
                plt.grid()
            else:
                axs[x].plot(x_grafico,y_grafico)
                axs[x].grid(True)
                x += 1
            
    #fig.tight_layout()
    plt.show()
    