#include <winsock2.h>
#include <windows.h>

#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#define SIZE_BUFFER 1024

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
    // Hago el setup del client socket

    SOCKET clientSocket;
    clientSocket = INVALID_SOCKET;
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
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
    // Hago la conexion con el servidor, no hace falta un bind porq lo hace todo en el connect

    int puerto = 8888;
    const TCHAR *ip = _T("192.168.0.8"); // localhost, ese es el ip que refiere a la maquina loca

    struct sockaddr_in configDireccionesCliente;
    InetPton(AF_INET, ip, &configDireccionesCliente.sin_addr.s_addr);

    // configDireccionesCliente.sin_addr.s_addr = INADDR_ANY;// seteo el IP any, Escuchar en todas las interfaces
    configDireccionesCliente.sin_family = AF_INET;
    configDireccionesCliente.sin_port = htons(puerto); // seteo el puerto

    if (connect(clientSocket, (struct sockaddr*)&configDireccionesCliente, sizeof(configDireccionesCliente)) == SOCKET_ERROR)
    {
        DWORD estado = WSAGetLastError();
        wprintf(L"Fallo el connect %ld\n", estado);
        WSACleanup();
    }
    else
    {
        printf("connect correcto, puede emepzar a enviar y recibir data\n");
    }

    //-------------------------------------------------------------------------------
    // Inicio menu del cliente

    char buffer[SIZE_BUFFER]; 
    char response[SIZE_BUFFER];
    int opcion;

    printf("Bienvenido al Menu del Cliente\n");
    printf("1. Generador de nombres de usuario\n");
    printf("2. Generador de contraseñas\n");
    printf("Elija la opcion requerida:  \n");
    scanf("%d", &opcion);
    printf("opcion : %d\n", opcion);
    snprintf(buffer, SIZE_BUFFER, "U%d", opcion);

    // Enviar datos al servidor
    send(clientSocket, buffer, sizeof(buffer), 0); // Envio opcion

    if( recv(clientSocket, response, sizeof(response), 0) < 0) // Recibo ok o fail del server
	{
        printf("recv failed");
	}

    if(strcmp(response, "ok\n") == 0){
        if(opcion == 1){
            int longitud;
            printf("Generar nombre de usuario\n");
            printf("Ingrese la longitud deseada: ");
            scanf("%d", &longitud);

            snprintf(buffer, SIZE_BUFFER, "U%d", longitud);
            // Enviar longuitud al servidor
            send(clientSocket, buffer, sizeof(buffer), 0);
            memset(buffer, 0, sizeof(buffer));

            // Leer respuesta del servidor
            if( recv(clientSocket, response, SIZE_BUFFER, 0) < 0)
            {
                printf("recv failed");
            }
        }
        else if(opcion == 2){
            if(strcmp(response, "ok\n") == 0){
                int longitud;
                printf("Generar contraseña\n");
                printf("Ingrese la longitud deseada: ");
                scanf("%d", &longitud);

                snprintf(buffer, SIZE_BUFFER, "U%d", longitud);
                // Enviar longuitud al servidor
                send(clientSocket, buffer, sizeof(buffer), 0);
                memset(buffer, 0, sizeof(buffer));

                // Leer respuesta del servidor
                if( recv(clientSocket, response, SIZE_BUFFER, 0) < 0)
                {
                    printf("recv failed");
                }
            }
        }
    }

    printf("Respuesta del servidor:     %s\n", response);
    WSACleanup();
    closesocket(clientSocket);
    printf("Fin de la conexion\n");
    system("pause");

    // Cerrar socket
    
    return 0;
}