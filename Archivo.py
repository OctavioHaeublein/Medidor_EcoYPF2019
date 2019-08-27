import csv
import time
import pathlib
import datetime

tiempo = []
tension = []
corriente = []
potencia = []
energia = []
velocidad = []

def cargar(archivo, directorio):
    
    datos = directorio + archivo
    i = 1
    
    try:
        with open(datos, 'r') as csv_file:
            reader = csv.DictReader(csv_file)
            largo = sum(2 for row in reader)

        csv_file.close()

    except Exception as e:
        print(f"Error cargando datos desde: {datos}")
        print("Puede que se encuentre vacio.")
        print(e)
        time.sleep(2)
        return 0

    try:
        with open(datos, 'r') as csv_file:
            reader = csv.DictReader(csv_file)
            for row in reader:
                tiempo.append(row['tiempo'])
                tension.append(row['tension'])
                corriente.append(row['corriente'])
                potencia.append(row['potencia'])
                energia.append(row['energia'])
                velocidad.append(row['velocidad'])
                print(f"-[{i}/{largo}]-")
                i += 1
                time.sleep(0.01)
                
        csv_file.close()
        cache(i, largo, archivo)

    except Exception as e:
        print(f"Error cargando datos desde: {datos}")
        print("Puede que se encuentre vacio.")
        print(e)
        time.sleep(2)
        return 0


def cache(i, largo,nombre):

    print("Almacenando datos en: cache.txt")
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
                                 'velocidad': str(velocidad[x])})

                print(f"-[{i}/{largo}]-")
                i += 1
                time.sleep(0.01)

        csv_file.close()
        return 0

    except Exception as e:
        print(f"Error cargando datos desde: {cache}")
        print(e)
        time.sleep(2)
        return 0
