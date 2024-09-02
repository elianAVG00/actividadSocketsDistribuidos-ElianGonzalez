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
    // inicio el generador de numeros aleatorios
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
    // inicio el generador de numeros aleatorios
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
    int wsaError;
    WORD versionWSA = MAKEWORD(2, 2);
    wsaError = WSAStartup(versionWSA, &wsadata); // inicio el winsock
    if (wsaError != 0)
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

    SOCKET servidor, aceptoSocketCliente;
    servidor = INVALID_SOCKET;
    servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor == INVALID_SOCKET)
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

    //*** En el caso de querer setear una ip especifica
    // const TCHAR* ip = _T("192.168.0.8");// localhost, ese es el ip que refiere a la maquina local
    // InetPton(AF_INET, ip, &configDireccionesServidor.sin_addr.s_addr);// si quiero asignar una ip perso para el servidor
    //***

    struct sockaddr_in configDireccionesServidor;
    configDireccionesServidor.sin_addr.s_addr = INADDR_ANY; // seteo el IP any, Escuchar en todas las interfaces
    configDireccionesServidor.sin_family = AF_INET;
    configDireccionesServidor.sin_port = htons(puerto); // seteo el puerto
    int tamañoDirecciones = sizeof(configDireccionesServidor);

    if (bind(servidor, (SOCKADDR *)&configDireccionesServidor, sizeof(configDireccionesServidor)) == SOCKET_ERROR)
    {
        DWORD estado = WSAGetLastError();
        wprintf(L"Fallo el connect %ld\n", estado);
        closesocket(servidor);
        WSACleanup();
    }
    else
    {
        printf("bindeo correcto \n");
    }
    //-------------------------------------------------------------------------------
    // Comienza la escucha
    if (listen(servidor, 1) == SOCKET_ERROR)
    {
        DWORD estado = WSAGetLastError();
        wprintf(L"Fallo el listen %ld\n", estado);
    }
    else
    {
        printf("listen correcto, esperando conexiones \n");
    }
    bool clienteConectado = false;
    while(1){
    
        //-------------------------------------------------------------------------------
        // Acepto la conexion con el cliente
        printf("Esperando conexiones...\n");
                    // system("pause");

        aceptoSocketCliente = accept(servidor, (struct sockaddr *)&configDireccionesServidor, (socklen_t *)&tamañoDirecciones);
        if (aceptoSocketCliente == INVALID_SOCKET)
        {
            DWORD estado = WSAGetLastError();
            wprintf(L"Fallo el accept %ld\n", estado);
            WSACleanup();
        }
        else
        {
            clienteConectado = true;
            printf("accept correcto\n");
        }

        //-------------------------------------------------------------------------------
        // Leer datos del cliente
        while(clienteConectado == true){
            char buffer[SIZE_BUFFER];
            char respuesta[SIZE_BUFFER];
            int opcion;
            //recibo la opcion elegida por el cliente
            int dataReceive = recv(aceptoSocketCliente, buffer, sizeof(buffer), 0);
            if( dataReceive < 0)
            {
                snprintf(respuesta, SIZE_BUFFER, "Opcion invalida. Elija entre 1 y 2.\n");
                send(aceptoSocketCliente, respuesta, sizeof(respuesta), 0);
            }
            else if (dataReceive == 0) {
                printf("Cliente desconectado.\n");
                clienteConectado = false;
                break;
            }
            else{

                opcion = atoi(buffer+1);
                if(opcion == 1 || opcion == 2){
                    snprintf(respuesta, SIZE_BUFFER, "ok\n");
                }
                else if(opcion == 0){
                    snprintf(respuesta, SIZE_BUFFER, "Error: Ingrese un digito valido.\n");
                }
                else{
                    snprintf(respuesta, SIZE_BUFFER, "Opcion invalida. Elija entre 1 y 2.\n");
                }

                send(aceptoSocketCliente, respuesta, sizeof(respuesta), 0);
            }

            if(opcion == 1){ // Generador de nombres de usuario
                if( recv(aceptoSocketCliente, buffer, sizeof(buffer), 0) <= 0)// Reciba la longitud
                {
                    clienteConectado = false;
                }
            
                int longitud = atoi(buffer + 1);
                if (longitud > 5 && longitud < 15)
                {
                    snprintf(respuesta, SIZE_BUFFER, "%s\n", generadorUsuario(longitud));
                }
                else if(longitud == 0){
                    snprintf(respuesta, SIZE_BUFFER, "Error: Ingrese un digito valido.\n");
                }
                else
                {
                    snprintf(respuesta, SIZE_BUFFER, "Error: Longitud de nombre de usuario inválida.\n");
                }
                // Enviar respuesta al cliente
                send(aceptoSocketCliente, respuesta, sizeof(respuesta), 0);
            }
            else if(opcion == 2){ // Generador de contraseñas
                if( recv(aceptoSocketCliente, buffer, sizeof(buffer), 0) < 0)// Reciba la longitud
                {
                    printf("recv failed");
                }
                int longitud = atoi(buffer + 1);
                if (longitud >= 8 && longitud < 50)
                {
                    snprintf(respuesta, SIZE_BUFFER, "%s\n", generadorContraseña(longitud));
                }
                else if(longitud == 0){
                    snprintf(respuesta, SIZE_BUFFER, "Error: Ingrese un digito valido.\n");
                }
                else
                {
                    snprintf(respuesta, SIZE_BUFFER, "Error: Longitud de contraseña inválida.\n");
                }

                // Enviar respuesta al cliente
                send(aceptoSocketCliente, respuesta, sizeof(respuesta), 0);
            }
        }
       
        // Cerrar conexión
        printf("Fin de la conexion\n");
        closesocket(aceptoSocketCliente);
    }
    

    return 0;
}