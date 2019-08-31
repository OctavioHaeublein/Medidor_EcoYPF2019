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
    
    os.system('cls')
    directorio = pathlib.Path(archivo)
    
    for x in directorio.iterdir():
        tiempo = []
        tension = []
        corriente = []
        potencia = []
        energia = []
        velocidad = []
        potencia_objetivo = []
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
                    tiempo.append(row['tiempo'])
                    tension.append(row['tension'])
                    corriente.append(row['corriente'])
                    potencia.append(row['potencia'])
                    energia.append(row['energia'])
                    velocidad.append(row['velocidad'])
                    potencia_objetivo.append(row[potencia_objetivo])
                    print(carga + f"-[{i}/{largo}]-" + reset)
                    i += 1
                    #time.sleep(0.01)
                    
            csv_file.close()
            cache(i, largo, x.name, tiempo, tension, corriente, potencia, energia, velocidad, potencia_objetivo)

        except Exception as e:
            print(error + f"Error cargando datos desde: {x}")
            print("Puede que se encuentre vacio.")
            print(e)
            print(reset)
            time.sleep(2)
            return 0


def cache(i, largo,nombre, tiempo, tension, corriente, potencia, energia, velocidad, potencia_objetivo):

    print(carga + "Almacenando datos en: cache.txt" + reset)
    fecha = int(time.time())
    archivo = str(fecha) + " - " +nombre
    cache = (pathlib.Path(__file__).parent / f'./datos/{archivo}')

    try:
        with open(cache, 'w') as csv_file:
            fields = ['tiempo', 'tension', 'corriente', 'potencia', 'energia', 'velocidad']
            writer = csv.DictWriter(csv_file, fieldnames=fields, delimiter = ',')
            writer.writeheader()
            for x in range(len(tiempo)):
                writer.writerow({'tiempo': str(tiempo[x]),
                                 'tension': str(tension[x]),
                                 'corriente': str(corriente[x]),
                                 'potencia': str(potencia[x]),
                                 'energia': str(energia[x]),
                                 'velocidad': str(velocidad[x]),
                                 'potencia_objetivo': str(potencia_objetivo[x])})

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
