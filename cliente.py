import socket

ip = "192.168.0.8"  # as both code is running on same pc
puerto = 8888  # socket server port number

client_socket = socket.socket()  # instantiate
client_socket.connect((ip, puerto))  # connect to the server

print("Bienvenido al Menu del Cliente")
print("1. Generador de nombres de usuario")
print("2. Generador de contraseñas")
print("Elija la opcion requerida:   ")
opcion = 'a' + input() # agrego el 'a' como caracter por q me genera conflicto al hacer el atoi en server
client_socket.send(opcion.encode('utf-8'))
respuestaOpcion = client_socket.recv(1024).decode('utf-8','ignore').splitlines()[0] # recibo respuesta del servidor
print("------------------------------")
if respuestaOpcion == "ok":
    if opcion[1:] == "1":
        print("Generar nombre de usuario")
        print("Ingrese la longitud deseada: ")
        longuitud = input()
        client_socket.send(('a' + longuitud).encode('utf-8'))
        nombreUsuario = client_socket.recv(1024).decode('utf-8','ignore').splitlines()[0] #mejorar el problema es que recibo demasiadas cosas ademas de el nombreUsuario entonces poniendo la longuitud hago un substring para que solo traiga el largo de la palabra q necesito, pero deberia ver como hacer para q el encode lo haga solo
        print(nombreUsuario)
        print("Respuesta del servidor: " + nombreUsuario)
    elif opcion[1:] == "2":
        print("Generar contraseña")
        print("Ingrese la longitud deseada: ")
        longuitud =  'a' + input()
        client_socket.send(longuitud.encode('utf-8'))
        contraseña = client_socket.recv(1024).decode('utf-8','ignore').splitlines()[0] #mejorar
        print("Respuesta del servidor: " + contraseña)
else:
    print(respuestaOpcion)