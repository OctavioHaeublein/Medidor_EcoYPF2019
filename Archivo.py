import csv
import progressbar as pb
import time

tiempo = []
tension = []
corriente = []
potencia = []
energia = []
velocidad = []

def cargar(archivo, directorio):

    datos = directorio + archivo

    widgets = [' [', pb.Timer(), '] ','[',pb.Counter(),']',pb.BouncingBar(marker='#', left=' -|', right='|-', fill=' ', fill_left=True)]

    try:
        with open(datos, 'r') as csv_file:
            reader = csv.DictReader(csv_file)
            largo = sum(1 for row in reader)

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
            bar = pb.ProgressBar(max_value=largo)
            i=0
            for row in reader:
                tiempo.append(row['tiempo'])
                tension.append(row['tension'])
                corriente.append(row['corriente'])
                potencia.append(row['potencia'])
                energia.append(row['energia'])
                velocidad.append(row['velocidad'])
                bar.update(i+1)
                time.sleep(1)

        csv_file.close()
        return(tiempo, tension, corriente, potencia, energia, velocidad)

    except Exception as e:
        print(f"Error cargando datos desde: {datos}")
        print("Puede que se encuentre vacio.")
        print(e)
        time.sleep(2)
        return 0