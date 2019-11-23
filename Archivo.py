import csv
import time
import pathlib
import datetime
import os
from colorama import init, Style, Fore, Back

error = Fore.RED + Style.DIM
carga = Fore.YELLOW
reset = Style.RESET_ALL
init()

def cargar(archivo):
    
    try:
        os.system('cls')
        directorio = pathlib.Path(archivo)

    except Exception as e:
        print(error + "Error con el formato")
        print(e)
        print(reset)
        time.sleep(2)
        return 0
    
    
    for x in directorio.iterdir():
        tiempo_programa = []
        tension = []
        corriente = []
        potencia = []
        energia = []
        soc = []
        velocidad = []
        corriente_objetivo = []
        tiempo_objetivo = []
        variables_graficos = []

        i = 1
        print(carga + "Abriendo..." + reset)
        try:
            with open(x, 'r') as csv_file:
                reader = csv.DictReader(csv_file)
                largo = sum(2 for row in reader)
            csv_file.close()

        except Exception as e:
            print(error + f"Error cargando datos desde: {x}")
            print("Puede que se encuentre vacio.")
            print(e)
            print(reset)
            time.sleep(2)
            return 0

        try:
            with open(x, 'r') as csv_file:
                reader = csv.DictReader(csv_file)
                for row in reader:
                    tiempo_programa.append(row['tiempo'])
                    tension.append(row['tension'])
                    corriente.append(row['corriente'])
                    potencia.append(row['potencia'])
                    energia.append(row['energia'])
                    soc.append(row['soc'])
                    velocidad.append(row['velocidad'])
                    corriente_objetivo.append(row['corriente_objetivo'])
                    tiempo_objetivo.append(row['tiempo_objetivo'])
                    print(carga + f"-[{i}/{largo}]-" + reset)
                    i += 1
                    #time.sleep(0.01)
                    
            csv_file.close()
            cache(i, largo, x.name, tiempo_programa, tension, corriente, potencia, energia,soc, velocidad, corriente_objetivo, tiempo_objetivo)

        except Exception as e:
            print(error + f"Error cargando datos desde: {x}")
            print("Puede que se encuentre vacio.")
            print(e)
            print(reset)
            time.sleep(2)
            return 0


def cache(i, largo,nombre, tiempo_programa, tension, corriente, potencia, energia,soc, velocidad, corriente_objetivo, tiempo_objetivo):

    print(carga + "Almacenando datos en: cache.txt" + reset)
    fecha = int(time.time())
    archivo = str(fecha) + " - " +nombre
    cache = (pathlib.Path(__file__).parent / f'./Datos_Medidor/{archivo}')

    try:
        with open(cache, 'w') as csv_file:
            fields = ['tiempo_programa', 'tension', 'corriente', 'potencia', 'energia','soc', 'velocidad', 'corriente_objetivo', 'tiempo_objetivo']
            writer = csv.DictWriter(csv_file, fieldnames=fields, delimiter = ',')
            writer.writeheader()
            for x in range(len(tiempo)):
                writer.writerow({'tiempo_programa': str(tiempo[x]),
                                 'tension': str(tension[x]),
                                 'corriente': str(corriente[x]),
                                 'potencia': str(potencia[x]),
                                 'energia': str(energia[x]),
                                 'soc': str(soc[x]),
                                 'velocidad': str(velocidad[x]),
                                 'corriente_objetivo': str(corriente_objetivo[x]),
                                 'tiempo_objetivo': str(tiempo_objetivo[x])})

                print(carga + f"-[{i}/{largo}]-" + reset)
                i += 1
                #time.sleep(0.01)

        csv_file.close()
        return 0

    except Exception as e:
        print(error + f"Error cargando datos desde: {cache}")
        print(e)
        print(reset)
        time.sleep(2)
        return 0
