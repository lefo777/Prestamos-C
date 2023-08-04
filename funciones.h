
/*    Antes de seleccionar una opcion del menu, usar pantalla completa con el compilador, si no se
    agranda la pantalla, la lista mostrada se desfigura debido al largo de los campos que tiene.
      Esto dificulta mucho la lectura y le visualizacion de lo realizado.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

#define max_num_clientes 100 //para modificar la cantidad de clientes totales que se guarden y puedan pedir prestamos
#define max_nombre 10
#define max_apellido 10

struct clientes {
    int orden;
    char nombre [max_nombre];
    char apellido [max_apellido];
    float importe;
    char tipo_credito [20];
    int dia;
    char mes [4];
    int anio;
    int numero_cuotas;
    float importe_cuota;
    float iva;
    float total_cuota;
    int activo;
};

// Emite los mensajes de los errores controlados.

void controlar_error(const char* mensaje) {
    printf("\n");
    printf("Error: %s\n", mensaje);
}

// Emite error si no se ingresa un numero

void validar_ingreso_entero(int * numero) {

    char buffer[100];
    fgets(buffer, sizeof(buffer), stdin);

    if (sscanf(buffer, "%d", numero) != 1) {

        controlar_error("Numero incorrecto.\n");
        validar_ingreso_entero(numero);

    }
}

// Emite error si hay problema con el float ingresado.

float validar_ingreso_float() {
    char entrada[100];
    float numero;

    while (1) {
        fgets(entrada, sizeof(entrada), stdin);

        // Intentar convertir la entrada a un n�mero float
        if (sscanf(entrada, "%f", &numero) == 1) {
            return numero;
        } else {
            controlar_error("Valor inv�lido. Ingrese un n�mero float v�lido: ");
        }
    }
}

//Comprueba que la fecha sea correcta.

int validar_fecha(int dia, int mes, int anio) {
    // Verificar rango de d�as
    if (dia < 1 || dia > 31) {
        return 0;  // D�a inv�lido
    }

    // Verificar rango de meses
    if (mes < 1 || mes > 12) {
        return 0;  // Mes inv�lido
    }

    // Verificar rango de a�os
    time_t tiempo_actual = time(NULL);
    struct tm* fecha_actual = localtime(&tiempo_actual);
    int anio_actual = fecha_actual->tm_year + 1900;  // A�o actual

    if (anio < anio_actual) {
        return 0;  // A�o inv�lido (menor al actual)
    }

    // Verificar si es febrero y el d�a no excede los 29 d�as en a�os bisiestos
    if (mes == 2) {
        int es_bisiesto = (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
        if (dia > 29 || (dia == 29 && !es_bisiesto)) {
            return 0;  // D�a inv�lido para febrero
        }
    }

    // Verificar d�as en meses que tienen 30 d�as
    if ((mes == 4 || mes == 6 || mes == 9 || mes == 11) && dia > 30) {
        return 0;  // D�a inv�lido para meses con 30 d�as
    }

    return 1;  // Fecha v�lida
}

// Cambio el numero de mes por los primeros 3 char del mes correspondiente

void cambio_formato_mes (int mes, char * nombre_mes){
    switch (mes) {
        case 1:
            strcpy(nombre_mes, "ene");
            break;
        case 2:
            strcpy(nombre_mes, "feb");
            break;
        case 3:
            strcpy(nombre_mes, "mar");
            break;
        case 4:
            strcpy(nombre_mes, "abr");
            break;
        case 5:
            strcpy(nombre_mes, "may");
            break;
        case 6:
            strcpy(nombre_mes, "jun");
            break;
        case 7:
            strcpy(nombre_mes, "jul");
            break;
        case 8:
            strcpy(nombre_mes, "ago");
            break;
        case 9:
            strcpy(nombre_mes, "sep");
            break;
        case 10:
            strcpy(nombre_mes, "oct");
            break;
        case 11:
            strcpy(nombre_mes, "nov");
            break;
        case 12:
            strcpy(nombre_mes, "dic");
            break;
        default:
            return 0;  // Mes inv�lido
    }
}

// obtengo el numero de mes de acuerdo al nombre del mes

int numero_mes(const char* nombre_mes) {

    if (strcmp(nombre_mes, "ene") == 0)
        return 1;
    else if (strcmp(nombre_mes, "feb") == 0)
        return 2;
    else if (strcmp(nombre_mes, "mar") == 0)
        return 3;
    else if (strcmp(nombre_mes, "abr") == 0)
        return 4;
    else if (strcmp(nombre_mes, "may") == 0)
        return 5;
    else if (strcmp(nombre_mes, "jun") == 0)
        return 6;
    else if (strcmp(nombre_mes, "jul") == 0)
        return 7;
    else if (strcmp(nombre_mes, "ago") == 0)
        return 8;
    else if (strcmp(nombre_mes, "sep") == 0)
        return 9;
    else if (strcmp(nombre_mes, "oct") == 0)
        return 10;
    else if (strcmp(nombre_mes, "nov") == 0)
        return 11;
    else if (strcmp(nombre_mes, "dic") == 0)
        return 12;

    return 0; // Mes inv�lido
}

// Como indica el nombre, separa el nombre del cliente en apellido y nombre.

char* separar_nombre_apellido(const char* nombre_completo, char* nombre, char* apellido) {
    const char* espacio = strchr(nombre_completo, ' ');
    if (espacio != NULL) {
        // Copiar el nombre hasta el espacio
        strncpy(nombre, nombre_completo, espacio - nombre_completo);
        nombre[espacio - nombre_completo] = '\0'; // Asegurarse de agregar el terminador nulo

        // Copiar el apellido despu�s del espacio
        strncpy(apellido, espacio + 1, max_apellido - 1);
        apellido[max_apellido - 1] = '\0'; // Asegurarse de agregar el terminador nulo
    }

    return apellido;
}

// Recibe un puntero al archivo prestamos.csv y un puntero a struct. Copia el contenido del archivo al struct.

void copiar_archivo_prestamo(FILE* archivo, struct clientes* ejem) {

    char linea[100];
    int dia, mes, anio;
    char nombre_mes [4] = "CAS";
    int j = 0; // contador para recorrer el archivo y no excederse al copiar datos

    //ignoro la primer linea del archivo prestamos.csv dado que hice mi propio encabezado
    fgets(linea, sizeof(linea), archivo);

    while (fgets(linea, sizeof(linea), archivo)) {

        if (j < max_num_clientes) {

            char* token = strtok(linea, ";"); // Carga el campo orden
            sscanf(token, "%d", &ejem[j].orden);

            token = strtok(NULL, ";"); // Pasa al siguente campo, cliente
            char* nombre_completo = token;

            // Funci�n para separar nombre y apellido
            char nombre[max_nombre];
            char apellido[max_apellido];
            separar_nombre_apellido(nombre_completo, nombre, apellido);

            // Copiar el nombre y apellido en la estructura ejem
            strncpy(ejem[j].nombre, nombre, sizeof(ejem[j].nombre) - 1);
            ejem[j].nombre[sizeof(ejem[j].nombre) - 1] = '\0';

            strncpy(ejem[j].apellido, apellido, sizeof(ejem[j].apellido) - 1);
            ejem[j].apellido[sizeof(ejem[j].apellido) - 1] = '\0';

            // Pasa el apellido a mayusculas.
            for (int i = 0; ejem[j].apellido[i] != '\0'; i++) {
                if (ejem[j].apellido[i] != ' ') {
                    ejem[j].apellido[i] = toupper(ejem[j].apellido[i]);
                }
            }

            token = strtok(NULL, ";"); // Obtener el siguiente campo
            sscanf(token, "%f", &ejem[j].importe);

            token = strtok(NULL, ";"); // Obtener el siguiente campo
            strncpy(ejem[j].tipo_credito, token, sizeof(ejem[j].tipo_credito) - 1);
            ejem[j].tipo_credito[sizeof(ejem[j].tipo_credito) - 1] = '\0'; // Asegurarse de agregar el terminador nulo

            // Convertir el tipo_credito a mayusculas
            for (int i = 0; ejem[j].tipo_credito[i] != '\0'; i++) {
                ejem[j].tipo_credito[i] = toupper(ejem[j].tipo_credito[i]);
            }

            token = strtok(NULL, ";");  // Obtener el siguiente campo
            sscanf(token, "%d", &dia);

            token = strtok(NULL, ";");  // Obtener el siguiente campo
            sscanf(token, "%d", &mes);

            token = strtok(NULL, ";");  // Obtener el siguiente campo
            sscanf(token, "%d", &anio);

            validar_fecha(dia, mes, anio);

            cambio_formato_mes (mes, nombre_mes);

            ejem[j].dia = dia;
            strncpy(ejem[j].mes, nombre_mes, sizeof(ejem[j].mes) - 1);
            ejem[j].mes[sizeof(ejem[j].mes) - 1] = '\0'; // Asegurar terminador nulo
            ejem[j].anio = anio;

            token = strtok(NULL, ";"); // Obtener el siguiente campo
            sscanf(token, "%d", &ejem[j].numero_cuotas);

            token = strtok(NULL, ";"); // Obtener el siguiente campo
            sscanf(token, "%f", &ejem[j].importe_cuota);

            token = strtok(NULL, ";"); // Obtener el siguiente campo
            sscanf(token, "%f", &ejem[j].iva);

            token = strtok(NULL, ";"); // Obtener el siguiente campo
            sscanf(token, "%f", &ejem[j].total_cuota);

        } else {

            break; // Sale del ciclo si se cargan todos los datos
        }

        j++; // Incrementar el contador dentro del bucle
    }

    // Rellenar con ceros a partir de la posici�n j
    for (; j < max_num_clientes; j++) {
        ejem[j].orden = 0;
        ejem[j].nombre[0] = '\0';
        ejem[j].apellido[0] = '\0';
        ejem[j].importe = 0.00;
        ejem[j].tipo_credito[0] = '\0';
        ejem[j].dia = 0;
        ejem[j].mes[0] = '\0';
        ejem[j].anio = 0;
        ejem[j].numero_cuotas = 0;
        ejem[j].importe_cuota = 0.00;
        ejem[j].iva = 0.00;
        ejem[j].total_cuota = 0.00;
        ejem[j].activo = 0;
    }
}

// Recibe un puntero al archivo binario y lo pasa al struct cuyo puntero tambien recibe.

void copiar_datos_binario(FILE* archivo_binario, struct clientes* ejem) {

    int i;
    for (i = 0; i < max_num_clientes; i++) {
        if (fread(&ejem[i], sizeof(struct clientes), 1, archivo_binario) != 1) {
            // Se encontr� el final del archivo o hubo un error de lectura
            break;
        }
    }

    // Rellenar los elementos no utilizados con valores nulos
    for (; i < max_num_clientes; i++) {
        ejem[i].orden = 0;
        ejem[i].nombre[0] = '\0';
        ejem[i].apellido[0] = '\0';
        ejem[i].importe = 0.0;
        ejem[i].tipo_credito[0] = '\0';
        ejem[i].dia = 0;
        ejem[i].mes[0] = '\0';
        ejem[i].anio = 0;
        ejem[i].numero_cuotas = 0;
        ejem[i].importe_cuota = 0.0;
        ejem[i].iva = 0.0;
        ejem[i].total_cuota = 0.0;
        ejem[i].activo = 0;
    }
}

// Opcion 1 del menu:
// Muestra la lista de clientes que se encuentra en prestamos.csv

void listar_clientes() {

    struct clientes ejem[max_num_clientes];
    int j;

    FILE* archivo_prestamo = fopen("prestamos.csv", "r");

    printf("****************************************************************************************************************************************\n");
    printf("| Orden |        Cliente        |  Importe  | Tipo de credito | Dia/  Mes  / Anio | Cuotas | Importe Cuota |   IVA  |   Total Cuota   |\n");
    printf("****************************************************************************************************************************************\n");

    if (archivo_prestamo != NULL) {

        copiar_archivo_prestamo(archivo_prestamo, ejem);

        for (j = 0; j < max_num_clientes; j++) {

            if (ejem[j].orden != 0){

                printf("|  %03d  | %10s %10s | %9.2f | %15s | %02d /  %3s  / %4d | %6d | %13.2f | %06.2f | %15.2f |\n",
                    ejem[j].orden, ejem[j].nombre, ejem[j].apellido, ejem[j].importe, ejem[j].tipo_credito,
                    ejem[j].dia, ejem[j].mes, ejem[j].anio, ejem[j].numero_cuotas, ejem[j].importe_cuota,
                    ejem[j].iva, ejem[j].total_cuota);
            }
        }

        fclose(archivo_prestamo);
    } else {
        controlar_error("No se pudo abrir el archivo prestamos.csv");
        perror("fopen");
    }
}

// Opcion 2 del menu:
// Creo un archivo binario con los datos que tiene prestamos.csv

void crear_binario() {

    int var_fwrite = max_num_clientes;

    // Verificar si ya existe el archivo creditos.dat
    if (access("creditos.dat", F_OK) == 0) {
        controlar_error("El archivo creditos.dat ya existe en la carpeta.\n");
        return;
    }

    struct clientes ejem[max_num_clientes];

    FILE* archivo_prestamo = fopen("prestamos.csv", "r");
    if (archivo_prestamo == NULL) {
        controlar_error("No se pudo abrir el archivo prestamos.csv\n");
        perror("fopen");
        return;
    }

    copiar_archivo_prestamo(archivo_prestamo, ejem);

    fclose(archivo_prestamo);

    FILE* archivo_binario = fopen("creditos.dat", "wb");
    if (archivo_binario == NULL) {
        controlar_error("No se pudo crear el archivo creditos.dat\n");
        perror("fopen");
        return;
    }

    // Escribir el n�mero de clientes en el archivo, usando max_numero_clientes directamente no funciona.
    fwrite(&var_fwrite, sizeof(int), 1, archivo_binario);

    // Escribir los datos de los clientes en el archivo
    fwrite(ejem, sizeof(struct clientes), max_num_clientes, archivo_binario);

    fclose(archivo_binario);

    printf("Archivo binario creditos.dat creado exitosamente.\n");
}

// Opcion 3 del menu:
// Copia los datos del archivo prestamos.csv al archivo binario

void migrar_datos() {

    struct clientes ejem[max_num_clientes];

    FILE* archivo_csv = fopen("prestamos.csv", "r");
    if (archivo_csv == NULL) {
        controlar_error("No se pudo abrir el archivo prestamos.csv");
        perror("fopen");
        return;
    }

    copiar_archivo_prestamo(archivo_csv, ejem);

    fclose(archivo_csv);

    for (int j = 0; j < max_num_clientes; j++) {

        if (ejem[j].orden != 0) {  ejem[j].activo = 1;}
    }

    FILE* archivo_binario = fopen("creditos.dat", "wb");
    if (archivo_binario == NULL) {
        controlar_error("No se encontro el archivo creditos.dat");
        perror("fopen");
        return;
    }

    fwrite(ejem, sizeof(struct clientes), max_num_clientes, archivo_binario);

    fclose(archivo_binario);

    printf("Los datos se han migrado correctamente a creditos.dat.\n");
}

// Opcion 4 del menu:
// Imprime los valores que tiene el archivo binario de acuerdo a lo seleccionado

void listar_datos() {

    int min_dia, max_dia, min_mes, max_mes, min_anio, max_anio, opcion;
    int mes;

    struct clientes ejem[max_num_clientes];

    FILE* archivo_binario = fopen("creditos.dat", "rb"); // Abrir en modo de lectura binaria

    if (archivo_binario == NULL) {
        printf("Error al abrir el archivo binario\n");
        perror("fopen");
        return;
    }

    copiar_datos_binario (archivo_binario, ejem);

    fclose(archivo_binario);

    printf("Seleccione una opcion:\n");
    printf("1. Listar todos (activos e inactivos)\n");
    printf("2. Listar solo los activos\n");
    printf("3. Filtrar por tipo de credito\n");
    printf("4. Filtrar por rango de tiempo\n");
    printf("Opcion: ");

    validar_ingreso_entero(&opcion);
    printf("\n");

    switch (opcion) {

        case 1:
            printf("****************************************************************************************************************************************************\n");
            printf("| Orden |        Cliente        |  Importe  | Tipo de credito | Dia/  Mes  / Anio | Cuotas | Importe Cuota |  IVA   |   Total Cuota   | activo |\n");
            printf("****************************************************************************************************************************************************\n");

            for (int j = 0; j < max_num_clientes; j++) {

                printf("|  %03d  | %10s %10s | %9.2f | %15s | %02d /  %3s  / %4d | %6d | %13.2f | %06.2f | %15.2f | %5d  |\n",
                    ejem[j].orden, ejem[j].nombre, ejem[j].apellido, ejem[j].importe, ejem[j].tipo_credito,
                    ejem[j].dia, ejem[j].mes, ejem[j].anio, ejem[j].numero_cuotas, ejem[j].importe_cuota,
                    ejem[j].iva, ejem[j].total_cuota, ejem[j].activo);
            }
            break;
        case 2:

            printf("****************************************************************************************************************************************************\n");
            printf("| Orden |        Cliente        |  Importe  | Tipo de credito | Dia/  Mes  / Anio | Cuotas | Importe Cuota |  IVA   |   Total Cuota   | activo |\n");
            printf("****************************************************************************************************************************************************\n");

            for (int j = 0; j < max_num_clientes; j++) {

                if (ejem[j].activo == 1) {
                    printf("|  %03d  | %10s %10s | %9.2f | %15s | %02d /  %3s  / %4d | %6d | %13.2f | %06.2f | %15.2f | %5d  |\n",
                        ejem[j].orden, ejem[j].nombre, ejem[j].apellido, ejem[j].importe, ejem[j].tipo_credito,
                        ejem[j].dia, ejem[j].mes, ejem[j].anio, ejem[j].numero_cuotas, ejem[j].importe_cuota,
                        ejem[j].iva, ejem[j].total_cuota, ejem[j].activo);
                }
            }
            break;
        case 3:
            printf("Seleccione el tipo de credito por el que quiere filtrar: \n");
            printf("1 para mostrar clientes A Solo Firma: \n");
            printf("2 para mostrar clientes Con Garantia: \n");
            printf("Opcion: ");

            validar_ingreso_entero(&opcion);
            printf("\n");

                switch (opcion) {

                    case 1:

                        printf("****************************************************************************************************************************************************\n");
                        printf("| Orden |        Cliente        |  Importe  | Tipo de credito | Dia/  Mes  / Anio | Cuotas | Importe Cuota |  IVA   |   Total Cuota   | activo |\n");
                        printf("****************************************************************************************************************************************************\n");

                        for (int j = 0; j < max_num_clientes; j++) {

                            if (strcasecmp(ejem[j].tipo_credito, "a sola firma") == 0){

                                printf("|  %03d  | %10s %10s | %9.2f | %15s | %02d /  %3s  / %4d | %6d | %13.2f | %06.2f | %15.2f | %5d  |\n",
                                    ejem[j].orden, ejem[j].nombre, ejem[j].apellido, ejem[j].importe, ejem[j].tipo_credito,
                                    ejem[j].dia, ejem[j].mes, ejem[j].anio, ejem[j].numero_cuotas, ejem[j].importe_cuota,
                                    ejem[j].iva, ejem[j].total_cuota, ejem[j].activo);
                            }
                        }

                        break;

                    case 2:

                        printf("***************************************************************************************************************************************************\n");
                        printf("| Orden |        Cliente        |  Importe  | Tipo de credito | Dia/  Mes  / Anio | Cuotas | Importe Cuota |  IVA   |   Total Cuota   | activo |\n");
                        printf("***************************************************************************************************************************************************\n");

                        for (int j = 0; j < max_num_clientes; j++) {
                            if (strcasecmp(ejem[j].tipo_credito, "con garantia") == 0) {

                                printf("|  %03d  | %10s %10s | %9.2f | %15s | %02d /  %3s  / %4d | %6d | %13.2f | %06.2f | %15.2f | %5d  |\n",
                                    ejem[j].orden, ejem[j].nombre, ejem[j].apellido, ejem[j].importe, ejem[j].tipo_credito,
                                    ejem[j].dia, ejem[j].mes, ejem[j].anio, ejem[j].numero_cuotas, ejem[j].importe_cuota,
                                    ejem[j].iva, ejem[j].total_cuota, ejem[j].activo);
                            }
                        }

                        break;
                }
            break;

        case 4:

            do {
                printf("Ingrese el rango de tiempo (solo numeros enteros):\n");
                printf("\n");

                printf("Dia minimo: ");
                validar_ingreso_entero(&min_dia);
                printf("Mes minimo: ");
                validar_ingreso_entero(&min_mes);
                printf("Anio minimo: ");
                validar_ingreso_entero(&min_anio);

                printf("\n");

                printf("Dia maximo: ");
                validar_ingreso_entero(&max_dia);
                printf("Mes maximo: ");
                validar_ingreso_entero(&max_mes);
                printf("Anio maximo: ");
                validar_ingreso_entero(&max_anio);

            } while (validar_fecha(min_dia, min_mes, min_anio) || validar_fecha(max_dia, max_mes, max_anio));

            printf("Listando registros por rango de tiempo dado:\n");
            printf("****************************************************************************************************************************************************\n");
            printf("| Orden |        Cliente        |  Importe  | Tipo de credito | Dia/  Mes  / Anio | Cuotas | Importe Cuota |  IVA   |   Total Cuota   | activo |\n");
            printf("****************************************************************************************************************************************************\n");

            for (int j = 0; j < max_num_clientes; j++) {

                if (ejem[j].dia >= min_dia && ejem[j].dia <= max_dia &&
                    (mes = numero_mes(ejem[j].mes)) >= min_mes && (mes = numero_mes(ejem[j].mes)) <= max_mes &&
                    ejem[j].anio >= min_anio && ejem[j].anio <= max_anio) {

                        printf("|  %03d  | %10s %10s | %9.2f | %15s | %02d /  %3s  / %4d | %6d | %13.2f | %06.2f | %15.2f | %5d  |\n",
                            ejem[j].orden, ejem[j].nombre, ejem[j].apellido, ejem[j].importe, ejem[j].tipo_credito,
                            ejem[j].dia, ejem[j].mes, ejem[j].anio, ejem[j].numero_cuotas, ejem[j].importe_cuota,
                            ejem[j].iva, ejem[j].total_cuota, ejem[j].activo);
                }
            }
            break;
        default:
            printf("Opcion invalida.\n");
            listar_datos();
            break;
    }
}

// Opcion 5 del menu:
// Crea un nuevo cliente y lo guarda en el archivo binario.

void alta_clientes() {

    struct clientes datos;
    struct clientes viejos[max_num_clientes];
    int numero_orden, numero, dia, mes, anio;
    char nombre_mes [4] = "CAS";

    FILE* archivo_binario = fopen("creditos.dat", "r");
    if (archivo_binario == NULL) {
        controlar_error("No se pudo abrir el archivo.");
        perror("fopen");
        return;
    }

    copiar_datos_binario(archivo_binario, viejos);

    fclose (archivo_binario);

    do {
        // Solicitar ingreso del n�mero de orden
        printf("Ingrese el numero de orden: ");
        validar_ingreso_entero(&numero_orden);

        if (viejos[numero_orden - 1].orden == numero_orden) {
            controlar_error("El numero de orden ingresado ya existe en el archivo.");
            printf("\n");
        } else {
            datos.orden = numero_orden;
        }
    } while (datos.orden == 0);

    printf("\n");
    printf("Ingrese el nombre: ");
    fflush(stdin);
    fgets(datos.nombre, sizeof(datos.nombre), stdin);
    datos.nombre[strcspn(datos.nombre, "\n")] = '\0'; // Eliminar el car�cter de nueva l�nea

    printf("\n");
    printf("Ingrese el apellido: ");
    fflush(stdin);
    fgets(datos.apellido, sizeof(datos.apellido), stdin);
    datos.apellido[strcspn(datos.apellido, "\n")] = '\0'; // Eliminar el car�cter de nueva l�nea

    // Convertir el apellido a may�sculas
    for (int i = 0; datos.apellido[i] != '\0'; i++) {
        datos.apellido[i] = toupper(datos.apellido[i]);
    }

    printf("\n");
    printf("Ingrese el importe a pedir: ");
    scanf("%d", &numero);
    datos.importe = numero;

    printf("\n");
    printf("Ingrese el numero de cuotas: ");
    scanf("%d", &numero);
    fflush(stdin);
    datos.numero_cuotas = numero;

    int finaliza;
    do {
        int opcion;

        printf("\n");
        printf("Seleccione el tipo de credito: \n");
        printf("1: A Sola Firma. \n");
        printf("2: Con Garantia. \n");
        fflush(stdin);

        validar_ingreso_entero(&opcion);
        switch (opcion) {
            case 1:
                strcpy(datos.tipo_credito, "A Sola Firma");
                finaliza = 1;
                break;
            case 2:
                strcpy(datos.tipo_credito, "Con Garantia");
                finaliza = 1;
                break;
            default:
                finaliza = 0;
                break;
        }
    } while (finaliza == 0);

    do {
        printf("\n");
        printf("Ingrese el rango de tiempo (solo numeros enteros):\n");
        printf("\n");

        printf("Dia de inicion de tramite: ");
        validar_ingreso_entero(&dia);
        printf("Mes de inicion de tramite: ");
        validar_ingreso_entero(&mes);
        printf("Anio de inicion de tramite: ");
        validar_ingreso_entero(&anio);

    } while (validar_fecha(dia, mes, anio));

    cambio_formato_mes (mes, nombre_mes);

    datos.dia = dia;
    strcpy(datos.mes, nombre_mes);
    datos.anio = anio;

    // Calcular importe de cuota, IVA y total de cuota
    datos.importe_cuota = datos.importe / datos.numero_cuotas;
    datos.iva = datos.importe_cuota * 0.21;
    datos.total_cuota = datos.importe_cuota + datos.iva;

    // Establecer el campo "activo" como 1 en el alta.
    datos.activo = 1;

    // Copia los datos nuevos al array viejos en la posicion elegida
    viejos[numero_orden - 1].orden = datos.orden;
    strcpy(viejos[numero_orden - 1].nombre, datos.nombre);
    strcpy(viejos[numero_orden - 1].apellido, datos.apellido);
    viejos[numero_orden - 1].importe = datos.importe;
    strcpy(viejos[numero_orden - 1].tipo_credito, datos.tipo_credito);
    viejos[numero_orden - 1].dia = datos.dia;
    strcpy(viejos[numero_orden - 1].mes, datos.mes);
    viejos[numero_orden - 1].anio = datos.anio;
    viejos[numero_orden - 1].numero_cuotas = datos.numero_cuotas;
    viejos[numero_orden - 1].importe_cuota = datos.importe_cuota;
    viejos[numero_orden - 1].iva = datos.iva;
    viejos[numero_orden - 1].total_cuota = datos.total_cuota;
    viejos[numero_orden - 1].activo = datos.activo;

    FILE* archivo_binario_2 = fopen("creditos.dat", "wb");
    if (archivo_binario == NULL) {
        controlar_error("No se pudo abrir el archivo.");
        perror("fopen");
        return;
    }

    // Escribir los datos actualizados en el archivo binario
    fseek(archivo_binario_2, 0, SEEK_SET);
    fwrite(viejos, sizeof(struct clientes), max_num_clientes, archivo_binario_2);

    fclose(archivo_binario_2);

    printf("El registro se ha dado de alta correctamente.\n");
}

// Opcion 6 del menu
// busca clientes en creditos.dat de acuerdo a los parametros elejidos y devuelve el valor del orden del
//      ultimo cliente encontrado, en caso de que al buscar por apellido aparezca mas de 1 cliente.

int buscar_clientes () {

    struct clientes ejem [max_num_clientes];
    char apellido [max_apellido];
    int orden, opcion, encontrado_orden;

    FILE* archivo_binario = fopen("creditos.dat", "r");
    if (archivo_binario == NULL) {
        controlar_error("No se pudo abrir el archivo.");
        perror("fopen");
        return;
    }

    copiar_datos_binario(archivo_binario, ejem);

    fclose(archivo_binario);

    do {

        printf("Seleccione la forma de buscar clientes: \n");
        printf("1 para buscar cliente por numero de orden: \n");
        printf("2 para buscar cliente por apellido: \n");
        printf("Opcion: ");

        validar_ingreso_entero(&opcion);
        printf("\n");

            switch (opcion) {

                case 1:

                    printf ("Ingrese el numero de orden que quiere buscar: \n");
                    validar_ingreso_entero(&orden);

                    if (ejem[orden - 1].orden == orden && ejem[orden - 1].orden != 0) {

                        printf("****************************************************************************************************************************************************\n");
                        printf("| Orden |        Cliente        |  Importe  | Tipo de credito | Dia/  Mes  / Anio | Cuotas | Importe Cuota |  IVA   |   Total Cuota   | activo |\n");
                        printf("****************************************************************************************************************************************************\n");

                        printf("|  %03d  | %10s %10s | %9.2f | %15s | %02d /  %3s  / %4d | %6d | %13.2f | %06.2f | %15.2f | %5d  |\n",
                                ejem[orden-1].orden, ejem[orden - 1].nombre, ejem[orden - 1].apellido, ejem[orden - 1].importe,
                                ejem[orden - 1].tipo_credito, ejem[orden - 1].dia, ejem[orden - 1].mes, ejem[orden - 1].anio,
                                ejem[orden - 1].numero_cuotas, ejem[orden - 1].importe_cuota, ejem[orden - 1].iva,
                                ejem[orden - 1].total_cuota, ejem[orden - 1].activo);

                        return ejem[orden - 1].orden;

                        break;

                    }

                    controlar_error ("El numero de orden ingresado no es valido o no forma parte del registro. \n");
                    break;

                case 2:

                    printf("Ingrese el apellido que desea buscar: \n");
                    fgets(apellido, sizeof(apellido), stdin);
                    apellido[strcspn(apellido, "\n")] = '\0';

                    // Convertir apellido a may�sculas
                    for (int i = 0; apellido[i] != '\0'; i++) {
                        apellido[i] = toupper(apellido[i]);
                    }

                    bool encontrado = false;

                    for (int i = 0; i < max_num_clientes; i++) {

                        if (strcasecmp(ejem[i].apellido, apellido) == 0) {

                            printf("***************************************************************************************************************************************************\n");
                            printf("| Orden |        Cliente        |  Importe  | Tipo de credito | Dia/  Mes  / Anio | Cuotas | Importe Cuota |  IVA   |   Total Cuota   | activo |\n");
                            printf("***************************************************************************************************************************************************\n");

                            printf("|  %03d  | %10s %10s | %9.2f | %15s | %02d /  %3s  / %4d | %6d | %13.2f | %06.2f | %15.2f | %5d  |\n",
                                ejem[i].orden, ejem[i].nombre, ejem[i].apellido, ejem[i].importe,
                                ejem[i].tipo_credito, ejem[i].dia, ejem[i].mes, ejem[i].anio,
                                ejem[i].numero_cuotas, ejem[i].importe_cuota, ejem[i].iva,
                                ejem[i].total_cuota, ejem[i].activo);

                            encontrado = true;
                            encontrado_orden = ejem[i].orden;
                        }
                    }

                    if (encontrado) {
                        return encontrado_orden;
                    } else {
                        controlar_error("No se encontraron clientes con el apellido especificado.\n");
                    }

            }
    }while (1);
}

// Opcion 7 del menu:
// Llama a la funcion buscar clientes para encontrar el numero de orden del cliente que desea modificar.
// Una vez cargados los datos cambiados se guarda el archivo y se finaliza el proceso.

void modificar_datos() {

    struct clientes ejem [max_num_clientes];
    char credito [20];
    char opcion [5];
    int orden = 0;
    float importe;

    FILE* archivo_binario = fopen("creditos.dat", "r+b");
    if (archivo_binario == NULL) {
        controlar_error("No se pudo abrir el archivo.");
        perror("fopen");
        return;
    }

    copiar_datos_binario(archivo_binario, ejem);

    printf("Buscando cliente a modificar: \n");
    orden = buscar_clientes () - 1;

    printf("\n");
    printf("ingrese el nuevo importe a cargar: \n");
    printf("(ingrese un numero flotante con punto (.) para la separacion de decimales)\n");

    importe = validar_ingreso_float();

    do {

        printf("Esta seguro que desea cambiar el valor del importe de: %s %s, cuyo numero de orden es: %d\n \n",
               ejem[orden].nombre, ejem[orden].apellido, ejem[orden].orden);

        printf("Ingrese S para cambiar al valor ingresado o N para cancelar el cambio.\n");
        fgets(opcion, sizeof(opcion), stdin);

        if (strcmp(opcion, "s\n") == 0 || strcmp(opcion, "si\n") == 0 ||
            strcmp(opcion, "S\n") == 0 || strcmp(opcion, "SI\n") == 0) {

            ejem[orden].importe = importe;
            break;

        } else if (strcmp(opcion, "n\n") == 0 || strcmp(opcion, "no\n") == 0 ||
                   strcmp(opcion, "N\n") == 0 || strcmp(opcion, "NO\n") == 0) {

            controlar_error("Se cancelo el cambio de importe.\n");
            break;

        } else {
            controlar_error("Opcion invalida.\n");
        }
    } while (1);

    if (strcmp(opcion, "s\n") == 0 || strcmp(opcion, "si\n") == 0 ||
        strcmp(opcion, "S\n") == 0 || strcmp(opcion, "SI\n") == 0) {

        do {

            printf("ingrese el tipo de credito que desea guardar: \n");
            printf("(A sola firma o Con garantia. Solo estas opciones funcionan)\n");
            fgets(credito, sizeof(credito), stdin);

            // Convertir a may�sculas
            for (int i = 0; credito[i] != '\0'; i++) {
                credito[i] = toupper(credito[i]);
            }

            if (strcmp(credito, "A SOLA FIRMA\n") == 0) {

                strcpy(ejem[orden].tipo_credito, "A SOLA FIRMA");
                break;

            } else if (strcmp(credito, "CON GARANTIA\n") == 0) {

                strcpy(ejem[orden].tipo_credito, "CON GARANTIA");
                break;

            }

            controlar_error ("Ingreso alguna letra incorrecta. \n");

        } while (1);

        // Escribir los datos actualizados en el archivo binario
        fseek(archivo_binario, 0, SEEK_SET);
        fwrite(ejem, sizeof(struct clientes), max_num_clientes, archivo_binario);

        fclose(archivo_binario);

        printf("El registro se ha dado de alta correctamente.\n");
    }

    controlar_error ("Debido a que se cancelo el cambio de importe,"
                     " se cancela el resto de la operacion");

}

// Opcion 8 del menu:
// Llama a la funcion buscar clientes para obtener el numero de ordel del cliente a dar de baja.
// Despues de obtener el valor del orden del cliente, confirma si desea darlo de baja o no.

void baja_logica() {

    struct clientes ejem [max_num_clientes];
    char credito [20];
    char opcion [5];
    int orden;
    float importe;

    FILE* archivo_binario = fopen("creditos.dat", "r+b");
    if (archivo_binario == NULL) {
        controlar_error("No se pudo abrir el archivo.");
        perror("fopen");
        return;
    }

    copiar_datos_binario(archivo_binario, ejem);

    printf("Buscando cliente a modificar: \n");
    orden = buscar_clientes () - 1;

    do {

        printf("Esta seguro que desea dar de baja a: %s %s, cuyo numero de orden es: %d?\n \n",
               ejem[orden].nombre, ejem[orden].apellido, ejem[orden].orden);

        printf("Ingrese S para continuar o N para cancelar el cambio.\n");
        fgets(opcion, sizeof(opcion), stdin);

        if (strcmp(opcion, "s\n") == 0 || strcmp(opcion, "si\n") == 0 ||
            strcmp(opcion, "S\n") == 0 || strcmp(opcion, "SI\n") == 0) {

            ejem[orden].activo = 0;

                printf("****************************************************************************************************************************************************\n");
                printf("| Orden |        Cliente        |  Importe  | Tipo de credito | Dia/  Mes  / Anio | Cuotas | Importe Cuota |  IVA   |   Total Cuota   | activo |\n");
                printf("****************************************************************************************************************************************************\n");

                for (int j = 0; j < max_num_clientes; j++) {

                    if (ejem[j].activo == 1) {
                        printf("|  %03d  | %10s %10s | %9.2f | %15s | %02d /  %3s  / %4d | %6d | %13.2f | %06.2f | %15.2f | %5d  |\n",
                            ejem[j].orden, ejem[j].nombre, ejem[j].apellido, ejem[j].importe, ejem[j].tipo_credito,
                            ejem[j].dia, ejem[j].mes, ejem[j].anio, ejem[j].numero_cuotas, ejem[j].importe_cuota,
                            ejem[j].iva, ejem[j].total_cuota, ejem[j].activo);
                    }
                }
            // Escribir los datos actualizados en el archivo binario
            fseek(archivo_binario, 0, SEEK_SET);
            fwrite(ejem, sizeof(struct clientes), max_num_clientes, archivo_binario);

            printf("El registro se ha dado de alta correctamente.\n");
        break;

        } else if (strcmp(opcion, "n\n") == 0 || strcmp(opcion, "no\n") == 0 ||
                   strcmp(opcion, "N\n") == 0 || strcmp(opcion, "NO\n") == 0) {

            controlar_error("Se cancelo la baja logica de: ");
            printf ("%s %s, cuyo numero de orden es: %d \n", ejem[orden].nombre, ejem[orden].apellido, ejem[orden].orden);
            break;

        } else {
            controlar_error("Opcion invalida.\n");
        }
    } while (1);

    fclose(archivo_binario);

}

// opcion 9 del menu:
// Crea los archivos con fecha para guardar los clientes procesados por la funcion baja logica
//      dado que abre el archivo creditos.dat y verifica si hay clientes con numero de orden
//      que tienen el campo activo en 0;

void baja_fisica() {

    struct clientes ejem [max_num_clientes];
    struct clientes bajas [max_num_clientes];
    time_t tiempo_actual;
    struct tm* tiempo_descompuesto;

    FILE* archivo_bajas;
    char nombre_bajas_archivo [max_num_clientes];

    // Obtener la fecha actual del sistema
    tiempo_actual = time(NULL);
    tiempo_descompuesto = localtime(&tiempo_actual);

    // Construir el nombre del archivo de bajas
    strftime(nombre_bajas_archivo, sizeof(nombre_bajas_archivo), "clientes_bajas_%Y%m%d.xyz", tiempo_descompuesto);

    // Abrir el archivo de bajas en modo escritura
    archivo_bajas = fopen(nombre_bajas_archivo, "w");

    if (archivo_bajas == NULL) {
        controlar_error("No se pudo crear el archivo de bajas.");
        perror("fopen");
        return;
    }

    FILE* archivo_binario = fopen("creditos.dat", "r+b");
    if (archivo_binario == NULL) {
        controlar_error("No se pudo abrir el archivo.");
        perror("fopen");
        fclose(archivo_bajas);
        return;
    }

    copiar_datos_binario(archivo_binario, ejem);

    int j = 0;

    for (int i = 0; i < max_num_clientes; i++) {
        if (ejem[i].orden != 0 && ejem[i].activo == 0) {  // Cliente inactivo

            bajas[j] = ejem[i];

    // Una vez que copio los clientes con '0' en el campo activo de mi archivo
    //     Creditos.dat vuelvo 0 a todos los campos ultilizados por ese cliente.
            ejem[i].orden = 0;
            ejem[i].nombre[0] = '\0';
            ejem[i].apellido[0] = '\0';
            ejem[i].importe = 0.0;
            ejem[i].tipo_credito[0] = '\0';
            ejem[i].dia = 0;
            ejem[i].mes[0] = '\0';
            ejem[i].anio = 0;
            ejem[i].numero_cuotas = 0;
            ejem[i].importe_cuota = 0.0;
            ejem[i].iva = 0.0;
            ejem[i].total_cuota = 0.0;

            j++;
        }
    }

    // Escribir los datos actualizados en ambos archivos y despues los cierro.
    fseek(archivo_bajas, 0, SEEK_SET);
    fwrite(bajas, sizeof(struct clientes), j, archivo_bajas);

    fseek(archivo_binario, 0, SEEK_SET);
    fwrite(ejem, sizeof(struct clientes), max_num_clientes, archivo_binario);

    fclose(archivo_binario);
    fclose(archivo_bajas);

    printf("Se han guardado los clientes inactivos correctamente en el archivo creado.\n");
}

//opcion 10 del menu:
//

void listar() {

    struct clientes ejem [max_num_clientes];
    FILE* archivo_bajas;
    char nombre_bajas_archivo[max_num_clientes];

    // Obtener la fecha actual del sistema
    time_t tiempo_actual = time(NULL);
    struct tm* tiempo_descompuesto = localtime(&tiempo_actual);

    // Construir el nombre del archivo de bajas
    strftime(nombre_bajas_archivo, sizeof(nombre_bajas_archivo), "clientes_bajas_%Y%m%d.xyz", tiempo_descompuesto);

    // Abrir el archivo de bajas en modo lectura
    archivo_bajas = fopen(nombre_bajas_archivo, "r");

    if (archivo_bajas == NULL) {
        controlar_error("No se pudo abrir el archivo de bajas.");
        perror("fopen");
        return;
    }

    size_t num_registros_leidos = fread(ejem, sizeof(struct clientes), max_num_clientes, archivo_bajas);

    printf("****************************************************************************************************************************************\n");
    printf("| Orden |        Cliente        |  Importe  | Tipo de credito | Dia/  Mes  / Anio | Cuotas | Importe Cuota |   IVA  |   Total Cuota   |\n");
    printf("****************************************************************************************************************************************\n");

    // Leer y mostrar cada registro del archivo
    for (size_t j = 0; j < num_registros_leidos; j++) {

        printf("|  %03d  | %10s %10s | %9.2f | %15s | %02d /  %3s  / %4d | %6d | %13.2f | %06.2f | %15.2f |\n",
               ejem[j].orden, ejem[j].nombre, ejem[j].apellido, ejem[j].importe, ejem[j].tipo_credito,
               ejem[j].dia, ejem[j].mes, ejem[j].anio, ejem[j].numero_cuotas, ejem[j].importe_cuota,
               ejem[j].iva, ejem[j].total_cuota);
    }

    // Cerrar el archivo
    fclose(archivo_bajas);
}

//Menu de opciones.

void desplegar_menu (){

    int opcion, orden;

    do {
        printf("\n==================  Menu de opciones  ==================\n");
        printf("0. Salir\n");
        printf("1. Listar archivo prestamos.csv\n");
        printf("2. Crear archivo binario\n");
        printf("3. Migrar datos de prestamo.csv al archivo binario\n");
        printf("4. Listar datos del archivo binario\n");
        printf("5. Alta cliente en el archivo binario\n");
        printf("6. Buscar cliente en el archivo binario\n");
        printf("7. Modificar datos del archivo binario\n");
        printf("8. Baja logica de clientes en el archivo binario\n");
        printf("9. Baja fisica de clientes en el archivo binario\n");
        printf("10. Listar archivo .xyz\n");
        printf("========================================================\n");
        printf("Ingrese el numero de opcion: ");
        fflush(stdin);
        validar_ingreso_entero(&opcion);

        switch (opcion) {
            case 0:
                printf("Finalizando progama.\n");
                break;
            case 1:
                printf("\n");
                listar_clientes();
                break;
            case 2:
                crear_binario();
                break;
            case 3:
                printf("\n");
                migrar_datos();
                break;
            case 4:
                printf("\n");
                listar_datos();
                break;
            case 5:
                printf("\n");
                alta_clientes();
                break;
            case 6:
                printf("\n");
                orden = buscar_clientes();
                break;
            case 7:
                printf("\n");
                modificar_datos();
                break;
            case 8:
                printf("\n");
                baja_logica();
                break;
            case 9:
                printf("\n");
                baja_fisica();
                break;
            case 10:
                printf("\n");
                listar();
                break;
            default:
                printf("\n");
                printf("Numero fuera del rango, ingrese un numero entre 0 y 10.\n");
                break;
        }
    } while (opcion != 0);

    return;
}
