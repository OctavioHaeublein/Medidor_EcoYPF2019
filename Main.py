import Archivo

print("----------------------------------------")
print("1. Cargar datos")
print("2. Graficar")
print("3. Salir")
print("----------------------------------------")
entrada = input("Opción: ")

if(entrada == "1"):
    directorio = input("Introduzca el directorio del archivo: ")
    archivo = input("Introduzca el nombre del archivo: ")
    Archivo.cargar(archivo, directorio)

if(entrada == "2"):
    print("----------------------------------------")
    print("1. Todo")
    print("2. Velocidad")
    print("3. Tensión")
    print("4. Corriente")
    print("5. Potencia")
    print("6. Energía")
    print("----------------------------------------")
    entrada = input("opcion: ")