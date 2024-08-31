#include <winsock2.h>
#include <windows.h>

#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <tchar.h>
#include <stdbool.h> // For bool, true, false

#define SIZE_BUFFER 1024

// Función para generar una contraseña
char *generadorContraseña(int longitud)
{
    const char alfanumericos[] = "0123456789abcdefghijklmnoqprstuvwyzxABCDEFGHIJKLMNOQPRSTUYWVZX";
    char *contraseña = (char *)malloc(longitud + 1);
    // Seed the random number generator once
    srand((unsigned int)time(NULL));
    
    for (int i = 0; i < longitud; i++)
    {
        contraseña[i] = alfanumericos[rand() % (sizeof(alfanumericos) - 1)];
    }

    contraseña[longitud] = '\0';    
    return contraseña;
}

// Función para generar un nombre de usuario
char *generadorUsuario(int longitud) {
    char *nombreUsuario = (char *)malloc(longitud + 1);
    char consonantes[] = "bcdfghjklmnpqrstvwxyz";
    char vocales[] = "aeiou";
    // Seed the random number generator once
    srand((unsigned int)time(NULL));
    bool esVocal = rand() % 2;

    for (int i = 0; i < longitud; i++) {
        if (esVocal) {
            nombreUsuario[i] = vocales[rand() % 5];
        } else {
            nombreUsuario[i] = consonantes[rand() % 21];
        }
        esVocal = !esVocal; // alternar entre vocal y consonante
    }

    nombreUsuario[longitud] = '\0';
    return nombreUsuario;
}

int main()
{

    // Con esto verifico que esta andando bien la libreria Ws2_32 de winsock en windows
    WSADATA wsadata;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsadata); // inicio el winsock
    if (wsaerr != 0)
    {
        printf("Winsock no anda\n");
    }
    else
    {
        printf("Winsock anda.\n");
        printf("Estado: %s\n", wsadata.szSystemStatus);
    }
    //-------------------------------------------------------------------------------
    // Hago el setup del socket

    SOCKET serverSocket, acceptSocket;
    serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        DWORD estado = WSAGetLastError();
        wprintf(L"Error con el socket %ld\n", estado);
        WSACleanup();
    }
    else
    {
        printf("Socket anda\n");
    }

    //-------------------------------------------------------------------------------
    // Bindeo del socket, el enlace, lo enlazo a un puerto y aun ip
    int puerto = 8888;
    // const TCHAR* ip = _T("192.168.0.8");// localhost, ese es el ip que refiere a la maquina local

    struct sockaddr_in configDireccionesServidor;
    // InetPton(AF_INET, ip, &configDireccionesServidor.sin_addr.s_addr);// si quiero asignar una ip perso para el servidor
    configDireccionesServidor.sin_addr.s_addr = INADDR_ANY; // seteo el IP any, Escuchar en todas las interfaces
    configDireccionesServidor.sin_family = AF_INET;
    configDireccionesServidor.sin_port = htons(puerto); // seteo el puerto
    int tamañoDirecciones = sizeof(configDireccionesServidor);

    if (bind(serverSocket, (SOCKADDR *)&configDireccionesServidor, sizeof(configDireccionesServidor)) == SOCKET_ERROR)
    {
        DWORD estado = WSAGetLastError();
        wprintf(L"Fallo el connect %ld\n", estado);
        closesocket(serverSocket);
        WSACleanup();
    }
    else
    {
        printf("bindeo correcto \n");
    }

    //-------------------------------------------------------------------------------
    // Comienza la escucha
    if (listen(serverSocket, 1) == SOCKET_ERROR)
    {
        DWORD estado = WSAGetLastError();
        wprintf(L"Fallo el listen %ld\n", estado);
    }
    else
    {
        printf("listen correcto, esperando conexiones \n");
    }
    //-------------------------------------------------------------------------------
    // Acepto la conexion con el cliente
    printf("Esperando conexiones...\n");

    acceptSocket = accept(serverSocket, (struct sockaddr *)&configDireccionesServidor, (socklen_t *)&tamañoDirecciones);
    if (acceptSocket == INVALID_SOCKET)
    {
        DWORD estado = WSAGetLastError();
        wprintf(L"Fallo el accept %ld\n", estado);
        WSACleanup();
    }
    else
    {
        printf("accept correcto\n");
    }

    //-------------------------------------------------------------------------------
    // Leer datos del cliente

    char buffer[SIZE_BUFFER];
    char response[SIZE_BUFFER];
    int opcion;
    //recibo la opcion elegida por el cliente
    if(  recv(acceptSocket, buffer, sizeof(buffer), 0) < 0)
	{
		printf("recv failed");
        snprintf(response, SIZE_BUFFER, "Opcion invalida. Elija entre 1 y 2.\n");
        send(acceptSocket, response, sizeof(response), 0);
	}
    else{

        opcion = atoi(buffer+1);
        if(opcion == 1 || opcion == 2){
            snprintf(response, SIZE_BUFFER, "ok\n");
        }else{
            snprintf(response, SIZE_BUFFER, "Opcion invalida. Elija entre 1 y 2.\n");
        }

        send(acceptSocket, response, sizeof(response), 0);
    }


    if(opcion == 1){ // Generador de nombres de usuario
        if( recv(acceptSocket, buffer, sizeof(buffer), 0) < 0)// Reciba la longitud
        {
            printf("recv failed");
        }
       
        int longitud = atoi(buffer + 1);
        if (longitud > 5 && longitud < 15)
        {
            snprintf(response, SIZE_BUFFER, "%s\n", generadorUsuario(longitud));
        }
        else
        {
            snprintf(response, SIZE_BUFFER, "Error: Longitud de nombre de usuario inválida.\n");
        }
        // Enviar respuesta al cliente
        send(acceptSocket, response, sizeof(response), 0);
    }
    else if(opcion == 2){ // Generador de contraseñas
        if( recv(acceptSocket, buffer, sizeof(buffer), 0) < 0)// Reciba la longitud
        {
            printf("recv failed");
        }
        int longitud = atoi(buffer + 1);
        if (longitud >= 8 && longitud < 50)
        {
            snprintf(response, SIZE_BUFFER, "%s\n", generadorContraseña(longitud));
        }
        else
        {
            snprintf(response, SIZE_BUFFER, "Error: Longitud de contraseña inválida.\n");
        }

        // Enviar respuesta al cliente
        send(acceptSocket, response, sizeof(response), 0);
    }
    // Cerrar conexión
    printf("Fin de la conexion\n");
    system("pause");
    WSACleanup();
    closesocket(acceptSocket);

    return 0;
}